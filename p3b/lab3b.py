#!/usr/bin/python

import sys
from collections import defaultdict

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

def inode_check(free_list, inodes, inode_count):
    """ Inode allocation audits

    1. Check to ensure that inodes reported as unallocated are not in fact 
    allocated (in the alloc_inodes set).

    2. Check to ensure that all of the inodes not reported as allocated are 
    properly reported as unallocated (not in alloc_inodes => in inode_bitmap).

    Performance: Bad, O(n^2). Could be made better, by converting into lists
    and merging the lists, while counting discrepancies and skipped numbers. 

    Args:
        inode_bitmap (set(int)): inode numbers of IFREE csv lines
        alloc_inodes (set(int)): inode numbers of INODE and INDIRECT lines
        inode_count (int): number of inodes as reported by SUPERBLOCK line
    """

    for free_inode in free_list:
        if (free_inode in inodes):
            print("ALLOCATED INODE " + str(free_inode) + " ON FREELIST")

    inode = 11
    while inode <= inode_count:
        if inode not in inodes and inode not in free_list:
            print("UNALLOCATED INODE " + str(inode) + " NOT ON FREELIST")

        inode = inode + 1

def dir_check(inode_reported_lnk, inode_actual_lnk, alloc_inodes, dir_inodes, parent_lookup, inode_count):
    """ Directory Consistency Audits

    1. Check to ensure that the links reported are accurate for each inode.

    2. Check to make sure that directory inodes are valid/allocated.

    3. Check to make sure that directories map back on themselves or to their 
    parent directory.

    Performance: O(n + d^2) where n is the inode entries and d is the directory 
    entries. Might be able to improve performance by combining checks. Method
    for checking the '..' accuracy is questionable at best. 

    Args:
        inode_reported_lnk (dir(int:int)): maps inode number to reported link count from csv
        inode_actual_lnk (dir(int:int)): maps inode number to counted references
        alloc_inodes (set(int)): inode numbers of INODE and INDIRECT lines 
        dir_inodes (dir(int:set)): stores inode, prnt_inode, name for each DIRENT
        inode_count (int): number of inodes as reported by SUPERBLOCK line.
    """
    # loop to check that the inode links and reported inode links are equal
    for inode in alloc_inodes:
        if (inode <= inode_count):
            if (inode_reported_lnk[inode] != inode_actual_lnk[inode]):
                print("INODE " + str(inode) + " HAS " + str(inode_actual_lnk[inode]) + " LINKS BUT LINKCOUNT IS " + str(inode_reported_lnk[inode]))

    # checking direcotyr inodes and start links
    for entry in dir_inodes:
        prnt = dir_inodes[entry]["prnt_inode"]
        inode = dir_inodes[entry]["inode"]
        name = dir_inodes[entry]["name"]

        if (inode < 1 or inode > inode_count):
            print("DIRECTORY INODE " + str(prnt) + " NAME " + name + " INVALID INODE " + str(inode))
        elif (inode not in alloc_inodes):
            print("DIRECTORY INODE " + str(prnt) + " NAME " + name + " UNALLOCATED INODE " + str(inode))
        elif (name == "'.'"):
            if (inode != prnt):
                print("DIRECTORY INODE " + str(prnt) + " NAME " + name + " LINK TO INODE " + str(inode) + " SHOULD BE " + str(prnt)) 
        elif (name == "'..'"): 
            # Find the dir inodes that have the parent inode as their inode
            # => they link down into the the dir we're in
            if (prnt in parent_lookup and inode != parent_lookup[prnt]):
                print("DIRECTORY INODE " + str(prnt) + " NAME " + name + " LINK TO INODE " + str(inode) + " SHOULD BE " + str(parent_lookup[prnt]))
            elif(prnt not in parent_lookup):
                if(inode != prnt):
                    print("DIRECTORY INODE " + str(prnt) + " NAME " + name + " LINK TO INODE " + str(inode) + " SHOULD BE " + str(prnt))

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

    # map of inode : linkcount
    # map of inode : ref_links
    # map of dir_inode : dir_name
    inode_reported_lnk = defaultdict(int)
    inode_actual_lnk = defaultdict(int)
    dir_inodes = {}
    free_list = set()
    inodes = set()
    parent_lookup = {}

    for line in file_name:
        file_lines.append(line)
        if("BFREE" in line):
            #extract the block number
            block_num = line[6:]
            block_bitmap.add(int(block_num))
        elif("IFREE" in line):
            inode_num = line[6:]
            inode_bitmap.add(int(inode_num))
            free_list.add(int(line.split(',')[1][:-1]))
        elif("INODE" in line):
            #extract the inode number and add to allocated list
            start_index = line.find(",") + 1;
            end_index = line.find(",", start_index)
            alloc_inodes.add(int(line[start_index: end_index]))

            # log the reported link count for the inode
            i_num = int(line.split(',')[1])
            lnk_cnt = int(line.split(',')[6])
            inode_reported_lnk[i_num] = lnk_cnt

            # add inode to list 
            inodes.add(i_num)

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
        elif("DIRENT" in line):
            dir_entry = line.split(',')
            inode_actual_lnk[int(dir_entry[3])] += 1

            # key to each dir is name + parent dir (guarentees unique)
            key = dir_entry[1] + "to" + dir_entry[3]

            details = {
                "name" : dir_entry[6][:-1],
                "prnt_inode" : int(dir_entry[1]),
                "inode" : int(dir_entry[3])
            }

            if (details["name"] != "'.'" and details["name"] != "'..'"):
                parent_lookup[details["inode"]] = details["prnt_inode"]
                
            dir_inodes[key] = details

    block_check(file_lines, max_block_num, min_block_num, block_bitmap)

    # performing inode check
    inode_count = int(superblock.split(",")[2])
    inode_check(free_list, inodes, inode_count)

    # performing directory check
    dir_check(inode_reported_lnk, inode_actual_lnk, alloc_inodes, dir_inodes, parent_lookup, inode_count)



if __name__ == "__main__":
    main()