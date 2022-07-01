#include "i2c_master_noint.h"
#include "NU32.h"
#include <stdio.h>

int main() {
  char buf[100] = {};                       // buffer for sending messages to the user
  int master_read0  = 0x00;       // first received byte
  char master_read1  = 0x00;       // second received byte
  int a = 0;
  int b = 0;
  int c = 0;

  NU32_Startup();                 // cache on, interrupts on, LED/button init, UART init
  __builtin_disable_interrupts();
  i2c_master_setup();                       // init I2C2, which we use as a master
  __builtin_enable_interrupts();
  
  while(1) {
    NU32_WriteUART3("Master: Press Enter to begin transmission.\r\n");
    NU32_ReadUART3(buf,2);
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(0x40 << 1);       // send the slave address, left shifted by 1, 
    i2c_master_send(0x31);
    i2c_master_send(0x9F);
    i2c_master_restart();                   // send a RESTART so we can begin reading 
    for (int i = 0; i < 4; i++)
    {
      i2c_master_send((0x40 << 1) | 1); // send slave address, left shifted by 1,
                                            // and then a 1 in lsb, indicating read
      master_read0 = i2c_master_recv();       // receive a byte from the bus
      i2c_master_ack(0);                      // send ACK (0): master wants another byte!
      master_read1 = i2c_master_recv();       // receive another byte from the bus
                    // send STOP:  end transmission, give up bus
      i2c_master_ack(0);                      // send ACK (0): master wants another byte!

      a = (int) (master_read0);
      sprintf(buf,"Master Wrote: %d\r\n", a);
      NU32_WriteUART3(buf);      
      b = (int) (master_read1);
      sprintf(buf,"Master Wrote 2: %d\r\n", b);
      NU32_WriteUART3(buf);      
      a = a << 4;
      b = b ;
      c= a+b;


      sprintf(buf,"Master Wrote 2: %d\r\n\n\n", c);
      NU32_WriteUART3(buf);
    }
    
    
    i2c_master_ack(1);                      // send NACK (1):  master needs no more bytes
    i2c_master_stop();  
  }
  return 0;
}
