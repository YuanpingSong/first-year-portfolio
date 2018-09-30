#!/usr/bin/env python3
import sys
import os
import csv
from math import ceil

# define data structures
class Superblock:
    def __init__(self, data):
        self.data = dict(
            num_blocks=int(data[1]), 
            num_inodes=int(data[2]), 
            block_size=int(data[3]), 
            inode_size=int(data[4]), 
            blocks_per_group=int(data[5]), 
            inodes_per_group=int(data[6]), 
            first_non_reserved_inode=int(data[7])
        )
    
class Group:
    def __init__(self, data):
        self.data = dict(
            group_num = int(data[1]),
            blocks_in_group = int(data[2]),
            inodes_in_group = int(data[3]),
            num_free_blocks = int(data[4]),
            num_free_inodes = int(data[5]),
            block_bitmap = int(data[6]),
            inode_bitmap = int(data[7]),
            block_num_of_first_inode = int(data[8])
        )

class Inode:
    def __init__(self, data):
        self.data = dict(
            inode_number=int(data[1]),
            file_type=data[2],
            mode=data[3], # can omit
            owner=int(data[4]),
            group=int(data[5]),
            link_count=int(data[6]),
            ctime=data[7],
            mtime=data[8],
            atime=data[9],
            file_size=int(data[10]),
            num_blocks=int(data[11]),
            data_blocks=[int(x) for x in data[12:24]],
            indirect_block=int(data[24]),
            double_indirect_block=int(data[25]),
            triple_indirect_block=int(data[26])
        )

class Dirent:
    def __init__(self, data):
        self.data = dict(
            parent_inode_number=int(data[1]),
            logical_byte_offset=int(data[2]),
            inode_number=int(data[3]),
            entry_length=int(data[4]),
            name_length=int(data[5]),
            name=data[6].rstrip()
        )

class IndirectReference:
    def __init__(self, data):
        self.data = dict(
            owner_inode=int(data[1]),
            level=int(data[2]),
            logical_offset_in_file=int(data[3]),
            source_block_number=int(data[4]),
            target_block_number=int(data[5])
        )    

# get command line argument
if len(sys.argv) != 2:
    print('Usage: ./lab3b filename', file=sys.stderr)
    sys.exit(1)

# initialize data structures
free_blocks = []
free_inodes = []
directories = []
allocated_inodes = []
indirect_blocks = []
groups = []
super_block = ''

# populate data strucutures
try:
    with open(sys.argv[1], 'r', newline='') as f:
        reader = csv.reader(f)
        for line in reader:
            if line[0] == 'SUPERBLOCK':
                super_block = Superblock(line)
            elif line[0] == 'GROUP':
                groups.append(Group(line))
            elif line[0] == 'BFREE':
                free_blocks.append(int(line[1]))
            elif line[0] == 'IFREE':
                free_inodes.append(int(line[1]))
            elif line[0] == 'INODE':
                allocated_inodes.append(Inode(line))
            elif line[0] == 'DIRENT':
                directories.append(Dirent(line))
            elif line[0] == 'INDIRECT':
                indirect_blocks.append(IndirectReference(line))
except OSError as e:
    print('Cannot open file!', file=sys.stderr)
    sys.exit(1)

hasError = False
# Inode Allocation Audit
allocated_inode_nums = []
for allocated_inode in allocated_inodes:
    allocated_inode_nums.append(allocated_inode.data['inode_number'])
del allocated_inode

for x in range(super_block.data['num_inodes']):
    if (x in allocated_inode_nums) and (x in free_inodes):
        print('ALLOCATED INODE {} ON FREELIST'.format(x))
        hasError = True
del x

for x in range(super_block.data['first_non_reserved_inode'],super_block.data['num_inodes']):
    if (x not in allocated_inode_nums) and (x not in free_inodes):
        print('UNALLOCATED INODE {} NOT ON FREELIST'.format(x))
        hasError = True
del x

# Directory Consistency Audit

link_counts_in_dirent = {}
link_counts_in_inode = {}
child_to_parent = {} # special case of root directory

for dirent in directories:
    if dirent.data['name'] != "'..'" and dirent.data['name'] != "'.'":
        child = dirent.data['inode_number']
        parent = dirent.data['parent_inode_number']
        child_to_parent[child] = parent
del dirent
child_to_parent[2] = 2 # special case of root directory

#print(child_to_parent)

for dirent in directories:
    #print(dirent.data)
    inode = dirent.data['inode_number']
    if inode < 1 or inode > super_block.data['num_inodes']:
        print('DIRECTORY INODE {} NAME {} INVALID INODE {}'.format(dirent.data['parent_inode_number'], dirent.data['name'], dirent.data['inode_number']))
        hasError = True
    elif inode not in allocated_inode_nums:
        print('DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}'.format(dirent.data['parent_inode_number'], dirent.data['name'], dirent.data['inode_number']))
        hasError = True
    elif inode in link_counts_in_dirent:
        link_counts_in_dirent[inode] += 1
    else:
        link_counts_in_dirent[inode] = 1
    
    name = dirent.data['name']
    dir_inode = dirent.data['parent_inode_number']
    dirent_inode = dirent.data['inode_number']
    if name == "'.'" and dir_inode != dirent_inode:
        print("DIRECTORY INODE {} NAME '.' LINK TO INODE {}SHOULD BE {}".format(dir_inode, dirent_inode, dir_inode))
        hasError = True
    elif name == "'..'" and dirent_inode != child_to_parent[dir_inode]:
        print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(dir_inode, dirent_inode, child_to_parent[dir_inode]))
        hasError = True
