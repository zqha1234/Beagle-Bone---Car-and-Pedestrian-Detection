/*
    NeoPixel RGBW demo program for 8 LED unit such as:
        https://www.adafruit.com/product/2868
    NOTE: This is RGBW, not RGB!

    Install process
    - Put the NeoPixel into a breadboard
    - Connect the NeoPixel with 3 wires:
        - Connect NeoPixel "GND" and "DIN" (data in) to the 3-pin "LEDS" header on Zen
            Zen Cape's LEDS header:
                Pin 1: DIN (Data): left most pin; beside USB-micro connection, connects to P8.11
                Pin 2: GND (Ground): middle pin
                Pin 3: Unused (it's "5V external power", which is not powered normally on the BBG)
        - Connect NeoPixel "5VDC" to P9.7 or P9.8
            Suggest using the header-extender to make it easier to make a good connection.
        - OK to wire directly to BBG: no level shifter required.
    - Software Setup
        - On Host
            make      # on parent folder to copy to NFS
        - On Target:
            config-pin P8.11 pruout
            make
            make install_PRU0
    - All lights should light up on the LED strip

    Based on code from the PRU Cookbook by Mark A. Yoder:
        https://beagleboard.org/static/prucookbook/#_setting_neopixels_to_different_colors
*/

#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "sharedMem.h"

#define STR_LEN         8       // # LEDs in our string
#define oneCyclesOn     700/5   // Stay on 700ns
#define oneCyclesOff    800/5
#define zeroCyclesOn    350/5
#define zeroCyclesOff   600/5
#define resetCycles     60000/5 // Must be at least 50u, use 60u

// P8_11 for output (on R30), PRU0
#define DATA_PIN 15       // Bit number to output on

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Shared Memory Configuration
// -----------------------------------------------------------
#define DELAY_1_MS 200000
#define THIS_PRU_DRAM 0x00000   // Address of DRAM
#define OFFSET 0x200            // Skip 0x100 for Stack, 0x100 for Heap (from makefile)
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)
// This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
volatile sharedMemStruct_t *pSharedMemStruct = (volatile void *)THIS_PRU_DRAM_USABLE;
const uint32_t NO_COLOR = 0x00000000;
uint32_t COLORS[3] = {0x000f0000, 0x0f000000, 0x00000f00};
uint32_t COLORS_BRIGHT[3] = {0x00ff0000, 0xff000000, 0x0000ff00};
uint32_t colors_8_LEDs[STR_LEN] = {0x000f0000, 0x000f0000, 0x000f0000, 0x000f0000,
                                   0x000f0000, 0x000f0000, 0x000f0000, 0x000f0000};
const int32_t BLINK_WAIT_PERIOD_MS = 500;


void fillColorsAll8LEDs(uint32_t desiredColor);
void driveLED(void);
void waitForMs(int delayInMs);
void fillColors3LEDs(uint32_t desiredColor, uint32_t desiredColorBright, int displayOption);


void main(void)
{
    // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
    __delay_cycles(resetCycles);

    pSharedMemStruct->isProgramFinished = false;
    pSharedMemStruct->LEDColorOption = GREEN;
    pSharedMemStruct->LEDDisplayOption = 0;

    while(!pSharedMemStruct->isProgramFinished) {
        int ledDisplayOption = pSharedMemStruct->LEDDisplayOption;
        int ledColorOption = pSharedMemStruct->LEDColorOption;
        fillColorsAll8LEDs(COLORS[ledColorOption]);
        waitForMs(20);
        if(ledDisplayOption != 0) {
            int waitTime = BLINK_WAIT_PERIOD_MS / ledDisplayOption;
            waitForMs(waitTime);
            fillColorsAll8LEDs(NO_COLOR);
            waitForMs(waitTime);
        }
    }

    fillColorsAll8LEDs(NO_COLOR);
    waitForMs(50);

    // Send Reset
    __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
    __delay_cycles(resetCycles);
}

void fillColorsAll8LEDs(uint32_t desiredColor) {
    for(int i=0; i < STR_LEN; i++) {
        colors_8_LEDs[i] = desiredColor;
    }
    driveLED();
}

void driveLED(void) {
    for(int j = 0; j < STR_LEN; j++) {
        for(int i = 31; i >= 0; i--) {
            if(colors_8_LEDs[j] & ((uint32_t)0x1 << i)) {
                __R30 |= 0x1<<DATA_PIN;      // Set the GPIO pin to 1
                __delay_cycles(oneCyclesOn-1);
                __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
                __delay_cycles(oneCyclesOff-2);
            } else {
                __R30 |= 0x1<<DATA_PIN;      // Set the GPIO pin to 1
                __delay_cycles(zeroCyclesOn-1);
                __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
                __delay_cycles(zeroCyclesOff-2);
            }
        }
    }
}

void waitForMs(int delayInMs)
{
    for (int i = 0; i < delayInMs; i++) {
        __delay_cycles(DELAY_1_MS);
    }
}

void fillColors3LEDs(uint32_t desiredColor, uint32_t desiredColorBright, int displayOption) {
    for(int i=0; i < STR_LEN; i++) {
        if (i == displayOption) {
            colors_8_LEDs[i] = desiredColorBright;
        } else if (i == displayOption - 1 || i == displayOption + 1) {
            colors_8_LEDs[i] = desiredColor;
        } else {
            colors_8_LEDs[i] = NO_COLOR;
        }
    }
    driveLED();
}
