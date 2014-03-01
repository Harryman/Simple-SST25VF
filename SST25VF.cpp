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

#include "SPI.h"
#include "SST25VF.h"

SST25VF::SST25VF(uint8_t csPin, uint8_t soPin){
	_csPin = csPin;
	_soPin = soPin;
	pinMode(_csPin, OUTPUT);
	digitalWrite(_csPin,HIGH);
	SPI.begin();
}

////////////// Comm Functions
void SST25VF::transInit(){
	SPI.setBitOrder(MSBFIRST);
	digitalWrite(_csPin, LOW);
}
void SST25VF::endTrans(){
	digitalWrite(_csPin,HIGH);
}

void SST25VF::writeEnable(){
	transInit();
	SPI.transfer(6);
	endTrans();
}

void SST25VF::writeDisable(){
	transInit();
	SPI.transfer(7);
	endTrans();
}

void SST25VF::enableBusy(){
	transInit();
	SPI.transfer(0x70);
	endTrans();
}

void SST25VF::disableBusy(){
	transInit();
	SPI.transfer(0x80);
	endTrans();
}

void SST25VF::addr(uint32_t addr){
	SPI.transfer(addr>>16);
	SPI.transfer(addr>>8);
	SPI.transfer(addr);
}

bool SST25VF::busyChk(){ /// returns false if busy, true if ready
	bool temp = 0;
	digitalWrite(_csPin, LOW);
	temp = digitalRead(_soPin);
	digitalWrite(_csPin, HIGH);
	return temp;
}
////////////// Action Functions

void SST25VF::readInit(uint32_t addr){
	transInit();
	SPI.transfer(3);
	addr(addr);
}

uint8_t SST25VF::read(){ // must run readInit, use endTrans to end read
	SPI.transfer(0);
}

void SST25VF::byteWrite(uint32_t addr, uint8_t data){
	writeEnable();
	digitalWrite(_csPin, LOW);
	SPI.transfer(2);
	addr(addr);
	SPI.transfer(data);
	endTrans();
}


bool SST25VF::AAI(uint16_t data){//returns false on fail
	if(busyChk() == true){
		transInit();
		SPI.transfer(0xad);
		SPI.transfer(data>>8);
		SPI.transfer(data);
		endTrans();
		return true;
	}
	return false;
}


void SST25VF::AAI(uint32_t addr, uint16_t data){
	enableBusy();
	writeEnable();
	digitalWrite(_csPin, LOW);
	SPI.transfer(0xad);
	addr(addr);
	SPI.transfer(data>>8);
	SPI.transfer(data);
	endTrans();
}

void SST25VF::endAAI(){
	if(busyChk() == true){
		writeDisable();
		disableBusy();
		return true;
	}
	return false;
}

void SST25VF::setBP(uint8_t level){//0-8 0,1/64,1/32,1/16,1/8,1/4,1/2,1
	writeStatus(level<<2);
}

void SST25VF::writeStatus(uint8_t regVal){
	writeEnable();
	digitalWrite(_csPin, LOW);
	SPI.transfer(1);
	SPI.transfer(regVal);
	endTrans();
}


uint8_t SST25VF::readStatus(){
	uint8_t temp = 0;
	transInit();
	SPI.transfer(0x5);
	temp = SPI.transfer(0);
	endTrans();	
	return temp;
}




void SST25VF::sectorErase(uint32_t addr){
	writeEnable();
	digitalWrite(_csPin, LOW);
	SPI.transfer(0x20);
	addr &= 0xFFF800; //sets remaining bits low
	addr(addr);
	endTrans();
	delay(25);
}

void SST25VF::blockErase(uint32_t addr, uint8_t size){ // 32k or 64k block
	writeEnable();
	digitalWrite(_csPin, LOW);
	if(size == 32){
		SPI.transfer(0x52);
		addr &= 0xFFC000; //sets remaining bits low
	}
	else{
		SPI.transfer(0xd8);
		addr &= 0xFF8000; //sets remaining bits low
	}
	addr(addr);
	endTrans();
	delay(25);
}

void SST25VF::chipErase(){//BP must not be on any sectors
	transInit();
	SPI.transfer(0xc7);
	endTrans();
}

////////////// Utility Functions

uint32_t SST25VF::readJID(){
	transInit();
	uint32_t id = 0;
	SPI.transfer(0x9f);
	id |= (SPI.transfer(0) <<16);
	id |= (SPI.transfer(0) << 8);
	id |= SPI.transfer(0);
	endTrans();
	return id;
}

uint8_t SST25VF::test(){// returns true when it reads JID successfully
	if(0xbf254a == readJID()){
		return 1;
	}
	return 0;
}
