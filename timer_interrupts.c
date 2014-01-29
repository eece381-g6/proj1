#include <stdio.h>
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "sys/alt_irq.h"


void handle_timer_interrupts(void);
unsigned int tps;
alt_u32 alarm_1_callback(void* context);
alt_u32 alarm_2_callback(void* context);
int buffer[] = {5,4,3,2,1};



alt_u32 alarm_1_callback(void* context) //function called when alarm 1 interrupts
{
	static unsigned int count =0;
	int buff = buffer[count++];
	printf("alarm 1: %d\n", buff);
	if (count == 5) count = 0;
	return buff*tps; // the value returned is used as the new period
}
alt_u32 alarm_2_callback(void* context) //function called when alarm 2 interrupts
{
	static unsigned int count =0;
	printf("alarm 2: %d\n",++count);
	return tps;
}


int flag = 0;
void handle_timer_interrupts(){ //Interrupt for when hardware timer counts to zero
	printf("successfully interrupted\n");
	flag = 1;
	IOWR_16DIRECT(TIMER_0_BASE,0,0); //needed to show that interrupt finished executing
	IOWR_16DIRECT(TIMER_0_BASE,4,0x5); //restarts the hardware timer before exiting the isr
	return;
}



int main(void)
{

	alt_alarm alarm_1; //defines alarms
	alt_alarm alarm_2;


	printf("starting alarms\n");

	tps = alt_ticks_per_second();

	printf("ticks per second: %d\n",tps);

	alt_alarm_start(&alarm_1,5*tps,alarm_1_callback,NULL);
	//starts alarms by passing in an alarm obj, the ticks to count down, the function to run, and what to pass into the function
	alt_alarm_start(&alarm_2,tps,alarm_2_callback,NULL);

	

	printf(" Hardware-Only Timer\n");
	printf(" Setting timer period to 5 seconds.\n");
	int timer_period = 5 * 50000000;
	int status;
	int done;
	IOWR_16DIRECT(TIMER_0_BASE, 8, timer_period & 0xFFFF); //writes the period to the hardware timer
	IOWR_16DIRECT(TIMER_0_BASE, 12, timer_period >> 16);

	alt_irq_register(TIMER_0_IRQ,NULL,(void*)handle_timer_interrupts);//registers function to a specific IRQ

	printf(" Stopping Timer\n");
	status = IORD_16DIRECT(TIMER_0_BASE, 0);
	if (status & 0x2) {
	IOWR_16DIRECT(TIMER_0_BASE, 4, 1 << 3); //stops timer
	}
	printf(" Starting Timer\n");
	IOWR_16DIRECT(TIMER_0_BASE, 4, 0x5);//starts timer with interrupt enabled


	printf(" Waiting for timer to expire...\n");

	done = 0;
	while (! done) {
	status = IORD_16DIRECT(TIMER_0_BASE, 0);
	done = status & 0x1;
	}
	printf(" 5 seconds timer is done\n");

	while (!flag){
		;
	}
	printf("successfully exit isr\n");



	while(1);
}
