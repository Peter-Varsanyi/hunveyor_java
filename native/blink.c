#include "RPI.h"
 
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

void select_port(int number) {
  INP_GPIO(8);
  OUT_GPIO(8);
  INP_GPIO(25);
  OUT_GPIO(25);
  INP_GPIO(7);
  OUT_GPIO(7);
  GPIO_SET = 1 << 7;
  GPIO_CLR = 1 << 8;
  GPIO_CLR = 1 << 25; // clear both bits
  switch(number) {
    case 1:
    GPIO_SET = 1 << 8;
      break;
    case 2:
    GPIO_SET = 1 << 25;
      break;
    case 3:
    GPIO_SET = 1 << 8;
    GPIO_SET = 1 << 25;
      break;
  }
  GPIO_CLR = 1 << 7;

}
void output(int number) {
  int i;
  int pins[10] = {4,17,22,10,9,11,23,24,25,8}; // raspberry PI named pins
  for(i=0;i<8;i++) { // clear the pins first
      INP_GPIO(pins[i]);
      OUT_GPIO(pins[i]); // define pin as output
      GPIO_CLR = 1 << pins[i];
    
  }
  for(i=0;i<8;i++) {
    if (number & 1 << i+1) { // if the pin should be active, then GPIO_SET
      GPIO_SET = 1 << pins[i];
    }
  }
}
  
int main()
{
  if(map_peripheral(&gpio) == -1) 
  {
    printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
    return -1;
  }
 select_port(1);
 sleep(1);
 select_port(2);
 sleep(1);
 select_port(3);
 sleep(1);
 select_port(4);
  output(16);
  return 0; 
}
