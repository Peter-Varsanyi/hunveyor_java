#include "RPI.h"
#include <string.h>

#define DEBUG 0

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int map_peripheral(struct bcm2835_peripheral *p)
{
   // Open /dev/mem
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }
 
   p->map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      p->mem_fd,      // File descriptor to physical memory virtual file '/dev/mem'
      p->addr_p       // Address in physical map that we want this memory block to expose
   );
 
   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }
 
   p->addr = (volatile unsigned int *)p->map;
 
   return 0;
}
 
void unmap_peripheral(struct bcm2835_peripheral *p) {
 
    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}
void i2c_init()
{
    INP_GPIO(0); // 0 pin as input
    SET_GPIO_ALT(0, 0); // 0 pin as alternate(input/output)
    INP_GPIO(1); // 1 pin as input
    SET_GPIO_ALT(1, 0); // 1 pin as alternate(input/output)
}  
 
// Function to wait for the I2C transaction to complete
void wait_i2c_done() {

  if (DEBUG)  {
    printf("status before completion - ");
    dump_bsc_status();
    printf("waiting for transaction to complete...\n");
  }
  int timeout = 50;
  while((!((BSC0_S) & BSC_S_DONE)) && --timeout) {
    usleep(1000);
  }
  if(timeout == 0)
    printf("Error: wait_i2c_done() timeout.\n");
  if (DEBUG) {
    printf("status after completion - ");
    dump_bsc_status();
  }
}

void i2c_scan_bus() {
  
  int device;

  for(device = 3; device < 119; device++) {
    if (DEBUG) {
      printf("Testing: %x\n", device);
    }
    BSC0_A = device; // address 0x4C, configured by registers http://datasheets.maximintegrated.com/en/ds/DS1631-DS1731.pdf

    BSC0_C = CLEAR_FIFO;
    BSC0_DLEN = 1;            // length: one byte
    BSC0_FIFO = 0x1;
    BSC0_S = CLEAR_STATUS;    // Reset status bits (see #define)
    BSC0_C = START_WRITE;     // Start Write (see #define)
    wait_i2c_done();
    if ((BSC0_S & BSC_S_TXE) != 0) {
      printf("%x address working\n", device);
    }
  }
}

int main(int argc, char *argv[]) {

  /* Gain access to raspberry pi gpio and i2c peripherals */
  if(map_peripheral(&gpio) == -1) {
    printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
    return -1;
  }
  if(map_peripheral(&bsc0) == -1) {
    printf("Failed to map the physical BSC0 (I2C) registers into the virtual memory space.\n");
    return -1;
  }

  i2c_scan_bus();

  BSC0_C = CLEAR_FIFO; // clearing bytes left in FIFO
  BSC0_A = 0x4C; // address 0x4C, configured by registers http://datasheets.maximintegrated.com/en/ds/DS1631-DS1731.pdf

  BSC0_DLEN = 1;            // length: one byte
  BSC0_FIFO = 0xAA;            // value 0xAA (read temperature)
  BSC0_S = CLEAR_STATUS;    // Reset status bits (see #define)
  BSC0_C = START_WRITE;     // Start Write (see #define)


  wait_i2c_done(); // wait for done bit

  BSC0_DLEN = 2;   // length: 3 byte
  BSC0_S = CLEAR_STATUS;  // Reset status bits (see #define)
  BSC0_C = START_READ;    // Start Read after clearing FIFO (see #define)

  wait_i2c_done(); // wait for done bit

  printf("Read: %x %x\n",BSC0_FIFO,BSC0_FIFO);

  unmap_peripheral(&gpio);
  unmap_peripheral(&bsc0);
}
