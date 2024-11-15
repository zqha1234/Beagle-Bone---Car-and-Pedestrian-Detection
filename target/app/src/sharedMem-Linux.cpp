#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sharedMem-Linux.h"
#include "../../hal/include/hal/i2c.h"



// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------
#define PRU_ADDR 0x4A300000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000 // Length of PRU memory
#define PRU0_DRAM 0x00000 // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000 // Offset to shared memory
#define PRU_MEM_RESERVED 0x200 // Amount used by stack and heap

// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ((uintptr_t)(base) + PRU0_DRAM + PRU_MEM_RESERVED)
static volatile void *pPruBase;
volatile sharedMemStruct_t* globalSharedStruct = 0x0;

// Return the address of the PRU's base memory
static void getPruMmapAddr(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }
    // Points to start of PRU memory.
    pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE,
                    MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void sharedMemInit(void) {
    i2c_runCommand("config-pin P8_11 pruout");
    i2c_runCommand("echo start | sudo tee /sys/class/remoteproc/remoteproc1/state || true");
    // Get access to shared memory for my uses
    getPruMmapAddr();
    globalSharedStruct = (volatile sharedMemStruct_t*) PRU0_MEM_FROM_BASE(pPruBase);

}

void sharedMemCleanUp(void) {
    globalSharedStruct->isProgramFinished = true;
    if (munmap((void*) pPruBase, PRU_LEN)) {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}
