#!/usr/bin/python

import sys

#helper functions
def calc_max_block(superblock):
    start_index = superblock.find(",") + 1
    end_index = superblock.find(",", start_index)
    max_block_num = int(superblock[start_index: end_index])
    return max_block_num

def calc_min_block(superblock, group):
    #determine the position of the inode table from the group summary
    occurrence = 0
    start_index = 0
    while occurrence < 7:
        start_index = group.find(",", start_index) + 1
        occurrence += 1
    end_index = group.find(",", start_index)
    inode_table = int(group[start_index: end_index]) + 1
    

    #determine the block size
    #the block size is given after the third comma in the superblock
    start_index = 0
    occurrence = 0
    while occurrence < 3:
        start_index = superblock.find(",", start_index) + 1
        occurrence +=1
    end_index = superblock.find(",", start_index)
    block_size = int(superblock[start_index: end_index])
    
    #determine the inode size
    #the inode size is given right after the block size
    start_index = end_index + 1
    end_index = superblock.find(",", start_index)
    inode_size = int(superblock[start_index: end_index])

    #determine the number of inodes
    start_index = 0
    occurrence = 0
    while occurrence < 2:
        start_index = superblock.find(",", start_index) + 1
        occurrence += 1
    end_index = superblock.find(",", start_index)
    num_inodes = int(superblock[start_index: end_index])

    min_block_num = inode_table +(num_inodes*inode_size)/block_size
    return min_block_num
    
def ignore_file(line):
    occurrence = 0
    start_index = 0
    file_type = ""
    while True:
        start_index = line.find(",", start_index) + 1
        end_index = line.find(",", start_index)
        occurrence += 1
        if(occurrence == 2):
            file_type = line[start_index: end_index]
        if(occurrence == 10):
            size = line[start_index: end_index]
            break;
    if(file_type == "s" and int(size) <= 60):
            return True
    else:
        return False


#inode consistency checker
#def inode_check(file_lines, free_inodes, alloc_inodes):
        
#block consistency checker
def block_check(file_lines, max_block_num, min_block_num):
    alloc_blocks = {}
    #scan the file once to determine if there are duplicate blocks

    for line in file_lines:
        if("INODE" in line):
            #first check if this is a symbolic link with short length
            if(ignore_file(line)):
                continue

            #check each of the block pointers
            pointers_found = 0
            end_index = len(line)
            while pointers_found < 15:
                start_index = line.rfind(",", 0, end_index) + 1
                pointer = int(line[start_index: end_index])
                #add the block to the allocated blocks dictionary if the value is nonzero
                duplicate = False
                if(pointer != 0):
                    #check if the pointer is already in the dictionary
                    if pointer in alloc_blocks:
                        alloc_blocks[pointer] += 1
                        duplicate = True
                    else:
                        alloc_blocks[pointer] = 1

                end_index = start_index - 1
                error_type = ""
                if(((pointer < min_block_num or pointer > max_block_num) and pointer != 0) or duplicate):
                    #print out an error message
                    offset = 0
                    if(pointer < min_block_num and pointer > 0):
                        error_type = "RESERVED BLOCK "
                    else:
                        error_type = "INVALID BLOCK "
                    if(pointers_found == 0):
                        #we are looking at a triple indirect block
                        offset = 65804
                    elif(pointers_found == 1):
                        #we are looking at a double indirect block
                        offset = 268
                    elif(pointers_found == 2):
                        #we are looking at an indirect block
                        offset = 12
                    else:
                        offset = 15 - pointers_found - 1
                    start_inode = line.find(",") + 1
                    end_inode = line.find(",", start_inode)
                    inode_num = int(line[start_inode: end_inode])
                    #print out different message depending on level of indirection
                    if(offset < 12):
                        #direct block
                         print(error_type + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
                         
                    elif(offset == 12):
                        #indirect block
                        print(error_type + "INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
                    elif(offset == 268):
                        print(error_type + "DOUBLE INDIRECT " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
                    elif(offset == 65804):
                        print(error_type + "TRIPLE INDIRECT " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
                pointers_found += 1
    
    #finally confirm that each block is allocated to exactly one file
    for i in range(max_block_num):
        if(i in alloc_blocks and i in free_blocks):
            print("ALLOCATED BLOCK " + str(i) + " ON FREELIST")
        else if(i not in alloc_blocks and i not in free_blocks):
            print("UNREFERENCED BLOCK " + str(i))

    
def main():
    if(len(sys.argv) != 2):
        print("Error: must supply one argument")
        sys.exit(1)

    file_name = None;
    try:
        file_name = open(sys.argv[1], "r");
    except:
        print("Invalid file name")
        sys.exit(1)

    #store the block bitmap and inode bitmaps as sets
    #NOTE: the elements in both sets are all integers
    block_bitmap = set();
    inode_bitmap = set();

    #store all allocated inodes in a set
    alloc_inodes = set();

    max_block_num = -1;
    min_block_num = -1;
    #loop through the file, extracting all information we will need later
    #store each line in an array so that we can use it later
    file_lines = []
    superblock = ""
    group_summary = ""
    for line in file_name:
        file_lines.append(line)
        if("BFREE" in line):
            #extract the block number
            block_num = line[6:]
            block_bitmap.add(int(block_num))
        elif("IFREE" in line):
            inode_num = line[6:]
            inode_bitmap.add(int(inode_num))
        elif("INODE" in line):
            #extract the inode number and add to allocated list
            start_index = line.find(",") + 1;
            end_index = line.find(",", start_index)
            alloc_inodes.add(int(line[start_index: end_index]))
        elif("INDIRECT" in line):
            start_index = line.rfind(",") + 1;
            alloc_inodes.add(int(line[start_index:]))
        elif("SUPERBLOCK" in line):
            #calculate the highest possible block number
            superblock = line
            max_block_num = calc_max_block(line)
            if(group_summary != ""):
                min_block_num = calc_min_block(superblock, group_summary)
        elif("GROUP" in line):
            group_summary = line
            if(superblock != ""):
                min_block_num = calc_min_block(superblock, group_summary)

    block_check(file_lines, max_block_num, min_block_num)

if __name__ == "__main__":
    main()
