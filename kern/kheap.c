#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

uint32 next_fit_cursor = KERNEL_HEAP_START;

#define NUM_KHEAP_PAGES ((KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE)

uint32 kheap_size_tracker[NUM_KHEAP_PAGES];

void* kmalloc(unsigned int size)
{
if (size == 0) return NULL;

size = ROUNDUP(size, PAGE_SIZE);
uint32 num_pages = size / PAGE_SIZE;

if (isKHeapPlacementStrategyNEXTFIT())
{
    uint32 search_start = next_fit_cursor;
    uint32 current_virtual_address = search_start;

    uint32 free_pages_found = 0;
    bool has_wrapped = 0;


    while (free_pages_found < num_pages)
    {
        if (current_virtual_address >= KERNEL_HEAP_MAX)
        {
            if (has_wrapped)
                return NULL;

            has_wrapped = 1;
            current_virtual_address = KERNEL_HEAP_START;
            free_pages_found = 0;
        }
// if we checked the entire heap and we found no suitable spot and we are about to start searching again , we stop the searcg to prevent an infinite loop
        if (has_wrapped && (current_virtual_address + PAGE_SIZE > search_start))
            return NULL;

        struct Frame_Info *frame_info = NULL;
        uint32 *page_table = NULL;

        frame_info = get_frame_info(ptr_page_directory, (void*)current_virtual_address, &page_table);

        if (frame_info == NULL)
        {
            free_pages_found++;
        }
        else
        {
            free_pages_found = 0;
        }

        current_virtual_address += PAGE_SIZE;
    }

    uint32 alloc_start = current_virtual_address - size;

    uint32 va = alloc_start;
    for (uint32 i = 0; i < num_pages; i++)
    {
        struct Frame_Info *new_frame = NULL;
        allocate_frame(&new_frame);

        map_frame(ptr_page_directory,
                  new_frame,
                  (void*)va,
                  PERM_PRESENT | PERM_WRITEABLE);

        va += PAGE_SIZE;
    }

    next_fit_cursor = current_virtual_address;

    uint32 idx = (alloc_start - KERNEL_HEAP_START) / PAGE_SIZE;
    kheap_size_tracker[idx] = num_pages;

    return (void*)alloc_start;
}
else if (isKHeapPlacementStrategyBESTFIT())
{
    uint32 best_start = 0;
    uint32 best_size = 0xFFFFFFFF;

    uint32 current_virtual_adress = KERNEL_HEAP_START;
    uint32 free_pages_count = 0;
    uint32 block_start = 0;

    while (current_virtual_adress < KERNEL_HEAP_MAX)
    {
        struct Frame_Info *frame_info = NULL;
        uint32 *pt = NULL;

        frame_info = get_frame_info(ptr_page_directory, (void*)current_virtual_adress, &pt);

        if (frame_info == NULL)
        {
            if (free_pages_count == 0)
                block_start = current_virtual_adress;

            free_pages_count++;
        }
        else
        {
            uint32 hole_size = free_pages_count * PAGE_SIZE;

            if (hole_size >= size && hole_size < best_size)
            {
                best_size = hole_size;
                best_start = block_start;
            }

            free_pages_count = 0;
        }

        current_virtual_adress += PAGE_SIZE;
    }


    if (free_pages_count > 0)
    {
        uint32 hole_size = free_pages_count * PAGE_SIZE;
        if (hole_size >= size && hole_size < best_size)
        {
            best_size = hole_size;
            best_start = block_start;
        }
    }

    if (best_size == 0xFFFFFFFF)
        return NULL;

    for (uint32 virtual_address = best_start; virtual_address < best_start + size; virtual_address += PAGE_SIZE)
    {
        struct Frame_Info *frame_info = NULL;
        allocate_frame(&frame_info);

        map_frame(ptr_page_directory,
                  frame_info,
                  (void*)virtual_address,
                  PERM_PRESENT | PERM_WRITEABLE);
    }

    uint32 idx = (best_start - KERNEL_HEAP_START) / PAGE_SIZE;
    kheap_size_tracker[idx] = num_pages;

    return (void*)best_start;
}

return NULL;


}

void kfree(void* virtual_address)
{
if ((uint32)virtual_address < KERNEL_HEAP_START ||
(uint32)virtual_address >= KERNEL_HEAP_MAX)
{
panic("invalid address (out of heap range)");
}


uint32 index = ((uint32)virtual_address - KERNEL_HEAP_START) / PAGE_SIZE;
uint32 pages = kheap_size_tracker[index];

if (pages == 0)
    return;
uint32 va = (uint32)virtual_address;


for (uint32 i = 0; i < pages; i++)
{
    unmap_frame(ptr_page_directory, (void*)va);
    va += PAGE_SIZE;
}


kheap_size_tracker[index] = 0;


}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2026 - [3] Kernel Heap] kheap_virtual_address()
	uint32 offset = physical_address % PAGE_SIZE;


	for(int i=0 ; i< NUM_KHEAP_PAGES ; i++){

		if(kheap_size_tracker[i] !=0){
			uint32 start_va = KERNEL_HEAP_START + i * PAGE_SIZE;
			uint32 num_pages = kheap_size_tracker [i];
				for (int j =0; j < num_pages ; j++){
		uint32 va = start_va + j *PAGE_SIZE;
		 uint32 *ptr_page_table = NULL;


		 struct Frame_Info *frame = get_frame_info(ptr_page_directory, (void*)va ,&ptr_page_table);
		 if (frame != NULL){
		 if((to_physical_address(frame)) == (physical_address-offset)) //if physical base= physical base keda tmam
		 {


	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer

	return va + offset;
}
	}
}
		}
	}
return 0;}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2026 - [4] Kernel Heap] kheap_physical_address()
	uint32 *ptr_page_table = NULL;

	struct Frame_Info *frame = get_frame_info (ptr_page_directory, (void*)virtual_address, &ptr_page_table);
	if (frame ==NULL)
		return 0;
	uint32 offset = virtual_address % PAGE_SIZE;
	return to_physical_address(frame) + offset;
	return 0;

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
}

