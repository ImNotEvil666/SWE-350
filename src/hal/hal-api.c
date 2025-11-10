#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "../../lib/address_map_arm.h"
#include "../../includes/hal/hal-api.h"

/*
 * hal_open
 * Purpose: Open /dev/mem and mmap the LW bridge window; populate hal_map_t.
 * Params:
 *   map  - non-NULL pointer to hal_map_t to initialize.
 * Returns:
 *   0 on success; -1 on error (stderr contains reason).
 * Side effects:
 *   map->fd, map->virtual_base, map->span are set.
 * Preconditions:
 *   map != NULL.
 * Errors:
 *   Fails if /dev/mem open or mmap fails.
 */


int hal_open(hal_map_t *map) {
    if (!map) return -1;

    map->fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (map->fd == -1) {
        perror("ERROR: could not open /dev/mem");
        return -1;
    }

    map->virtual_base = mmap(NULL, LW_BRIDGE_SPAN, PROT_READ | PROT_WRITE, 
                            MAP_SHARED, map->fd, LW_BRIDGE_BASE);
    if (map->virtual_base == MAP_FAILED) {
        perror("ERROR: mmap() failed");
        close(map->fd);
        return -1;
    }

    map->span = LW_BRIDGE_SPAN;
    return 0;
}

/*
 * hal_close
 * Purpose: Unmap and close resources previously opened by hal_open.
 * Params:
 *   map  - non-NULL pointer to hal_map_t to clear and close.
 * Returns:
 *   0 on success; -1 on error.
 * Side effects:
 *   Unmaps virtual_base; closes fd; zeros fields.
 * Preconditions:
 *   map != NULL and may be partially initialized.
 */


int hal_close(hal_map_t *map) {
    if (!map) return -1;

    if (munmap(map->virtual_base, map->span) != 0) {
        perror("ERROR: munmap() failed");
        return -1;
    }

    close(map->fd);
    map->fd = -1;
    map->virtual_base = NULL;
    map->span = 0;
    return 0;
}

/*
 * hal_get_virtual_addr
 * Purpose: Convert an LW-bridge byte offset to a process-virtual pointer.
 * Params:
 *   map    - non-NULL hal_map_t with valid virtual_base.
 *   offset - byte offset within the mapped LW bridge window.
 * Returns:
 *   Pointer to (virtual_base + offset), or NULL if map/virtual_base invalid.
 * Notes:
 *   Caller is responsible for using the correct peripheral offsets.
 */


void* hal_get_virtual_addr(hal_map_t *map, unsigned int offset) {
    if (!map || !map->virtual_base) return NULL;
    
    return (void*)((char*)map->virtual_base + offset);
}