#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

typedef struct frame{
    int seq_no;		// 0 and 1 alternate
    int ack;       	// acq will become 1 after getting acq from client
    char data[255] ; // data packet
} Frame;


void error(char* msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){

	int port_no;

    if (argc < 2){
        printf("No Port number provided\n");
        exit(1);
    }else{
        port_no = atoi(argv[1]);
        printf("Starting server on port %d \n", port_no);
    }

//PART 1
	int server_sock;
	struct sockaddr_in server_addr, client_addr;
	char server_message[255], client_message[255];
	int client_struct_size = sizeof(client_addr);

	//Cleaning buffers
	memset(server_message, '\0', sizeof(server_message));
	memset(client_message, '\0', sizeof(client_message));

	//UDP implementation
	server_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_sock < 0){
		error("Error creating socket");
	}else{
		printf("Socket created successfully \n");
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_no);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


	//Binding
	if ( bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		error("Binding error");
	}else{
		printf("Binding successful \n");
	}

	printf("Listening for incoming messages...\n\n");

	//Receiving message
	if (recvfrom(server_sock, client_message, sizeof(client_message), 0,
        (struct sockaddr*)&client_addr, &client_struct_size) < 0){
        error("Error receiving");
    }
	
	printf("Received message from IP: %s and port: %i \n",
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	printf("Msg from client: %s \n", client_message);

	char m2[] = "Hello Client" ;
	strcpy(client_message, m2);

	//Sending message
    if (sendto(server_sock, server_message, strlen(server_message), 0,
         (struct sockaddr*)&client_addr, client_struct_size) < 0){
        error("Error sending message");
    }
    

//PART 2

	int frame_id=1;
	Frame frame_send;
	Frame frame_recv;
	frame_send.ack = 0;  // this will always be 0

	frame_send.seq_no = 0;

	while (1) //Always true
	{
		int ack_recv = 0;  // this will be 1 after recving
		printf(" \nFrame id = %d \n", frame_id);
		printf("Frame ack = %d \n", frame_send.ack);
		printf("Enter string: ");
		gets(frame_send.data);  //take input data for frame

		clock_t start_time, end_time;
   		double time_diff;
    	// Record start time
    	start_time = clock();

		//sending frame
		int r1 = sendto(server_sock, &frame_send, sizeof(frame_send), 0,
			(struct sockaddr*) &client_addr, client_struct_size);
		if (r1 == -1){
			error("Sending error");
		}else{
			printf("Frame sent \n");
		}

		//recv acknowledgement
		int r2 = recvfrom(server_sock, &frame_recv, sizeof(frame_recv), 0,
        (struct sockaddr*)&client_addr, &client_struct_size);
		if (r2 == -1){
			continue;   //if no ack is received, repeat with same seq no.
		}

		//record end time
		end_time = clock();
		
		printf("Frame received \n");
		printf("Frame acq. = %d \n", frame_recv.ack);

		// Calculate time difference
    	time_diff = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		printf("RTT: %.6f seconds \n", time_diff);


		if(frame_send.seq_no == 1){   //logic to alternate the ack
			frame_send.seq_no = 0;
		}else{
			frame_send.seq_no = 1;
		}
		frame_id++;
	}
	


//END
	close(server_sock);
	return 0;
}
