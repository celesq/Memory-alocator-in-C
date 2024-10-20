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
