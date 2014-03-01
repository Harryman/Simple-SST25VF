// file generated using https://github.com/Harryman/Arduino-header-keyword-creator

/*Developed by:
/ Harrison Stahl https://github.com/Harryman
/ Donate: 15Hnv9T4fr9uzKMNgW5Vbop52Aq9YwSYvz
/ The Everything Corp
+++++++++++++++++++++++++++++++++++
/ Simple Library for the SST25VF032B SPI Flash Memory chip
/
/ This library is pretty cut and dry there aren't a lot of frills
/ you would be wise to look at the datasheet to get a good idea how
/ to use this library but I will give you a quick run down of how
/ to get up and running with this.

/ 1. The chip boots up with all blocks protected so you will need to 
	setBP(0) to unlock all sectors

/ 2. This library assumes that you are using the SO pin to indicate 
	weather the chip is currently available or not, instead of repeatingly
	polling the status register. This blocks the SPI from being used so 
	AAI and endAAI commands will return false if they fail due to busy condition
	and will need to be ran again to execute the command. If you are excuting
	other write or erase commands you back to back you may want to run busyChk()
	to make sure your command goes through, it will return true when ready



*/

#ifndef SST25VF_h
#define SST25VF_h

#include "Arduino.h"

class SST25VF{
  public:
    SST25VF(uint8_t csPin, uint8_t soPin);
    void transInit();
    void endTrans();
    void writeEnable();
    void writeDisable();
    void enableBusy();
    void disableBusy();
    void addr(uint32_t addr);
    bool busyChk(); /// returns false if busy, true if ready
    void readInit(uint32_t addr);
    uint8_t read(); // must run readInit, use endTrans to end read
    void byteWrite(uint32_t addr, uint8_t data);
    bool AAI(uint16_t data);//returns false on fail
    void AAI(uint32_t addr, uint16_t data);
    void endAAI();
    void setBP(uint8_t level);//0-8 0,1/64,1/32,1/16,1/8,1/4,1/2,1
    void writeStatus(uint8_t regVal);
    uint8_t readStatus();
    void sectorErase(uint32_t addr);
    void blockErase(uint32_t addr, uint8_t size);
    void chipErase();//BP must not be on any sectors
    uint32_t readJID();
    uint8_t test();// returns true when it reads JID successfully

    //????temp;
    //????id;

  private:

    uint8_t _csPin;
    uint8_t _soPin;

};

#endif