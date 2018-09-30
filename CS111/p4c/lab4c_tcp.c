//
//  main.c
//  p4b
//
//  Created by Yuanping Song on 8/13/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <mraa.h>
#include <pthread.h>
#include <mraa/aio.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>

static int debug = 0; // debug flag
static unsigned period = 1; // period argument
static int useFahrenheit = 1; // unit argument
char id[10]; // id argument
char* host_name = NULL; // host argument
static int port_num = -1; // port number argument
static int start_processing_commands = 0; // do not process command until we have output
static int started = 0; // has report thread started running?
mraa_aio_context temp_sensor = NULL; // mraa struct
pthread_t threads[3]; // threads functionality
int sockfd; // tcp socket
char *filename = NULL; // log functionality
int log_fds;
char output_buf[100];

ssize_t safe_write(int fd, const void *buf, size_t nbyte){
	ssize_t num_c;
	if ((num_c = write(fd, buf, nbyte)) < 0) {
		perror("lab4c_tcp");
		exit(2);
	}
	return num_c;
}

ssize_t safe_read(int fd, void *buf, size_t nbyte) {
	ssize_t num_c;
	if((num_c = read(fd, buf, nbyte)) < 0) {
		perror("lab4c_tcp");
		exit(2);
	}
	return num_c;
}

void safe_create(pthread_t * tid, void* func) {
	if(pthread_create(tid, NULL, func, NULL) < 0){
		perror("lab4c_tcp");
		exit(2);
	}
}

void output(char *output_buf) {
	safe_write(sockfd, output_buf, strlen(output_buf));
	safe_write(log_fds, output_buf, strlen(output_buf));
}

void getTimeString(char *buffer, size_t size) {
	time_t clock;
	struct tm* cur_time;
	time(&clock);
	cur_time = localtime(&clock);
	strftime(buffer, size, "%H:%M:%S", cur_time);
}

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
float C2F(float C_temp) {
	return C_temp * 1.8 + 32;
}

void termination_handler(int signum __attribute__((unused))) {
	started = 0;
	pthread_exit(NULL);
}

void* report_thread(void *arg __attribute__((unused))) {
	started = 1;
	signal(SIGTERM, termination_handler);
	sprintf((char*)&output_buf, "ID=%s\n",id);
	output(output_buf);
	
	while (1) {
		// Compute temperature
		float a = mraa_aio_read_float(temp_sensor);
		float R = 1023.0/a-1.0;
		R = R0*R;
		float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15 + 120; // remove this
		if (useFahrenheit) {
			temperature = C2F(temperature);
		}
		
		// Compute timestamp
		char time_string[30];
		getTimeString(time_string, 30);
		sprintf((char*)&output_buf, "%s %.1f\n", time_string,temperature);
		
		output(output_buf);
		
		if (debug) {
			fprintf(stdout, "start_processing_commands set to 1\n");
			fflush(stdout);
		}
		start_processing_commands = 1;
		sleep(period);
	}
	pthread_exit(NULL);
}

void* command_thread(void *arg __attribute__((unused))) {
	char buffer[1024];
	long offset = 0;
	
	if (debug) {
		fprintf(stdout, "Command Thread Running!\n");
		fflush(stdout);
	}
	
	ssize_t num_c;
	while (1) {
		fflush(stdout);
		if (debug) {
			fprintf(stdout, "start_processing_commands is %d\n", start_processing_commands);
			fflush(stdout);
		}
		if (!start_processing_commands) {
			continue;
		}
		if (debug) {
			fprintf(stdout, "prepare to read!\n");
		}
		
		num_c = safe_read(sockfd, &buffer + offset, 1024-offset);
		
		if (debug) {
			fprintf(stdout, "read %zd characters\n", num_c);
			fflush(stdout);
		}
		
		int j = 0, i;
		for (i = 0; i < num_c; i++) {
			if (buffer[i] == '\n') {
				buffer[i] = 0;
				int len = i - j;
				safe_write(log_fds, &buffer[j], strlen(&buffer[j]));
				safe_write(log_fds, &"\n", 1);
				
				if (len == 3 && strcmp(&buffer[j], "OFF") == 0) {
					char time_string[30];
					getTimeString(time_string, 30);
					sprintf((char*)&output_buf, "%s SHUTDOWN\n", time_string);
					output(output_buf);
					close(log_fds);
					close(sockfd);
					exit(EXIT_SUCCESS);
				} else if (len == 4 && strcmp(&buffer[j], "STOP") == 0 && started) {
					pthread_kill(threads[0], SIGTERM);
				} else if (len == 5 && strcmp(&buffer[j], "START") == 0 && !started) {
					safe_create(&threads[0], report_thread);
				} else if (len == 7 && strcmp(&buffer[j], "SCALE=F") == 0) {
					useFahrenheit = 1;
				} else if (len == 7 && strcmp(&buffer[j], "SCALE=C") == 0) {
					useFahrenheit = 0;
				} else if (memcmp(&buffer[j], "LOG",strlen("LOG")) == 0) {
					if (debug) {
						fprintf(stdout, "LOG Detected!\n");
						fflush(stdout);
					}
					output(&buffer[j]);
				} else if(len >= 8 && memcmp(&buffer[j], "PERIOD=", 7) == 0) {
					int second = atoi(&buffer[j+7]);
					period = second;
				} // end if
				j = i + 1;
				if (debug) {
					fprintf(stdout, "num_c is %zd, j is %d\n", num_c, j);
					fflush(stdout);
				}
			} // end if
		} // end for
		
		if (j != num_c) { // command is not terminated
			if (debug) {
				fprintf(stdout, "num_c is %zd, j is %d\n", num_c, j);
				fflush(stdout);
			}
			memmove(buffer, buffer + j, num_c - j);
			offset = num_c - j;
		} else {
			offset = 0;
		}
	} //end while
}

