General Purpose info:
/*********************************************************************************************
 In any user code at least following 5 functions must be present
 
 void startNode():
  	  	  	  	  This function is initialization function of the node

 void userTaskHandler(uint8 taskType):
 	 	 	 	  This function is called when the timer for a task previously
 	 	 	 	  placed by user by calling addTask has expired. The user should
 	 	 	 	  handle thetaskType here.

void userCriticalTaskHandler(uint8 critTaskType):
				  This function is called when a task with higher priority is to
				  be handled. Currently this is used in context of alerts generated
				  by light and temperature sensors when their threshold settings are
				  exceeded. critTaskType is to be compared with TEMP_CRIT_TASK or
				  LIGHT_CRIT_TASK to know which sensor has generated the alert.
				  
void userReceiveDataPacket(uint8* payload,uint8 length,uint16 prevAddr,uint8 linkQuality):
				  This function is called when a data packet intended for the node 
				  is received from the mac layer.

void pcInterruptHandler():
				  This function gets called if receive from pc is enabled and a byte 
				  is received from pc. This is getting called from interrupt handler
				  unlike other functions which were all scheduled by OS. This being 
				  an interrupt handler, must be having least possible statements so 
				  that the device comes back to normal operation out of interrupt quickly.
If the user doesn't require  the use of any of these functions, the definition could be 
left empty. 

startNode() can never be left empty. Initialization of modules that are to be used in 
program has to be done here.
**********************************************************************************************/

Demo Purpose: This demo is to show how a coordinator can send data to pan coordinator multihops away using MBR.
			  
Demo brief:	  This demo requires use of startNode() and void userTaskHandler(uint8 taskType)
		  	  functions and  void userReceiveDataPacket(uint8* payload,uint8 length,uint16 prevAddr,
		  	  uint8 linkQuality).
			  First, we initialize modules used using tmpInit, lightSensorInit, sendToPcInit, 
			  macInit,routingInit. After that a task to send data  after 1 second is added. Then on expiry, 
			  we read data and check if route to destination is available or not. If available the 
			  is sent towards destination, if not, route discovery may be triggered. To make it 
			  periodic, a new task is added to be executed after 1 sec.
			  
			  A macro defined in config.h defines whether the node is coordinator or pan coordinator. 
			  FFD_COORDINATOR means coordinator, FFD_PAN_COORDINATOR means pan coordinator. 
			  
			  Another macro defined in config file defines the routing protocol to use in the network.
			  MAC_BASED_ROUTING means mac based routing, LEVEL_BASED_ROUTING means hop level based
			  routing, AODV means AODV.
			  
			  Here MAC_BASED_ROUTING is defined in config file.

Requirements: For using temperature sensor and light sensor, tmpSensor.h and lightSensor.h must be 
			  included. For sending data to pc, sendToPc.h must be included. For sending data to mac, mac.h
			  must be included.	For routing routing.h must be included.		  