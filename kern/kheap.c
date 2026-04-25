#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

uint32 nextFitPtr = KERNEL_HEAP_START;
// Calculation: (0xFFFFF000 - 0xF6000000) / 4096
#define NUM_KHEAP_PAGES ((KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE)

// Global array to remember the size of each kmalloc allocation
uint32 kheap_size_tracker[NUM_KHEAP_PAGES];
void* kmalloc(unsigned int size)
{
	if(size == 0)return NULL;
		//roundup to the nearest PAGE_SIZE multiple
		size =  ROUNDUP(size,PAGE_SIZE);
		uint32 numPages = size / PAGE_SIZE;
		if(isKHeapPlacementStrategyNEXTFIT()){
		// Next Fit: search from nextFitPtr for contiguous free pages
		uint32 start = nextFitPtr;
		uint32 current = start;
		uint32 count = 0;
		 bool wrapped = 0;
		while (count < numPages)
		{
					if(current >= KERNEL_HEAP_MAX) // wrap around
			        {
				 	 	if(wrapped) return NULL; // went full circle, heap is full
				        wrapped = 1;
			            current = KERNEL_HEAP_START;
			            count = 0;
			        }
			 	    // stop if we've looped back past start
			        if(wrapped &&  current + PAGE_SIZE > start) return NULL;
		            // check if this page is free
			        struct Frame_Info *ptr_frame_info = NULL;
			        uint32 *ptr_page_table = NULL;
			        ptr_frame_info = get_frame_info(ptr_page_directory, (void*)current, &ptr_page_table);

			        if(ptr_frame_info == NULL) // page is free
			            count++;
			        else // not free, reset
			        {
			            count = 0;
			            current += PAGE_SIZE;
			            continue;
			        }
			        current += PAGE_SIZE;
		}

		// 3. Found spot— allocate and map
		uint32 allocStart = current - size;
		for(uint32 va = allocStart; va < allocStart + size; va += PAGE_SIZE)
		{
		 struct Frame_Info *ptr_frame_info = NULL;
		 allocate_frame(&ptr_frame_info);
		 map_frame(ptr_page_directory, ptr_frame_info, (void*)va, PERM_PRESENT | PERM_WRITEABLE);
		 }
		nextFitPtr = current; // update next fit pointer
		kheap_size_tracker[(allocStart - KERNEL_HEAP_START) / PAGE_SIZE] = numPages;
		return (void*)allocStart;
		}
		else if(isKHeapPlacementStrategyBESTFIT())
		{
		        	uint32 bestStart = 0;
			        uint32 bestSize = 0xFFFFFFFF;
			        uint32 current = KERNEL_HEAP_START;
			        uint32 count = 0;
			        uint32 blockStart = 0;

			        while(current < KERNEL_HEAP_MAX)
			        {
			            struct Frame_Info *ptr_frame_info = NULL;
			            uint32 *ptr_page_table = NULL;
			            ptr_frame_info = get_frame_info(ptr_page_directory, (void*)current, &ptr_page_table);

			            if(ptr_frame_info == NULL)
			            {
			                if(count == 0) blockStart = current;
			                count++;
			            }
			            else
			            {
			                uint32 holeSize = count * PAGE_SIZE;
			                if(holeSize >= size && holeSize < bestSize)
			                {
			                    bestSize = holeSize;
			                    bestStart = blockStart;
			                }
			                count = 0;
			            }
			            current += PAGE_SIZE;
			        }
			        // check last hole at end of heap
			        if(count > 0)
			        {
			            uint32 holeSize = count * PAGE_SIZE;
			            if(holeSize >= size && holeSize < bestSize)
			            {
			                bestSize = holeSize;
			                bestStart = blockStart;
			            }
			        }

			        if(bestSize == 0xFFFFFFFF) return NULL; // nothing fits

			        for(uint32 va = bestStart; va < bestStart + size; va += PAGE_SIZE)
			        {
			            struct Frame_Info *fi = NULL;
			            allocate_frame(&fi);
			            map_frame(ptr_page_directory, fi, (void*)va, PERM_PRESENT | PERM_WRITEABLE);
			        }
			        kheap_size_tracker[(bestStart - KERNEL_HEAP_START) / PAGE_SIZE] = numPages;
			        return (void*)bestStart;

		}
		return NULL;
}
void kfree(void* virtual_address)
{
	 if((uint32)virtual_address < KERNEL_HEAP_START ||
	       (uint32)virtual_address >= KERNEL_HEAP_MAX)
	        panic("kfree: invalid address");

	 uint32 index = ((uint32)virtual_address - KERNEL_HEAP_START) / PAGE_SIZE;
	     uint32 num_pages = kheap_size_tracker[index];

	     if (num_pages == 0) return; // Nothing was allocated here!

	     // 3. Free EXACTLY num_pages
	     uint32 va = (uint32)virtual_address;
	     for (int i = 0; i < num_pages; i++) {
	         unmap_frame(ptr_page_directory, (void*)va);
	         va += PAGE_SIZE;
	     }

	     // 4. Mark this spot as available in your tracker
	     kheap_size_tracker[index] = 0;
	 }

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2026 - [3] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer

	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2026 - [4] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	return 0;
}

