//
//  main.c
//  p3a
//
//  Created by Yuanping Song on 8/19/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h> // link with -lm
#include <time.h>
#include <string.h>
#include "ext2_fs.h"

int ids = -1;
__u32 block_size = -1;

void getFormattedTime(char *buffer, size_t n, time_t* timer) {
	struct tm *time = gmtime(timer);
	if (!time) {
		fprintf(stderr, "Unable to convert time!\n");
		exit(EXIT_FAILURE);
	}
	strftime(buffer, n, "%D %H:%M:%S", time);
}

// This function prints the dirents contained within a single block
// args: block - pointer to the block to be printed
// 		 parent_inode_number - inode number of the containing directory
//       block_num - logical block number within file
void printDirentBlock(char * block, int parent_inode_number, int block_num) {
	__u32 idx = 0;
	struct ext2_dir_entry *entry_ptr = (struct ext2_dir_entry *) block;
	while (idx + 4 < block_size && entry_ptr -> inode) { // +4 ensures dereferencing pointer is not out of bounds. 
		char *name = malloc(entry_ptr->name_len + 1);
		if (name == NULL) {
			perror("lab3a");
			exit(EXIT_FAILURE);
		}
		memcpy(name, &entry_ptr->name, entry_ptr->name_len);
		name[entry_ptr->name_len] = 0;
		fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n",
				parent_inode_number,
				block_num * block_size + idx,
				entry_ptr->inode,
				entry_ptr->rec_len,
				(int) entry_ptr->name_len,
				name);
		idx += entry_ptr->rec_len;
		entry_ptr = (struct ext2_dir_entry *) (block + idx);
		free(name);
	}
}

// This function operates on a given indirect block and outputs the appropriate data,
// even when the indirect block is part of a directory
// args: block - the block to be printed
// 		 num - block number of the first argument
// 		 level - levels of indirection used by block
// 		 owner_inode - inode number of the owning file
// 		 offset - logical block offset of block within file
// 		 isDir - is block a data block of some directory?
void printIndirectBlock(char *block, int num, int level, __u32 owner_inode, __u32 offset, int isDir) {
	if (level == 0) {  // base case
		return;
	}
	__u32 counter = 0; // logical block number offset
	__u32 *block_num = (__u32*) block;
	char *sub_block = malloc(block_size); // data for recursive call
	if (sub_block == NULL) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	
	while (((char *)block_num <= block + block_size - 4) ) {
		if (*block_num != 0) {
			fprintf(stdout, "INDIRECT,%u,%u,%u,%u,%u\n",
					owner_inode,
					level,
					offset + counter,
					num,
					*block_num);

			ssize_t num_c = pread(ids, sub_block, block_size, *block_num * block_size);
			if (num_c < 0) {
				perror("lab3a");
				exit(EXIT_FAILURE);
			}
			printIndirectBlock(sub_block, *block_num, level - 1, owner_inode, offset + counter, isDir);
			if (isDir && level == 1) {
				printDirentBlock(sub_block, owner_inode, offset + counter); // check
			}
		}
		if (level == 1) {
			counter++;
		} else if (level == 2) {
			counter += block_size / 4;
		} else { // level = 3
			counter += block_size * block_size / 16;
		}
		block_num++;
	} // end while
	free(sub_block);
}

