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

def inode_entry_check(line, max_block_num, min_block_num, alloc_blocks):
    #first check if this is a symbolic link with short length
    if(ignore_file(line)):
        return

    #check each of the block pointers
    pointers_found = 0
    end_index = len(line)
    while pointers_found < 15:
        start_index = line.rfind(",", 0, end_index) + 1
        pointer = int(line[start_index: end_index])
        end_index = start_index - 1
        #check if the block is a duplicate entry
        duplicate = False
        if(alloc_blocks[pointer] > 1):
            duplicate = True
        if((pointer < min_block_num or pointer > max_block_num or duplicate) and pointer != 0):
            #print out an error message
            offset = 0
            error_type = ""
            if(pointer < min_block_num and pointer > 0):
                error_type = "RESERVED "
            elif(pointer < 0 or pointer > max_block_num):
                error_type = "INVALID "
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
                if(duplicate):
                    print("DUPLICATE" + " BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
                if(error_type != ""):
                    print(error_type + "BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))

            elif(offset == 12):
                #indirect block
                if(duplicate):
                    print("DUPLICATE " + "INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
                if(error_type != ""):
                    print(error_type + "INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
            elif(offset == 268):
                if(duplicate):
                    print("DUPLICATE " + "DOUBLE INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
                if(error_type != ""):
                    print(error_type + "DOUBLE INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))

            elif(offset == 65804):
                if(duplicate):
                    print("DUPLICATE " + "TRIPLE INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))
                if(error_type != ""):
                    print(error_type + "TRIPLE INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode_num) + " AT OFFSET " + str(offset))

        pointers_found += 1

def indirect_entry_check(line, max_block_num, min_block_num, alloc_blocks):
    #scan the block to check if it is valid
    start_index = line.rfind(",") + 1
    pointer = int(line[start_index:])
    #check if the block is a duplicate
    duplicate = False
    if(alloc_blocks[pointer] > 1):
        duplicate = True
    if((pointer < min_block_num or pointer > max_block_num or duplicate) and pointer != 0):
        error_type = ""
        if(pointer < min_block_num and pointer > 0):
            error_type = "RESERVED "
        else:
            error_type = "INVALID "

        #determine the logical offset of the block and the inode 
        occurrences = 0
        start_index = 0
        inode = 0
        occurrence = 0
        lev_indirection = 0
        while occurrence < 3:
            if(occurrence == 1):
                #find the inode number
                end_index = line.find(",", start_index)
                inode = int(line[start_index: end_index])
            if(occurrence == 2):
                end_index = line.find(",", start_index)
                lev_indirection = int(line[start_index: end_index])
            start_index = line.find(",", start_index) + 1
            occurrence += 1
            
        end_index = line.find(",", start_index)
        offset = int(line[start_index: end_index])

        #print error message depending on level of indirection
        #we can determine the level of indirection by the 
        if(lev_indirection == 1):
            #direct block
            if(duplicate):
                print("DUPLICATE " + "DIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))
            if(error_type != ""):
                print(error_type + "DIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))

        if(lev_indirection == 2):
            #indirect block
            if(duplicate):
                print("DUPLICATE " + "INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))
            if(error_type != ""):
                print(error_type + "INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))

        if(lev_indirection == 3):
            #double indirect block
            if(duplicate):
                print("DUPLICATE " + "DOUBLE DIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))
            if(error_type != ""):
                print(error_type + "DOUBLE INDIRECT BLOCK " + str(pointer) + " IN INODE " + str(inode) + " AT OFFSET " + str(offset))

    
#block consistency checker
def block_check(file_lines, max_block_num, min_block_num, free_blocks):
    alloc_blocks = {}
    #scan the file once to determine if there are duplicate blocks
    for line in file_lines:
        if("INDIRECT" in line):
            #add the block pointer to the allocated blocks list
            start_index = line.rfind(",") + 1
            pointer = int(line[start_index:])
            if(pointer in alloc_blocks):
                alloc_blocks[pointer] += 1
            else:
                alloc_blocks[pointer] = 1
        if("INODE" in line):
            if(ignore_file(line)):
                continue
            pointers_found = 0
            end_index = len(line)
            while(pointers_found < 15):
                start_index = line.rfind(",", 0, end_index) + 1
                pointer = int(line[start_index: end_index])
                end_index = start_index - 1
                if(pointer in alloc_blocks):
                    alloc_blocks[pointer] += 1
                else:
                    alloc_blocks[pointer] = 1
                pointers_found += 1

    for line in file_lines:
        if("INDIRECT" in line):
            indirect_entry_check(line, max_block_num, min_block_num, alloc_blocks)
        if("INODE" in line):
            inode_entry_check(line, max_block_num, min_block_num, alloc_blocks)

    #make sure every valid block is either on the free list or allocated to one file
    for i in range(max_block_num):
        if(i < min_block_num):
            continue
        elif(i in alloc_blocks and i in free_blocks):
            print("ALLOCATED BLOCK " + str(i) + " ON FREELIST")
        elif(i not in alloc_blocks and i not in free_blocks):
            print("UNREFERENCED BLOCK " + str(i))
def main():
    if(len(sys.argv) != 2):
        sys.stderr.write("Error: must supply one argument")
        sys.exit(1)

    file_name = None;
    try:
        file_name = open(sys.argv[1], "r");
    except:
        sys.stderr.write("Invalid file name")
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
    block_check(file_lines, max_block_num, min_block_num, block_bitmap)

if __name__ == "__main__":
    main()