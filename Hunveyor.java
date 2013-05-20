import java.io.IOException;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.math.BigInteger;

public class Hunveyor {

  /**
   * @param args
   * @throws IOException 
   */
  public static void ReadTemperature(int address) throws IOException {
    I2CBus i2cBus = I2CFactory.getInstance(I2CBus.BUS_1);
    I2CDevice temp = i2cBus.getDevice(address);
    temp.write((byte)0x51);
    Thread.sleep(50);
    temp.write((byte)0xAA);

    byte[] buffer = new byte[2];
    temp.read(buffer, 0, 2);
    byte msb = buffer[0];
    byte lsb = buffer[1];

    if (BigInteger.valueOf(msb).testBit(7)) { // sign bit 
      msb -= 256;
    }


    System.out.println("Temp: "+(msb+lsb*0.00625));
    for(byte b : buffer) {
      System.out.println("Address: "+address+" Data : "+ b +" hex 0x"+Integer.toHexString((b & 0xff)));
    }
  }
  public static void main(String[] args) throws IOException {
    //ReadTemperature(75);
    //ReadTemperature(76);

    // TODO Auto-generated method stub


    for(int i=0;i<80;i++) {
      try {
        ReadTemperature(i);
      } catch(java.io.IOException e) 
      {
        //System.out.println("Address: "+i+" nem nyert");
      }
    }
    /*I2CDevice device = i2cBus.getDevice(0x4B);
    System.out.println(i2cBus);
    tempSensor.write((byte)0xAA);
    
     byte[] buffer = new byte[2];
     int amountBytes = tempSensor.read(1, buffer, 0, 2);
     System.out.println("Amount of byte read : " + amountBytes);
     for(byte b : buffer) {
              System.out.println("Data : "+ b +" hex 0x"+Integer.toHexString((b & 0xff)));
      }
      */
    
  }

}