// This Function prints a single allocated inode by using informaiton in the inode table.
// args: inode_num - the inode number of the inode to be printed
// 		 fds_offset - the offset of the inode table entry from the beggining of the file.
void printInodeFromTable(__u32 inode_num, __u32 fds_offset){
	struct ext2_inode inode;
	ssize_t num_c = pread(ids, &inode, sizeof(struct ext2_inode), fds_offset);
	if (num_c < 0) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	if (inode.i_mode == 0 || inode.i_links_count == 0) { // unallocated inode
		return;
	}
	
	// filetype
	char filetype;
	if (S_ISREG(inode.i_mode)) {
		filetype = 'f';
	} else if (S_ISDIR(inode.i_mode)) {
		filetype = 'd';
	} else if (S_ISLNK(inode.i_mode)) {
		filetype = 's';
	} else {
		filetype = '?';
	}
	
	// mode
	__u16 mode = inode.i_mode;
	mode = mode << 4;
	mode = mode >> 4; // get the lower 12 bits
	
	// times
	char ctime[30], mtime[30], atime[30];
	time_t ct = inode.i_ctime, mt = inode.i_mtime, at = inode.i_atime;
	
	getFormattedTime((char *)&ctime, 30,  &ct);
	getFormattedTime((char *)&mtime, 30,  &mt);
	getFormattedTime((char *)&atime, 30,  &at);
	
	// first twelve fields
	fprintf(stdout, "INODE,%u,%c,%o,%u,%u,%u,%s,%s,%s,%u,%u",
			inode_num,
			filetype,
			mode,
			inode.i_uid,
			inode.i_gid,
			inode.i_links_count,
			ctime,
			mtime,
			atime,
			inode.i_size,
			inode.i_blocks);
	
	// last fifteen fields
	
	int i;
	// case: files and directories
	if (filetype == 'f' || filetype == 'd' || (filetype == 's' && inode.i_size > 60 )) {
		for (i = 0; i < EXT2_N_BLOCKS; i++) {
			fprintf(stdout, ",%d", inode.i_block[i]);
		}
	} else if (filetype == 's' && inode.i_size <= 60 ) {
		// nothing is printed
	} else {
		fprintf(stderr, "\nUnexpected filetype! in print Inode from table!\n");
	}
	fprintf(stdout, "\n");
	
	int isDir = 0;
	char * block_buffer = malloc(block_size);
	if (block_buffer == NULL) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	
	// directory direct blocks
	if (filetype == 'd') {
		isDir = 1;
		for (i = 0; i < 12; i++) { // direct blocks
			if (inode.i_block[i] == 0) {
				break;
			}
			num_c = pread(ids, block_buffer, block_size, block_size * inode.i_block[i]);
			if (num_c < 0) {
				perror("lab3a");
				exit(EXIT_FAILURE);
			}
			printDirentBlock(block_buffer, inode_num, i);
		}
	}
	
	// all indirect blocks - directory, file
	if (inode.i_block[12] != 0) {
		num_c = pread(ids, block_buffer, block_size, inode.i_block[12] * block_size);
		if (num_c < 0) {
			perror("lab3a");
			exit(EXIT_FAILURE);
		}
		printIndirectBlock(block_buffer, inode.i_block[12], 1, inode_num, 12, isDir);
	}
	if (inode.i_block[13] != 0 ) {
		num_c = pread(ids, block_buffer, block_size, inode.i_block[13] * block_size);
		if (num_c < 0) {
			perror("lab3a");
			exit(EXIT_FAILURE);
		}
		printIndirectBlock(block_buffer, inode.i_block[13], 2, inode_num, 12 + block_size / 4, isDir);
	}
	if (inode.i_block[14] != 0 ) {
		num_c = pread(ids, block_buffer, block_size, inode.i_block[14] * block_size);
		if (num_c < 0) {
			perror("lab3a");
			exit(EXIT_FAILURE);
		}
		printIndirectBlock(block_buffer, inode.i_block[14], 3, inode_num, 12 + block_size / 4 + block_size * block_size / 16, isDir);
	}
	
	free(block_buffer);
}

