#include "../lib/util.h"

void do_something(){
	for(int i=0;i<50000;i++){
		int j = i*2;
		j++;
	}
	return;
}

int main(int argc, char **argv) {

	char msg[51]; //buffer to receive data
	unsigned int msg_len = 0;
	clock_t t_recv;
	double recv_time;
	double recv_rate;
	msg[0] = '\0';
	
	// TODO: Establish your cache covert channel

	t_recv = clock();

	// TODO: synchronize with sender and receive data in msg buffer.

	map_handle_t *handle;     // declaring a handle for file mapping
  	char *map;

  	map = (char *) map_file("/home/vinayakt/test.txt", &handle);
  	if(map==NULL){
  		return -1;
  	}

  	CYCLES a,b;
  	int characters = 0;
    clflush(map);
    clflush(map+512);
  	while(1){
      char y = *map;
  		clflush(map);
  		//do_something();
  		a = rdtscp();	
  		char x = *(map);
  		b = rdtscp();
  		//printf("%d a\n",b-a);
  		if((b-a) < 50 ){
  			printf("A");
        //break;
  			characters++;
  		}

      
  		
  		// clflush(map+512);
  		// //do_something();

  		// a = rdtscp();	
  		// x = *(map+512);
  		// b = rdtscp();
  		// //printf("%d \n",b-a);
  		// if((b-a) < 50 ){
  		// 	printf("B");
  		// 	characters++;
  		// }
  		

  	}
    // while(1){
    //   char x = *(map);
    // clflush(map);
    // a = rdtscp();
    // x = *(map);
    // b = rdtscp();
    // printf("time is %d\n",b-a);  
    // }
    
  	
  	//printf("2nd %x\n",measure_one_block_access_time(map));




	t_recv = clock() - t_recv;
	msg_len = strlen(msg);
	recv_time = ((double) t_recv) / CLOCKS_PER_SEC;
	recv_rate = (double) (msg_len * 8) / recv_time;

	printf("[Receiver] Received data : %s\n", msg);
	printf("[Receiver] Total data received : %u bytes\n", msg_len);
	printf("[Receiver] Time taken to receive data : %lf second\n", recv_time);
	printf("[Receiver] Data receiving rate : %lu bps\n", (unsigned long) recv_rate);

	return 0;

}
