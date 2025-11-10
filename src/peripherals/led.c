#include "peripherals/led.h"
#include "hal/hal-api.h"
#include <stdio.h>

#include "../../lib/address_map_arm.h"

// Global HAL mapping - shared across LED operations
static hal_map_t hal_map;
static int hal_initialized = 0;

/*
 * led_init
 * Purpose: Initialize an LED handle by mapping the LED register address.
 * Params:
 *   led - non-NULL pointer to led_handle_t to initialize.
 * Returns:
 *   0 on success; -1 on error.
 * Side effects:
 *   Opens HAL if needed; sets led->reg_addr; marks initialized.
 * Preconditions:
 *   led != NULL.
 */

int led_init(led_handle_t *led) {
    if (!led) return -1;
    
    // Initialize HAL if not already done
    if (!hal_initialized) {
        if (hal_open(&hal_map) != 0) {
            fprintf(stderr, "Failed to initialize HAL for LED\n");
            return -1;
        }
        hal_initialized = 1;
    }
    
    // Get virtual address for LED register (offset from LW bridge base)
    led->reg_addr = hal_get_virtual_addr(&hal_map, LEDR_BASE);
    if (!led->reg_addr) {
        fprintf(stderr, "Failed to get LED register address\n");
        return -1;
    }
    
    led->initialized = 1;
    
    // Initialize LEDs to off state
    led_set(led, LED_ALL_OFF);
    
    printf("LED peripheral initialized successfully at virtual address %p\n", led->reg_addr);
    return 0;
}

/*
 * led_cleanup
 * Purpose: Mark LED handle as uninitialized; release HAL if owned here.
 * Params:
 *   led - pointer to led_handle_t.
 * Returns:
 *   0 on success; -1 on error.
 * Notes:
 *   If HAL lifetime is global, this may only clear the handle state.
 */

int led_cleanup(led_handle_t *led) {
    if (!led || !led->initialized) return -1;
    
    // Turn off all LEDs before cleanup
    led_set(led, LED_ALL_OFF);
    
    led->reg_addr = NULL;
    led->initialized = 0;
    
    // Close HAL if this was the last LED user
    if (hal_initialized) {
        if (hal_close(&hal_map) != 0) {
            fprintf(stderr, "Failed to cleanup HAL\n");
            return -1;
        }
        hal_initialized = 0;
    }
    
    printf("LED peripheral cleaned up successfully\n");
    return 0;
}

/*
 * led_set
 * Purpose: Write a 10-bit LED pattern to the LED register.
 * Params:
 *   led     - initialized LED handle.
 *   pattern - lower 10 bits drive LED9..LED0 (1 = on).
 * Returns:
 *   0 on success; -1 on error.
 * Preconditions:
 *   led != NULL and led->initialized == 1.
 */

int led_set(led_handle_t *led, uint32_t pattern) {
    if (!led || !led->initialized || !led->reg_addr) return -1;
    
    // Mask to ensure only valid LED bits are set (10 LEDs)
    pattern &= LED_ALL_ON;
    
    // Write to LED register
    *(volatile uint32_t *)led->reg_addr = pattern;
    
    return 0;
}

/*
 * led_get
 * Purpose: Read back the current LED register value.
 * Params:
 *   led      - initialized LED handle.
 *   pattern  - out parameter; receives lower 10 LED bits.
 * Returns:
 *   0 on success; -1 on error.
 * Preconditions:
 *   pattern != NULL; handle initialized.
 */

int led_get(led_handle_t *led, uint32_t *pattern) {
    if (!led || !led->initialized || !led->reg_addr || !pattern) return -1;
    
    // Read from LED register
    *pattern = *(volatile uint32_t *)led->reg_addr;
    *pattern &= LED_ALL_ON;
    
    return 0;
}

/*
 * led_turn_on
 * Purpose: Turn on a single LED while preserving other LED states.
 * Params:
 *   led        - initialized LED handle.
 *   led_number - 0..9, index of LED to turn on.
 * Returns:
 *   0 on success; -1 on invalid handle or index, or read/write failure.
 * Behavior:
 *   Reads current pattern, sets bit led_number, writes back.
 */

int led_turn_on(led_handle_t *led, int led_number) {
    if (!led || !led->initialized || led_number < 0 || led_number >= LED_COUNT) {
        return -1;
    }
    
    // Get current LED state
    uint32_t current_pattern;
    if (led_get(led, &current_pattern) != 0) return -1;
    
    // Turn on the specified LED (set bit)
    current_pattern |= (1 << led_number);
    
    // Write back the updated pattern
    return led_set(led, current_pattern);
}