del dirent

for allocated_inode in allocated_inodes:
    inode = allocated_inode.data['inode_number']
    count = allocated_inode.data['link_count']
    link_counts_in_inode[inode] = count
del allocated_inode
#print(link_counts_in_dirent)
#print(link_counts_in_inode)

for (inode, count) in link_counts_in_inode.items():
    if count != link_counts_in_dirent.get(inode,0):
        print('INODE {} HAS {} LINKS BUT LINKCOUNT IS {}'.format(inode, link_counts_in_dirent.get(inode,0), count))
        hasError = True
del inode
del count


# Block Consistency Audits
allocated_blocks = set()
block_refs = {}

super_block_id = 0 if super_block.data['block_size'] > 1024 else 1
inode_table_size = ceil(super_block.data['num_inodes'] * super_block.data['inode_size'] / super_block.data['block_size'])

def isReserved(block_num):
    if block_num  > (3 + inode_table_size + super_block_id):
        return False
    else:
        return True

def check_pointer(source_type, block_num, inode_num, offset):
    if (block_num < 0 or block_num > super_block.data['num_blocks']):
        print("INVALID {} {} IN INODE {} AT OFFSET {}".format(source_type, block_num, inode_num, offset))
        hasError = True
    elif block_num != 0 and isReserved(block_num):
        print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(source_type, block_num, inode_num, offset))
        hasError = True

for inode in allocated_inodes:
    offset = 0
    inode_num = inode.data['inode_number']
    for block_num in inode.data['data_blocks']:
        if block_num == 0:
            continue
        if block_num in block_refs:
            block_refs[block_num].append((inode_num, offset, 'BLOCK'))
        else:
            block_refs[block_num] = [(inode_num, offset, 'BLOCK')]
        allocated_blocks.add(block_num)
        if not (inode.data['file_type'] == 's' and inode.data['file_size'] <= 60):
            check_pointer('BLOCK', block_num, inode_num, offset)
        offset+=1

    if not (inode.data['file_type'] == 's' and inode.data['file_size'] <= 60):
        check_pointer('INDIRECT BLOCK', inode.data['indirect_block'], inode_num, 12)
        check_pointer('DOUBLE INDIRECT BLOCK', inode.data['double_indirect_block'], inode_num, 268)
        check_pointer('TRIPLE INDIRECT BLOCK', inode.data['triple_indirect_block'], inode_num, 65804)

    indirect_block_num = inode.data['indirect_block']
    if indirect_block_num != 0 and indirect_block_num in block_refs:
        block_refs[indirect_block_num].append((inode_num, 12, 'INDIRECT BLOCK'))
    else:
        block_refs[indirect_block_num] = [(inode_num, 12, 'INDIRECT BLOCK')]
    double_indirect_block_num = inode.data['double_indirect_block']
    if double_indirect_block_num != 0 and double_indirect_block_num in block_refs:
        block_refs[double_indirect_block_num].append((inode_num, 268, 'DOUBLE INDIRECT BLOCK'))
    else:
        block_refs[double_indirect_block_num] = [(inode_num, 268, 'DOUBLE INDIRECT BLOCK')]
    triple_indirect_block_num = inode.data['triple_indirect_block']
    if triple_indirect_block_num != 0 and triple_indirect_block_num in block_refs:
        block_refs[triple_indirect_block_num].append((inode_num, 65804, 'TRIPLE INDIRECT BLOCK'))
    else:
        block_refs[triple_indirect_block_num] = [(inode_num, 65804, 'TRIPLE INDIRECT BLOCK')]
del inode

for indirect in indirect_blocks:
    source_type = ''
    if indirect.data['level'] == 1:
        source_type = 'INDIRECT BLOCK'
    elif indirect.data['level'] == 2:
        source_type = 'DOUBLE INDIRECT BLOCK'
    else:
        source_type = 'TRIPLE INDIRECT BLOCK'
    block_num = indirect.data['target_block_number']
    inode_num = indirect.data['source_block_number']
    offset = indirect.data['logical_offset_in_file']
    allocated_blocks.add(block_num)
    allocated_blocks.add(inode_num)
    if block_num in block_refs:
            block_refs[block_num].append((inode_num, offset, source_type))
    else:
        block_refs[block_num] = [(inode_num, offset, source_type)]
    check_pointer(source_type, block_num, inode_num, offset)
del indirect
    
for x in range(1, super_block.data['num_blocks']):
    if not isReserved(x):
        if x in free_blocks and x in allocated_blocks:
            print("ALLOCATED BLOCK {} ON FREELIST".format(x))
            hasError = True
        elif (not x in free_blocks) and (not x in allocated_blocks):
            print("UNREFERENCED BLOCK {}".format(x))
            hasError = True
del x

for (block_num, refs) in block_refs.items():
    if len(refs) > 1:
       for (inode, offset, source_type) in refs:
           print('DUPLICATE {} {} IN INODE {} AT OFFSET {}'.format(source_type, block_num, inode, offset))
           hasError = True
del block_num
del refs

if hasError:
    sys.exit(2)
else:
    sys.exit(0)