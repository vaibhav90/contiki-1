/* This basically is a sequence the sensor will run that spits out a data 'notification' when the CUSUM has been reached */

#include "tool-sample-stats.h"
#include "contiki.h"
#include "net/rime.h"
#include "shell.h"
#include "global-sensor.h"

#include "dev/serial-line.h"
#include "lib/sky-math.h"
#include <stdio.h>
#include <string.h>

// Number of samples to be taken over 1 second time period
#define NUM_SAM 50

// Set to 1 if we want the test to stop after a decision has been met.
// Setting to 0 causes observations to be transmitted when decision == 1,
// runs continuously until a killall command is sent to it.
#define STOP_MODE 1

/*---------------------------------------------------------------------------*/
static struct mesh_conn mesh;

static void
sent(struct mesh_conn *c)
{
  printf("packet sent\n");
}

static void
timedout(struct mesh_conn *c)
{
  printf("packet timedout\n");
}

static void
recv(struct mesh_conn *c, const rimeaddr_t *from, uint8_t hops)
{
  printf("Data received from %d.%d: %s (%d)\n",
         from->u8[0], from->u8[1],
         (char *)packetbuf_dataptr(), packetbuf_datalen());

}

const static struct mesh_callbacks callbacks = {recv, sent, timedout};
/*---------------------------------------------------------------------------*/
uint16_t counter;
static uint16_t mean_0;
static uint16_t mean_1;
static uint16_t std_dev_0 = 1;
static uint16_t std_dev_1 = 1;
/*---------------------------------------------------------------------------*/
PROCESS(shell_sample_stats_process, "sample-stats");
SHELL_COMMAND(sample_stats_command,
		"sample-stats",
		"sample-stats: gathers data and finds mean and standard deviation",
		&shell_sample_stats_process);
PROCESS(shell_get_stats_process, "get-stats");
SHELL_COMMAND(get_stats_command,
		"get-stats",
		"get-stats: prints out the gathered data stats",
		&shell_get_stats_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_sample_stats_process, ev, data) {

	static uint16_t data_sam[NUM_SAM]; 
	uint16_t sum = 0;
	uint16_t sqsum = 0;
	static struct etimer etimer;

	PROCESS_BEGIN();

	// Gather NUM_SAM samples over 1 second of time
	sensor_init();
	printf("Gathering pre-change data... ");
	for(counter = 0;counter < NUM_SAM;counter++) {
		// Get data for no change analysis.
		etimer_set(&etimer, CLOCK_SECOND / NUM_SAM);
		PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
		data_sam[counter] = sensor_read();
	}
	printf("done!\n");
	sensor_uinit();

	// Sum the no change data
	sum = 0;
	for(counter = 0;counter < NUM_SAM;counter++) {
		sum = sum + data_sam[counter];
	}
	mean_0 = 0;
	printf("sum = %d\n",sum);
	mean_0 = sum/NUM_SAM;		// 155.3130
	printf("mean_0 = %d\n",mean_0);

	// Caclulate std_dev_0
	sqsum = 0;
	for(counter = 0;counter < NUM_SAM;counter++) {
		sqsum = sqsum + mypow2(abs_sub(data_sam[counter], mean_0));
	}
	std_dev_0 = 0;
	std_dev_0 = sqsum/NUM_SAM;	// 0.9947
	std_dev_0 = mysqrt(std_dev_0);
	printf("std_dev_0 = %d\n",std_dev_0);

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_get_stats_process, ev, data) {

	PROCESS_BEGIN();

	// Print out the data distribution stats, to both the local serial and
	// broadcast over the network as a response to a call.

	char message[32];
	strcpy(message,"mean_0 = ");			// 9 chars
	char num[5];
	itoa(mean_0, num, 10);
	strcat(message,num);					// 5 chars

	strcat(message,"\nstd_dev_0 = \n");		// 14 chars
	itoa(std_dev_0, num, 10);
	strcat(message,num);					// 5 chars

											// Total = 33 chars

	packetbuf_copyfrom(message, sizeof(message));

	rimeaddr_t addr;

	/*
	Addresses of the nodes:
	1: 97.43
	2: 88.56
	4: 62.41
	*/

	addr.u8[0] = 62;
	addr.u8[1] = 41;
    mesh_send(&mesh, &addr);

	// Print out the message on the local serial port.
	printf("%s\n",message);

	PROCESS_END();

}
/*---------------------------------------------------------------------------*/
void shell_sample_stats_init(void) {
	mesh_open(&mesh, 132, &callbacks);
	shell_register_command(&sample_stats_command);
	shell_register_command(&get_stats_command);
}
/*---------------------------------------------------------------------------*/