int main(int argc, char * argv[]) {
	long long i,j,k;
	// Get image name from command line
	if (argc != 2) {
		fprintf(stderr, "Usage: ./lab3a image\n");
		exit(EXIT_FAILURE);
	}
	char *image_name = argv[1];
	
	// open image file descriptor
	ids = open(image_name, O_RDONLY);
	if (ids < 0) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	
	// superblock summary
	const int superblock_offset = 1024;
	struct ext2_super_block m_super;
	if (pread(ids, &m_super, sizeof(struct ext2_super_block), superblock_offset) != sizeof(struct ext2_super_block)) {
		fprintf(stderr, "Error reading superblock location!\n");
		exit(EXIT_FAILURE);
	}
	if (m_super.s_magic != EXT2_SUPER_MAGIC) {
		fprintf(stderr, "Superblock not in expected location!\n");
		exit(EXIT_FAILURE);
	}
	
	block_size = 1024 << m_super.s_log_block_size;
	
	fprintf(stdout, "SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n",
			m_super.s_blocks_count,
			m_super.s_inodes_count,
			block_size,
			m_super.s_inode_size,
			m_super.s_blocks_per_group,
			m_super.s_inodes_per_group,
			m_super.s_first_ino);
	
	// end superblock summary, the following are available past this point:
	// m_super, block_size
	
	// group summary
	const int group_descriptor_table_offset = 2048;
	__u32 group_count = (m_super.s_blocks_count - 1) / m_super.s_blocks_per_group + 1;
	if (group_count != 1) {
		fprintf(stderr, "Error calculating group count! Got %u instead of 1!\n", group_count);
		exit(EXIT_FAILURE);
	}
	
	long long *block_counts = malloc(sizeof(long long) * group_count); // block count per group
	if (block_counts == NULL) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	long long *inode_counts = malloc(sizeof(long long) * group_count); // inode count per group
	if (inode_counts == NULL) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	
	struct ext2_group_desc *groups = malloc(sizeof(struct ext2_group_desc) * group_count); // group data structures
	if (groups == NULL) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < group_count; i++) { // fill group data structures
		ssize_t num_c = pread(ids, &groups[i], sizeof(struct ext2_group_desc), group_descriptor_table_offset + i * sizeof(struct ext2_group_desc));
		if (num_c == -1) {
			fprintf(stderr, "Failed to read in group descriptor table!\n");
			exit(EXIT_FAILURE);
		}
	}
	
	__u32 blocks_remaining = m_super.s_blocks_count;
	__u32 inodes_remaining = m_super.s_inodes_count;
	for (i = 0; i < group_count; i++) {
		block_counts[i] = (blocks_remaining < m_super.s_blocks_per_group) ? blocks_remaining : m_super.s_blocks_per_group;
		inode_counts[i] = (inodes_remaining < m_super.s_inodes_per_group) ? inodes_remaining : m_super.s_inodes_per_group;
		fprintf(stdout, "GROUP,%lld,%lld,%lld,%u,%u,%u,%u,%u\n",
				i,
				block_counts[i],
				inode_counts[i],
				groups[i].bg_free_blocks_count,
				groups[i].bg_free_inodes_count,
				groups[i].bg_block_bitmap,
				groups[i].bg_inode_bitmap,
				groups[i].bg_inode_table);
		
		blocks_remaining -= m_super.s_blocks_per_group;
		inodes_remaining -= m_super.s_inodes_per_group;
	}
	
	// end of group summary, the following are available past this point:
	// block_counts (pending destruction)
	// inode_counts (pending destruction)
	// groups		(pending destruction)
	
	
	// free block entries
	// need: m_super, groups, block_counts
	char *block_bitmap = malloc(block_size);
	if (block_bitmap == NULL) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < group_count; i++) {
		ssize_t num_c = pread(ids, block_bitmap, block_size, groups[i].bg_block_bitmap * block_size);
		if (num_c < 0) {
			perror("lab3a");
			exit(EXIT_FAILURE);
		}
		long long block_count = 1;
		for (j = 0; j < block_size; j++) {
			if (block_count > block_counts[i]) {
				break;
			}
			__u32 bit = 1;
			for (k = 0; k < 8; k++) {
				if (block_count > block_counts[i]) {
					break;
				}
				if (!(bit & block_bitmap[j])) {
					fprintf(stdout, "BFREE,%llu\n", i * m_super.s_blocks_per_group + block_count); // maybe add 1?
				}
				bit = bit << 1;
				block_count++;
			}
		}
	}
	free(block_bitmap);
	
	// free inode entries
	// need: m_super, groups, inode_counts
	char *inode_bitmap = malloc(block_size);
	if (inode_bitmap == NULL) {
		perror("lab3a");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < group_count; i++) {
		ssize_t num_c = pread(ids, inode_bitmap, block_size, groups[i].bg_inode_bitmap * block_size);
		if (num_c < 0) {
			perror("lab3a");
			exit(EXIT_FAILURE);
		}
		int inode_count = 1;
		for (j = 0; j < block_size; j++) {
			if (inode_count > inode_counts[i]) {
				break;
			}
			__u32 bit = 1;
			for (k = 0; k < 8; k++) {
				if (inode_count > inode_counts[i]) {
					break;
				}
				if (!(bit & inode_bitmap[j])) {
					fprintf(stdout, "IFREE,%llu\n", i * m_super.s_inodes_per_group + inode_count);
				} else {
					printInodeFromTable(
										(__u32) i * m_super.s_inodes_per_group + inode_count,
										groups[i].bg_inode_table * block_size + sizeof(struct ext2_inode) * (inode_count - 1));
				}
				bit = bit << 1;
				inode_count++;
			}
		}
	}
	free(inode_bitmap);
	
	// end of free block/inode summary
	
	// inode summmary
	
	
	
	free(block_counts);
	free(inode_counts);
	free(groups);
	close(ids);
	
	return 0;
}
