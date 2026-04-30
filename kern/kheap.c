#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

// Keeping a “moving pointer” for next-fit strategy (not the best approach, but works fine in practice)
uint32 next_fit_cursor = KERNEL_HEAP_START;

// Calculation: (0xFFFFF000 - 0xF6000000) / 4096
#define NUM_KHEAP_PAGES ((KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE)

// simple tracker: stores how many pages each allocation occupies (index = base page)
uint32 kheap_size_tracker[NUM_KHEAP_PAGES];

void* kmalloc(unsigned int size)
{
if (size == 0) return NULL;

size = ROUNDUP(size, PAGE_SIZE);
uint32 num_pages = size / PAGE_SIZE;

if (isKHeapPlacementStrategyNEXTFIT())
{
    uint32 start_scan = next_fit_cursor;
    uint32 cur = start_scan;

    uint32 found_count = 0;
    bool wrapped_once = 0;

    uint32 candidate_end = 0; // just keeping this for clarity while scanning

    while (found_count < num_pages)
    {
        if (cur >= KERNEL_HEAP_MAX)
        {
            if (wrapped_once)
                return NULL; // full circle, nothing available

            wrapped_once = 1;
            cur = KERNEL_HEAP_START;
            found_count = 0;
        }

        // if we wrapped and somehow passed start again, give up
        if (wrapped_once && (cur + PAGE_SIZE > start_scan))
            return NULL;

        struct Frame_Info *fi = NULL;
        uint32 *pt = NULL;

        fi = get_frame_info(ptr_page_directory, (void*)cur, &pt);

        if (fi == NULL)
        {
            found_count++;
            if (found_count == num_pages)
                candidate_end = cur + PAGE_SIZE;
        }
        else
        {
            // hit an occupied page -> restart counting
            found_count = 0;
        }

        cur += PAGE_SIZE;
    }

    uint32 alloc_start = cur - size;

    // allocate pages
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

    next_fit_cursor = cur;

    uint32 idx = (alloc_start - KERNEL_HEAP_START) / PAGE_SIZE;
    kheap_size_tracker[idx] = num_pages;

    return (void*)alloc_start;
}
else if (isKHeapPlacementStrategyBESTFIT())
{
    uint32 best_start = 0;
    uint32 best_size = 0xFFFFFFFF;

    uint32 cur = KERNEL_HEAP_START;
    uint32 count = 0;
    uint32 block_start = 0;

    while (cur < KERNEL_HEAP_MAX)
    {
        struct Frame_Info *fi = NULL;
        uint32 *pt = NULL;

        fi = get_frame_info(ptr_page_directory, (void*)cur, &pt);

        if (fi == NULL)
        {
            if (count == 0)
                block_start = cur;

            count++;
        }
        else
        {
            uint32 hole_size = count * PAGE_SIZE;

            if (hole_size >= size && hole_size < best_size)
            {
                best_size = hole_size;
                best_start = block_start;
            }

            count = 0;
        }

        cur += PAGE_SIZE;
    }

    // last hole check (I always forget this edge case if I don't comment it)
    if (count > 0)
    {
        uint32 hole_size = count * PAGE_SIZE;
        if (hole_size >= size && hole_size < best_size)
        {
            best_size = hole_size;
            best_start = block_start;
        }
    }

    if (best_size == 0xFFFFFFFF)
        return NULL;

    for (uint32 va = best_start; va < best_start + size; va += PAGE_SIZE)
    {
        struct Frame_Info *fi = NULL;
        allocate_frame(&fi);

        map_frame(ptr_page_directory,
                  fi,
                  (void*)va,
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
panic("kfree: invalid address (out of heap range)");
}


uint32 index = ((uint32)virtual_address - KERNEL_HEAP_START) / PAGE_SIZE;
uint32 pages = kheap_size_tracker[index];

if (pages == 0)
    return; // nothing allocated here (silent ignore)

uint32 va = (uint32)virtual_address;

// unmap pages one by one (not the fastest, but straightforward)
for (uint32 i = 0; i < pages; i++)
{
    unmap_frame(ptr_page_directory, (void*)va);
    va += PAGE_SIZE;
}

// mark as free again
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

