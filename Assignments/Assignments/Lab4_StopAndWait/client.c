#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct packet{
    char data[1024];
}Packet;

typedef struct frame{
    int seq_no;		// 0 and 1 alternate
    int ack;       	// acq will become 1 after getting acq from client
    Packet packet;  // data packet
}Frame;


void error(char* msg){
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]){

	int port_no;
	if(argc<2){
		printf("Port no. not provided");
		exit(1);
	}else{
		port_no = atoi(argv[1]);
		printf("client running on port %d \n", port_no);
	}

//PART 1
	int client_sock;
	struct sockaddr_in server_addr;
	char server_message[255], client_message[255];
	int server_struct_size = sizeof(server_addr);

	client_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(client_sock < 0){
		error("Error creating socket");
	}else{
		printf("Socket created successfully \n");
	}

	//setting up port and ip
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_no);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	char m1[] = "Hello this is client reporting to server";
    strcpy(client_message, m1);

	//Sending message
	if(sendto(client_sock, client_message, strlen(client_message), 0, 
		(struct sockaddr*) &server_addr, server_struct_size) < 0){
		error("Error sending message");
	}

	//Receiving message
	if(recvfrom(client_sock, server_message, sizeof(server_message), 0,
         (struct sockaddr*)&server_addr, &server_struct_size) < 0){
        error("Error receiving message");
    }

	printf("Server's response: %s \n", server_message);

//PART 2

	Frame frame_send;
	Frame frame_recv;
	frame_send.ack = 1;  //frame sent to server
	frame_recv.ack = 0;  //frame received
	int frame_id=1;

	while(1){
		
		int r1 = recvfrom(client_sock, &frame_recv, sizeof(frame_recv), 0,
			(struct sockaddr*) &server_addr, &server_struct_size);
		if(r1 == -1){
			error("Error receiving frame");
		}
		printf("Recieved frame %d \n", frame_id);
		
		int r2 = sendto(client_sock, &frame_send, sizeof(frame_send), 0, 
		(struct sockaddr*) &server_addr, server_struct_size);

		if(r2 == -1){
			error("Error sending");
		}
		printf("Acknowledgement sent \n");
		
		frame_id++;
	}

//END
	close(client_sock);
	return 0;
}
