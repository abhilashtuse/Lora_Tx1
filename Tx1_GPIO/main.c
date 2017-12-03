/*
===============================================================================
 Name        : RF_Handshaking.c
 Description : RF and PWM code for LPC1769

 CTI One Corporation released for Dr. Harry Li for CMPE 245 Class use ONLY!
===============================================================================
*/

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "tx1_lora.h"

#define RF_Receive 1
#define TransmittACk 0
#define ack_start_stop 0
int rfInit(void);

char receiveData=0;
int packetSize;



/**************************************************************************************************
* @brief	wait for ms amount of milliseconds
* @param	ms : Time to wait in milliseconds
**************************************************************************************************/
static void delay_ms(unsigned int ms)
{
    unsigned int i,j;
    for(i=0;i<ms;i++)
        for(j=0;j<50000;j++);
}


/**************************************************************************************************
* main : Main program entry
**************************************************************************************************/
int main(void)
{
	// Working frequency range from 724 MHz to 1040 MHz.
	//LoRabegin(1040000000);
	//LoRabegin(1020000000);
	//LoRabegin(724000000);
	//LoRabegin(750000000);
	//LoRabegin(790000000);
	//LoRabegin(800000000);
	//LoRabegin(845000000);
	//LoRabegin(850000000);
	//LoRabegin(910000000);
	//LoRabegin(868000000);
	LoRabegin(915000000);
	int counter =0;
	//timer_initialise();

	/* Start the tasks running. */
	//vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
#if RF_Receive
	while(1)
	{
		packetSize = parsePacket(0);
		if (packetSize)
		{
			counter = 0;
			//NVIC_EnableIRQ(TIMER0_IRQn);
			//received a packet
//			printf("Received packet '");
			// read packet
			while (available())
			{
				counter = 0;
				receiveData = lora_read();
				//printf("%c",receiveData);
				if(receiveData == 'A')
				{
					printf("Received packet '");
					printf("%c",receiveData);
					printf("' with RSSI ");
					printf("%d\n",packetRssi());
				}
				else if(receiveData == 'B')
				{
					printf("Received packet '");
					printf("%c",receiveData);
					printf("' with RSSI ");
					printf("%d\n",packetRssi());
				}
				else if(receiveData == 'C')
				{
					printf("Received packet '");
					printf("%c",receiveData);
					printf("' with RSSI ");
					printf("%d\n",packetRssi());
				}
				else if(receiveData == 'D')
				{
					printf("Received packet '");
					printf("%c",receiveData);
					printf("' with RSSI ");
					printf("%d\n",packetRssi());
				}
				else if(receiveData == 'E')
				{
					printf("Received packet '");
					printf("%c",receiveData);
					printf("' with RSSI ");
					printf("%d\n",packetRssi());
				}
				else if(receiveData == 'F')
				{
					printf("Received packet '");
					printf("%c",receiveData);
					printf("' with RSSI ");
					printf("%d\n",packetRssi());
				}


			}

			// print RSSI of packet
//			printf("Received packet '");
//			printf("%c",receiveData);
//			printf("' with RSSI ");
//			printf("%d\n",packetRssi());
		}

	}

#if TransmittACk
	const char buffer[] = "Data from TX1";
	char Acknowledgement;
	Acknowledgement = 'A';
	while(1)
	{
		printf("Start Sending data \n");
		delay_ms(1000);
		LoRabeginPacket(0);
		//writebyte(Acknowledgement);
		write(buffer, sizeof(buffer));
		LoRaendPacket();
		printf("Data sent \n");

	}

#endif

#endif
finish(); //unexport gpio
}

