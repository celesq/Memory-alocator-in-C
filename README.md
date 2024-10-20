# Memory-alocator-in-C
Implemented a memory allocator that manages dynamic memory using custom structures. Features include MALLOC, FREE, READ, WRITE, and DUMP_MEMORY, with error handling for segmentation faults and memory fragmentation.
The memory allocator uses a fictive memory allocation model where addresses were stored in a data structure.

# User Guide for the Memory Allocator

  ## MALLOC Command
        Command: MALLOC <nr_bytes>
        Allocates <nr_bytes> of memory. If no block of the appropriate size is found, it splits a larger one or displays an "Out of memory" error.
        Example: MALLOC 128
        Output: Allocates memory if available, or returns an error.

  ## FREE Command
        Command: FREE <address>
        Frees the memory block starting at <address>. Ensures the address is valid and performs coalescing of adjacent free blocks.
        Example: FREE 0x1000
        Output: Marks the block as free, or displays "Invalid free" if the address is invalid.

  ## READ Command
        Command: READ <address> <nr_bytes>
        Displays <nr_bytes> starting from <address> in ASCII format. If the memory is unallocated, it throws a "Segmentation fault" error.
        Example: READ 0x1000 64
        Output: Displays memory contents or raises a segmentation fault.

  ## WRITE Command
        Command: WRITE <address> <data> <nr_bytes>
        Writes <nr_bytes> from <data> to <address>. If the memory block isn’t allocated, a segmentation fault is raised.
        Example: WRITE 0x1000 "Hello" 5
        Output: Writes data or raises a segmentation fault if the memory is unallocated.

  ## DUMP_MEMORY Command
        Command: DUMP_MEMORY
        Displays all allocated and free memory zones, including the contents of allocated blocks.
        Example: DUMP_MEMORY
        Output: Prints the memory layout and block contents.

## Memory Allocator Implementation

### Data Structure
For the implementation, I used a doubly linked list structure that contains the necessary fields to complete the task, such as `data_size` (for generalization), `size`, `block_size`, `nr`, and a `*head` of type `dll_node` that itself contains a `void *data`, `*start`, `*end` (for generalization), `next` and `prev` pointers, as well as helper data like `size` and `write`. Additionally, I utilized another structure, `dump_mem`, which helps me retain all the necessary data for the `DUMP_MEMORY` function, minimizing the number of parameters in functions.

I mention that `dll_node_t` contains a `struct data` that holds all the information about the node, including `*prev` and `*next` (to generalize as much as possible).

### Function Organization
The function headers are organized before the `main` function for easier readability and better modularization. In `main`, the command is read as a character string of a maximum of 650 characters and parsed to obtain the necessary data for the functions. Besides that, the doubly linked list is initialized, along with the defensive programming corresponding to it.

### INIT_HEAP Function
The first function, `INIT_HEAP`, receives the necessary parameters obtained in `main`, such as the address, number of lists, required bytes, and type, then initializes and allocates the vector of doubly linked lists. For each list, it calculates how many blocks are needed to have exactly `nr_bytes` each, and then for each block in `nr_blocks`, it initializes and allocates a new node, adding it to the list using the `addtolist` function, which takes as parameters the list where the node should be added, the respective node, and the type of addition. If the type is 0, the node will be added to the end of the list; otherwise, it will be added between nodes with addresses smaller and larger than its own to maintain the sorted order. This function will also be used in other functions, such as `MALLOC` and `FREE`.

### MALLOC Function
The `MALLOC` function receives the necessary parameters, increments the number of mallocs made, and searches for the first list in the vector of lists with a `block_size` smaller than or equal to `bytes` (the parameter) to extract a node. Once found, the node is extracted using the `removenode` function, which removes a node from a given list and returns it, and then the node is added to the doubly linked list. If `nr_bytes` is less than the `block_size` of the list from which we extracted, it means fragmentation will occur. A `list_node` is initialized and allocated to represent the fragmented piece of the node that remains in the vector of lists, while the node added to the doubly linked list changes its `end` address. Then, if a list with `block_size` equal to the `size` of the node to be added in the vector of lists is found, it is added. Otherwise, the vector of lists will be resized to allow the addition of a new list with the appropriate size, which is initialized according to the dimensions. To maintain the vector of lists sorted by the block sizes, each list is moved so that the new list ends up between two lists with smaller and larger block sizes, ensuring that the vector remains sorted. After this movement is completed, the node is added to the moved list. If allocation fails, meaning the "Out of memory" message is displayed, the number of allocations is decremented.

### FREE Function
The `FREE` function follows the same principle as the `MALLOC` function, iterating through the doubly linked list to find where the node with the given address should be removed. It then places it in the vector of lists if it finds a list with a `block_size` equal to the `size` of the removed node. Otherwise, the vector of lists will be resized again, and the newly added list will be moved using the same principle, and the node will be added to the corresponding list. Similarly, if "Invalid free" is displayed at the end, the number of frees is decremented to ensure the correct functioning of the `DUMP_MEMORY` function later on.

### READ Function
The `READ` function receives the relevant parameters and iterates through each list in the vector of lists to check if the given address exists. If found, it means it was not allocated, and the variable `unaloc` is set to 1. It then searches the doubly linked list for the address, and when it finds it, it uses the node's `end` to move to the next node, checking if there is a contiguous allocated memory area larger than or equal to the number of bytes to be read. Otherwise, the `unaloc` variable remains 1. Then, if `unaloc` is equal to 1, it prints the message "Segmentation fault (core dumped)" and performs a memory dump according to the function's rules before exiting the program. If the program continues, it means a contiguous memory area allocated from the given address was found; it uses a temporary character string to copy from each node exactly `current_node->write`, a variable that helps us see how many bytes have been written in the current node, and displays only what is necessary for efficiency. To avoid writing more than necessary, in case memory is split across multiple nodes, it uses the `start` and `bytes` variables to check exactly where to read from and if there is more to read.

### WRITE Function
In the `WRITE` function, the same principle is applied to check if there is a contiguous allocated memory area to write the desired message; if not, a memory dump is performed according to the function's rules, and the program exits with the message "Segmentation fault (core dumped)." For writing to nodes, it first checks if the current node has been allocated; if so, it will overwrite the current node with exactly the required number of bytes, preserving anything beyond that in the node. For this, a temporary character string and the `len` and `oldlen` variables are used to ensure an exact copy of the desired number of characters. If the node was not allocated, it allocates the respective node and writes as much as possible into the node, then moves on to the next one using the same logic as in the `READ` function, keeping track of the `bytes` and `start` variables to know how much is left to write.

### DUMP_MEMORY Function
The `DUMP_MEMORY` function receives, in addition to the vector of lists and the selected list, the structure where the necessary data for the correct functioning of the function has been retained so far, and then prints it according to the rules from the `DUMP_MEMORY` function. When printing the nodes, it iterates through the current list using a node that receives `list->head` and displays the requested data.

### DESTROY_HEAP Function
The `DESTROY_HEAP` function takes the vector of lists and the selected list as parameters and iterates through them to ensure that all dynamically allocated memory for each node in them is freed, specifically `current_node->start`, `current_node->end`, and `current_node->data`, and then it frees the memory allocated for the lists.

### Debugging Function
The print function was a helper function that helped me debug the program; it printed each list in the vector of lists and then the doubly linked list.

