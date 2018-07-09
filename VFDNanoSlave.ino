// VFDNanoSlave
//
// Uses a Arduino Nano to drive a Noritake VFD dispay.  Not only does this allow me to use
// the known-good example code (that only works on the 328P, it seems) but it allows me to
// run the VFD in parallel mode while taking up only the I2C connection on the master chip
//
// The I2C Address must be agreed upon and not conflict with other I2C devices.  
//
// (c) Dave Plummer  07.05.2018 - First draft
//                   07.08.2018 - Made interfaces for master and slave side

#include <CUU_Interface.h>
#include <CUU_Parallel_I80.h>
#include <CUU_Parallel_M68.h>
#include <CUU_Serial.h>
#include <Noritake_VFD_CUU.h>
#include <util/delay.h>
#include <Wire.h>
#include "VFDi2c.h"

#define VFD_SLAVE_ADDRESS 0x05						// Arbitrarily chosen and agreed upon i2c address
#define VFD_WIDTH         20
#define VFD_HEIGHT        4

CUU_Parallel_I80 interface(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);	// Connections: RS,RW,E, D1-D7
VFDi2cSlave slaveDisplay;

// onReceiveEventHandler
//
// When an i2c request comes in, read the text from i2c and then dislay it on the VFD line

void onReceiveEventHandler(int c)
{
	Serial.println("onReceiveEventHandler");
	slaveDisplay.ReadAndProcessCommand();
}

void setup()
{
	Serial.begin(115200);
	_delay_ms(500);							// wait for device to power up

	slaveDisplay.Init(interface, VFD_WIDTH, VFD_HEIGHT);

	Wire.begin(VFD_SLAVE_ADDRESS);			// Begin i2c comm
	Wire.onReceive(onReceiveEventHandler);	// Set the event handler for when an i2c request comes in

	Serial.println("VFDNano Slave Ready");
	slaveDisplay.ClearScreen();
	slaveDisplay.print("Ready at i2c addr ");
	slaveDisplay.print(VFD_SLAVE_ADDRESS);
}

void loop()
{
	delay(10);
}