int main(int argc, char * argv[]) {
	id[0] = 0;
	// process command line option
	static struct option long_options[] = {
		{"period", required_argument, NULL, 'p'},
		{"scale", required_argument, NULL, 's'},
		{"log", required_argument, NULL, 'l'}, // required option
		{"id", required_argument, NULL, 'i'}, // required option
		{"host", required_argument, NULL, 'h'}, // required option
		{0, 0, 0, 0} // last element of the array needs be filled this way
	};
	
	int opt;
	while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
		switch (opt) {
			case 'p':
			period = atoi(optarg);
			if (period < 1) {
				fprintf(stderr, "Period must not be less than 1!\n");
				exit(EXIT_FAILURE);
			}
			break;
			case 's':
			if (strlen(optarg) != 1 || (*optarg != 'C' && *optarg != 'F')) {
				fprintf(stderr, "Usage: [--scale=C|F]\n");
				exit(EXIT_FAILURE);
			}
			if (*optarg == 'C') {
				useFahrenheit = 0;
			}
			break;
			case 'l':
			filename = malloc(strlen(optarg) + 1);
			if (filename == NULL) {
				perror("lab4b");
				exit(EXIT_FAILURE);
			}
			strcpy(filename, optarg);
			break;
			case 'h':
			host_name = malloc(strlen(optarg) + 1);
			if (host_name == NULL) {
				perror("lab4b");
				exit(EXIT_FAILURE);
			}
			strcpy(host_name, optarg);
			break;
			case 'i':
			if (strlen(optarg) != 9) {
				fprintf(stderr, "Incorrect ID\n");
				exit(EXIT_FAILURE);
			}
			strcpy(id, optarg);
			break;
			default: // '?'
			fprintf(stderr, "Usage: ./lab4b_tcp [--period=#] [--scale=C|F] --log=filename --id=xxxxxxxxx --host=hostname portnumber\n");
			exit(EXIT_FAILURE);
			break;
		} // end switch
	} // end while
	
	if (argv[optind] != NULL && filename != NULL && host_name != NULL && id[0] != 0){
		port_num = atoi(argv[optind]);
	} else {
		fprintf(stderr, "Required Argument Missing!\n");
		exit(EXIT_FAILURE);
	}
	
	// id check
	if(strlen(id) != 9) {
		fprintf(stderr, "Worng id format!\n");
		exit(EXIT_FAILURE);
	}
	int i;
	for(i = 0; i < 9; i++) {
		if(!isdigit(id[i])) {
			fprintf(stderr, "Worng id format!\n");
			exit(EXIT_FAILURE);
		}
	}
	if(id[9] != 0) {
		fprintf(stderr, "Worng id format!\n");
		exit(EXIT_FAILURE);
	}
	
	if (debug) {
		fprintf(stdout, "Period is: %d.\n", period);
		if (useFahrenheit) {
			fprintf(stdout, "We report in Fahrenheit.\n");
		} else {
			fprintf(stdout, "We report in Celsius.\n");
		}
		if (filename) {
			fprintf(stdout, "log file name is %s.\n", filename);
		}
		if (host_name){
			fprintf(stdout, "host name is %s.\n", host_name);
		}
		if (id[0] != 0) {
			printf("id is %s.\n", id);
		}
		if (port_num != -1) {
			printf("port_num is %d.\n", port_num);
		}
	}
	
	/* initialize AIO */
	temp_sensor = mraa_aio_init(1); // aio 0 is mapped to mraa 1
	if (temp_sensor == NULL) {
		fprintf(stderr, "Failed to initialize AIO\n");
		mraa_deinit();
		return EXIT_FAILURE;
	} else if (debug) {
		fprintf(stdout, "Successfully initialized aio.\n");
	}
	
	// initialize log file
	log_fds = open(filename, O_CREAT | O_WRONLY | O_APPEND | O_SYNC, S_IRUSR | S_IWUSR);
	if (log_fds < 0) {
		fprintf(stderr, "cannot write to non-writable file");
		exit(1);
	} else if (debug) {
		fprintf(stdout, "created log file\n");
	} 
	
	// establish tcp connection to server
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "opening socket");
		exit(EXIT_FAILURE);
	}
	
	if ((server = gethostbyname(host_name)) == NULL) {
		fprintf(stderr, "no such host");
		exit(EXIT_FAILURE);
	}
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *) &serv_addr.sin_addr.s_addr, (char*) server -> h_addr_list[0], server -> h_length);
	serv_addr.sin_port = htons(port_num);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "error connecting");
		exit(EXIT_FAILURE);
	}
	
	if (debug) {
		printf("socket created\n");
	}
	
	// multi thread creation
	safe_create(&threads[0], report_thread);
	safe_create(&threads[2], command_thread);
	if (started) {
		pthread_join(threads[0], NULL);
	}
	pthread_join(threads[2], NULL);
	
	exit(EXIT_SUCCESS);
}
