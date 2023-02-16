# banhammer.c
    This is the file containing the main function and is the file to be compiled into a binary
    its options include:
        -m enabling move to front, causing found nodes in the linked lists of its hashtable to be moved to the front to make accessing them quicker
        -h printing out the help message, relaying much of this information
        -s enabling statistics to be printed out instead of the message about speech
        -t <ht_size> sets the size of the hashtable (default = 10000)
        -f <bf_size> sets the size of the filter in the bloomfilter (default = 2^19)
    This program will first read in a list of words from a file called badspeak.txt, adding each to its hashtable without a translation and setting the appropriate bits in the bloomfilter
    It will do the same with a file called newspeak.txt, though this time adding words to the hashtable with their associated translations
    It will then take an input from standard in, and parse all words from standard in. If the words are within badspeak.txt, it will printout a message listing the words, and a message about how it is badspeak. If the words are within newspeak.txt, it will printout a message listing the words and their translations, and a message about how it is oldspeak and needs to change. If there is words from both, then the words in badspeak will be shown, the words in newspeak shown with their translations, and a message about mixspeak. If no words are contained within badspeak.txt or newspeak.txt, then nothing is printed.
    With the statistics option enabled, we will instead see information about the number of keys in the hashtable, information about how much the bloom filter is examined, and some other values
    This file is dependent upon all .h files for proper compilation

# Makefile
    This file takes all the c code and compiles it into the actual banhammer executeable.
    There are 4 calls we can make
    make or make all: compiles the .c code into the banhammer binary
    make clean: removes all the .o files
    make spotless: removes all the .o files and the banhammer binary
    make format: formats the .c files so they look nicer

# parser.c
    This file contains the code that creates the parser, deletes the parser, and gets the next word out of the input into banhammer

# parser.h
    This file contains the header for parser.c and is required for the compilation of the code, as it has important include statements

# bf.c
    This file contains the implementation of the bloomfilter ADT
    It will create, destroy, check values, and keep track of all stats regarding the bloomfilter
    It also requires bv.c, bv.h, city.c, and city.h in order to compile

# bf.h
    This file contains the header for bf.c and is required for the compilation of the code, as it has important include statements

# bv.c
    This file contains the implementation of the bitvector ADT
    It will initialize a set of uint64_t values such that they can contain all the bits for the entire defined length of the bloomfilter
    It will create the bitvector, destroy it, set, clear, and get bits within the bitvector

# bv.h
    This file contains the header for bv.c and is required for the compilation of the code, as it has important include statements

# ht.c
    This file contains the implementation of the hashtable ADT
    It will create a space for the hashtable's size number of linked lists
    It will destroy the hashtable, help call destroy to all linked lists
    It will handle the insertion and searching of the hashtable ADT
    It requires the ll.c, ll.h, city.c, and city.h files to compile properly

# ht.h
    This file contains the header for ht.c and is required for the compilation of the code, as it has important include statements

# ll.c
    This file contains the implementation of the linked list ADT
    It will initialize a linked list with a head and tail sentinel node, handle insertion and lookups
    It will handle destruction of the linked list, including calls to delete nodes
    It requires proper compilation of node.c and node.h

# ll.h
    This file contains the header for ll.c and is required for the compilation of the code, as it has important include statements

# node.c
    This file contains the implementation of a single node for a doubly linked list
    It will handle creation and deletion of a node

# node.h
    This file contains the header for node.c and is required for the compilation of the code, as it has important include statements

# city.c
    This file is the implementation of the hash function used for the hashtable and bloomfilter

# city.h
    This file is important for the correct compilation of the code

# messages.h
    This file contains the messages regarding badspeak, oldspeak, and mixspeak
