#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MAX_MSG 4096

/**
 * Written by Alberto De Angelis
 * This program is a simple client who sends an UDP message
 * to the given address. It basically works as a netcat -u
 * To compile: $>gcc clientUDP.c -o (FILENAME)
 * To execute: $>./FILENAME DESTINATION_IP DESTINATION_PORT MESSAGE
 */

void my_error(char* s, int n) {
	printf("ERROR on %s\n", s);
	printf("#%d:%s\n", errno, strerror(errno));
	printf("Exiting with ret code = %d\n", n);
	exit(n);
}

int main(int argc, char* argv[]){
	int 	msg_len;
	int 	port_dest;
	int 	ret;
	int 	socket_id;
	char* 	ip_dest;
	char* 	msg;
	struct 	sockaddr_in dest;	/*This structure will contain
					the IP and the port of the receiver.*/
/*Verifying correctness of input values*/
	if(argc != 4){
		printf("USAGE: %s IP_DEST PORT_DEST MESSAGE\n", argv[0]);
		return -1;
	}
	ip_dest = argv[1];
	port_dest = atoi(argv[2]);
	msg = argv[3];
	msg_len = strlen(msg) + 1;
/*Opening socket for communication*/
	printf("Opening socket...\n");	
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(!socket_id)		my_error("socket()",-2);
	printf("Socket opened\n");	
/*Trying to send something to a server with a socket opened on the same IP and port*/
	dest.sin_family = AF_INET;
	inet_aton(ip_dest, &dest.sin_addr);	/*Converting char* to IP and placing it inside receiver structure*/
	dest.sin_port = htons(port_dest);	/*Converting int to port to place it inside receiver structure*/
	for(int i = 0; i < 8; i++) dest.sin_zero[i] = 0;
	printf("Sending '%s' to %s:%d...\n", msg, ip_dest, port_dest);
	ret = sendto(socket_id,
		msg,
		msg_len,
		0, (struct sockaddr*)&dest,
		(size_t)sizeof(struct sockaddr_in));
	if((ret - msg_len)) my_error("sendto()", -3);
/*Seeing if server answers to my message*/
	printf("Waiting for a reply...\n");
	char buffer[MAX_MSG+1];
	struct sockaddr_in sender;
	int sender_len = sizeof(struct sockaddr_in);
	int rec_ret = recvfrom(socket_id,
			(void*)buffer,
			MAX_MSG,
			0,
			(struct sockaddr*)&sender,
			(socklen_t*)&sender_len);

	if (ret <= 0)	my_error("recvfrom()", -4);
	buffer[ret] = '\0';
/*Taking sender's data*/
	char* ip_sender = strdup(inet_ntoa(sender.sin_addr));
	int port_sender = ntohs(sender.sin_port);
	printf("%s:%d - '%s'\n", ip_sender, port_sender, buffer);
/*Doing close operations*/	
	free(ip_sender);
	printf("Closing socket...\n");
	if(close(socket_id))	my_error("close()", -5);
	printf("Socket closed\n");
	return 0;
}
