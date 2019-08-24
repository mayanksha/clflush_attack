#include "../lib/util.h"

int main(int argc, char **argv) {

	char filename[51]; //buffer to receive filename
	char received_file[61] = "received_";
	unsigned int content_length = 0;
	clock_t t_recv;
	double recv_time;
	double recv_rate;

	// TODO: Establish your cache covert channel

	filename[0] = '\0';

	/* TODO:
	 * Receive the filename from the sender process over covert channel
	 * Use filename buffer to store received file name
	*/

	strcat(received_file, filename);

	// TODO: Create a binary file with the filename stored in received_file buffer in write mode.

	t_recv = clock();

	/* TODO:
	 * Receive image file contents from the sender process over covert channel
	 * Write them into the file opened earlier
	 * store the length of the image file in content_length variable
	*/




	t_recv = clock() - t_recv;
	recv_time = ((double) t_recv) / CLOCKS_PER_SEC;
	recv_rate = (double) (content_length * 8) / recv_time;

	printf("[Receiver] File (%s) received : %u bytes\n", received_file, content_length);
	printf("[Receiver] Time taken to receive file : %lf second\n", recv_time);
	printf("[Receiver] Data receiving rate : %lu bps\n", (unsigned long) recv_rate);

	return 0;

}