/*All rights reserved.
 *
 * Eigen Technologies Pvt. Ltd.
 * New Delhi
 * www.eigen.in
 *
 * This source code is provided to SensenutsTM  users for
 * research purpose only. No portion of this source code
 * may be used as the basis of development of any similar
 * kind of product.
 *
 */

#include <jendefs.h>
#include <AppHardwareApi.h>
#include <AppQueueApi.h>

#include "clock.h"  //provides clock related functionality
#include "node.h"
#include "task.h"

/*******************************Do not modify above this line********************************/

//add header files for temperature and light sensors
#include "tmpSensor.h"
#include "lightSensor.h"
#include "dio.h"

#include "adc.h"

//add header files for mac layer APIs
#include "mac.h"

//add header file for routing APIs
#include "routing.h"

#include "pcInterface.h"
//task type to be used in this demo
#define SEND_PACKET_TO_MAC 0

#define READ_ADC 1
#define time 100
void Led_on();
void Led_off();
int counter=0;
//user defined packet type should always be greater than 0x23 as routing protocols
//define their packet types below this value.
#define USER_PACKET_TYPE 0x30
#define USER_PACKET_TYPE1 0x31
uint8 packetfrompc[3];
uint8 packet[6];
uint8 packetrelay[4],i;
//*******************************************************************************
//Program execution starts from this function. It is like main function
//*******************************************************************************
void startNode()
{
//initialize sensors
	tmpInit();
	lightSensorInit();

//initialize mac layer
	macInit();

//initialize routing
	routingInit();

	eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER,0);

	sendToPcInit();
	setoutput(16);
/*set task to read sensor info and send the data to mac. If
  the coordinator has already associated, it will broadcast
  this packet or else it will drop the packet.*/

	addTask(USER,READ_ADC,5*SECOND);
}

//*******************************************************************************
//              All the tasks added by user expire in this function
//*******************************************************************************
void userTaskHandler(uint8 taskType)
{
	uint8 packet[6],tmp;
		uint16 light;
	    float adc1Val, adc2Val, adc3Val;
	    float adc4Val;
	    uint16 nodeId = getNodeId();
	    bool check;

	switch (taskType)

	{


	case READ_ADC:
	// enable power to analog interface with internal reference and no interrupts
			enableAnalog(INTERRUPT_OFF, INTERNAL_REF);
			// read from ADC1
			adc1Val = adcRead(ADC1, SINGLE_SHOT);
			// disable ADC, needed only in continuous mode and not in single shot
		        disableAdc();
			// reading from ADC4
			adc4Val = adcRead(ADC4, SINGLE_SHOT);
			adc2Val = 4.123;
			adc3Val = 5.456;
			disableAdc();
			// disabling power to Analog interface, saves power
			disableAnalog();
			// indication for conversion completion

			adc1Val *= 1000;
			adc2Val *= 1000;
			adc3Val *= 1000;
			adc4Val *= 1000;
			packet[0]=USER_PACKET_TYPE;
			packet[1]=(uint16)adc1Val>>8;
			packet[2]=(uint16)adc1Val;
			packet[3]=(uint16)adc2Val>>8;
			packet[4]=(uint16)adc2Val;
			packet[5]=(uint16)adc3Val>>8;
			packet[6]=(uint16)adc3Val;
			packet[7]=(uint16)adc4Val>>8;
			packet[8]=(uint16)adc4Val;
			packet[9] = nodeId >> 8; //source address
			packet[10] = nodeId;
			check = routingSendData(packet,11,0);
			if (check <= 0)
			{
				debug("error", 5*sizeof(char));
			}
			// updating ADC values to PC
			UpdateSLbADC(nodeId,adc1Val,adc2Val,adc3Val,adc4Val);
			// repeating the task again
			addTask(USER,READ_ADC,500*MILLI_SECOND);

			break;

	case SEND_PACKET_TO_MAC:
	//read sensor values
	tmp=readTmp();
	light=readLux();

	//create the packet to send on network
	packet[0]=USER_PACKET_TYPE;	//packet type info
	packet[1]=tmp;				//temperature
	packet[2]=light>>8;			//light info
	packet[3]=light;
	packet[4]=nodeId>>8;		//source address
	packet[5]=nodeId;

	//send the packet to  routing protocol for further processing
	check=routingSendData(packet, 6, 0);	//0 is the destination address

	if (check<=0)
	{
		//packet is dropped
	}

	//repeat the same process
	addTask(USER,SEND_PACKET_TO_MAC,1*SECOND);
	break;
	}
}

//*******************************************************************************
//When a data packet from network is received, this function gets called from OS.
//*******************************************************************************
void userReceiveDataPacket(uint8* payload,uint8 length,uint16 prevAddr,uint8 linkQuality)
{
	if (payload[0]==USER_PACKET_TYPE1)
			{

				if(payload[1] > 0x05)
				{
					ledOn();
					Led_on();
				}

				else
				{
					ledOff();
					Led_off();
				}

			}
}
void Led_on()
{
		setPin(16);
		msdelay(time);
}
void Led_off()
{
		clearPin(16);
		msdelay(time);
}

//*******************************************************************************
//                  A dio event is received in this function. 
//*******************************************************************************
void userCriticalTaskHandler(uint8 critTaskType)
{
	
}

//#########################################################################################################################################
//														A very special function
//#########################################################################################################################################

//*******************************************************************************
//  this function gets called if receive from pc is enabled and a byte 
//  is received from pc. This is getting called from interrupt handler
//  unlike other functions which were all scheduled by OS. This being 
//  an interrupt handler, must be having least possible statements so 
//  that the device comes back to normal operation out of interrupt quickly.
//*******************************************************************************
void pcInterruptHandler()
{
}
