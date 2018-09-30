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

static unsigned period = 1;
static int debug = 0, useFahrenheit = 1, start_processing_commands = 0, started = 0;
mraa_aio_context temp_sensor = NULL;
mraa_gpio_context control = NULL;
char *filename = NULL;
FILE *fstream;
int log_fds;
pthread_t threads[3];

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
	while (1) {
		// Compute temperature
		float a = mraa_aio_read_float(temp_sensor);
		float R = 1023.0/a-1.0;
		R = R0*R;
		float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // remove this
		if (useFahrenheit) {
			temperature = C2F(temperature);
		}
		
		// Compute timestamp
		time_t clock;
		char time_string[30];
		struct tm* cur_time;
		
		time(&clock);
		cur_time = localtime(&clock);
		
		strftime(time_string, 30, "%H:%M:%S", cur_time);
		
		fprintf(stdout, "%s %.1f\n", time_string,temperature);
		fflush(stdout);
		
		if (filename) {
			fprintf(fstream, "%s %.1f\n", time_string,temperature);
			fflush(fstream);
		}
		
		if (debug) {
			fprintf(stdout, "start_processing_commands set to 1\n");
			fflush(stdout);
		}
		start_processing_commands = 1;
		sleep(period);
		
	}
	pthread_exit(NULL);
}

void* button_thread(void *arg __attribute__((unused))) {
	// Compute timestamp
	time_t clock;
	char time_string[30];
	struct tm* cur_time;
	
	while (1) {
		int state = mraa_gpio_read(control);
		if (state) { // high if pressed
			time(&clock);
			cur_time = localtime(&clock);
			
			strftime(time_string, 30, "%H:%M:%S", cur_time);
			fprintf(stdout, "%s SHUTDOWN\n", time_string);
			
			if (filename) {
				fprintf(fstream, "%s SHUTDOWN\n", time_string);
			}
			
			exit(EXIT_SUCCESS);
		}
	}
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
		
		num_c = read(STDIN_FILENO, &buffer + offset, 1024-offset);
		
		if (debug) {
			fprintf(stdout, "read %zd characters\n", num_c);
			fflush(stdout);
		}
		
		if (num_c < 0) {
			perror("lab4b");
			exit(EXIT_FAILURE);
		}
		int j = 0, i;
		for (i = 0; i < num_c; i++) {
			if (buffer[i] == '\n') {
				buffer[i] = 0;
				int len = i - j;
				if (filename) {
					fprintf(fstream, "%s\n", &buffer[j]);
				}
				
				if (len == 3 && strcmp(&buffer[j], "OFF") == 0) {
					time_t clock;
					char time_string[30];
					struct tm* cur_time;
					time(&clock);
					cur_time = localtime(&clock);
					
					strftime(time_string, 30, "%H:%M:%S", cur_time);
					fprintf(stdout, "%s SHUTDOWN\n", time_string);
					
					if (filename) {
						fprintf(fstream, "%s SHUTDOWN\n", time_string);
					}
					
					fclose(fstream);
					close(log_fds);
					
					exit(EXIT_SUCCESS);
				} else if (len == 4 && strcmp(&buffer[j], "STOP") == 0) {
					if(started) {pthread_kill(threads[0], SIGTERM);}
				} else if (len == 5 && strcmp(&buffer[j], "START") == 0) {
					if (!started) {
						if (pthread_create(&threads[0], NULL, report_thread, NULL) < 0) {
							fprintf(stderr, "Failed to create thread");
							exit(EXIT_FAILURE);
						}
					}
				} else if (len == 7 && strcmp(&buffer[j], "SCALE=F") == 0) {
					useFahrenheit = 1;
				} else if (len == 7 && strcmp(&buffer[j], "SCALE=C") == 0) {
					useFahrenheit = 0;
				} else if (memcmp(&buffer[j], "LOG",strlen("LOG")) == 0) {
					if (debug) {
						fprintf(stdout, "LOG Detected!\n");
						fflush(stdout);
					}
					if (filename) {
						fprintf(fstream, "%s", &buffer[j]);
					}
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
		
	}
}

int main(int argc, char * argv[]) {
	// process command line option
	static struct option long_options[] = {
		{"period", required_argument, NULL, 'p'},
		{"scale", required_argument, NULL, 's'},
		{"log", required_argument, NULL, 'l'},
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
			default: // '?'
				fprintf(stderr, "Usage: ./lab4b [--period=#] [--scale=C|F] [--log=filename]\n");
				exit(EXIT_FAILURE);
				break;
		} // end switch
	} // end while
	
	
	if (debug) {
		fprintf(stdout, "Period is: %d.\n", period);
		if (useFahrenheit) {
			fprintf(stdout, "We report in Fahrenheit.\n");
		} else {
			fprintf(stdout, "We report in Celsius.\n");
		}
		if (filename) {
			fprintf(stdout, "log file name is %s\n.", filename);
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
	
	control = mraa_gpio_init(60);
	if (control == NULL) {
		fprintf(stderr, "Failed to initialize gpio\n");
		mraa_deinit();
		return EXIT_FAILURE;
	} else if (debug) {
		fprintf(stdout, "Successfully initialized gpio.\n");
	}
	
	if (filename) {
		log_fds = open(filename, O_CREAT | O_WRONLY | O_APPEND | O_SYNC, S_IRUSR | S_IWUSR);
		if (log_fds < 0) {
			fprintf(stderr, "cannot write to non-writable file");
			exit(1);
		}
		fstream =fdopen(log_fds, "a");
		if (!fstream) {
			perror("lab4b");
		}
	}
	
	if (pthread_create(&threads[0], NULL, report_thread, NULL) < 0) {
		fprintf(stderr, "Failed to create thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&threads[1], NULL, button_thread, NULL) < 0) {
		fprintf(stderr, "Failed to create thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&threads[2], NULL, command_thread, NULL) < 0) {
		fprintf(stderr, "Failed to create thread");
		exit(EXIT_FAILURE);
	}
	
	if (started) {
		pthread_join(threads[0], NULL);
	}
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
	
	
	exit(EXIT_SUCCESS);
}
