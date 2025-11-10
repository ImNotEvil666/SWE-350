#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "../includes/peripherals/hex-display.h"

/*
 * main
 * Purpose: Run a simple clock on HEX0..HEX5 using MMIO via HAL.
 * Behavior:
 *   Initializes HEX blocks, maintains hh:mm:ss with 1 s ticks,
 *   handles rollovers, writes digits to HEX displays each loop,
 *   then clears displays and closes resources on exit.
 * Returns:
 *   0 on normal exit; nonzero on initialization failure.
 */

int main(void) {
    if (init_hex0_hex3() != 0 || init_hex4_hex5() != 0) {
        fprintf(stderr, "HEX init failed\n");
        return 1;
    }

    int hours = 12;
    int minutes = 0;
    int seconds = 0;

    while (1) {
        sleep(1);
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
                hours = (hours + 1) % 24;
            }
        }

        int h1 = hours / 10;
        int h0 = hours % 10;
        int m1 = minutes / 10;
        int m0 = minutes % 10;
        int s1 = seconds / 10;
        int s0 = seconds % 10;

        hex_display_write(5, h1);
        hex_display_write(4, h0);
        hex_display_write(3, m1);
        hex_display_write(2, m0);
        hex_display_write(1, s1);
        hex_display_write(0, s0);
    }

    hex_display_clear_all();
    close_hex0_hex3();
    close_hex4_hex5();
    return 0;
}
