#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define MAX_MSG		4096
#define MYSELF_IP	"0.0.0.0"

/**
 * Written by Alberto De Angelis
 * This program is a simple server UDP who listens to an incoming message
 * and sends an UDP message to reply to the same address which sent it
 * to the given address. It basically works as a netcat -u -s
 * To compile: $>gcc serverUDP.c -o (FILENAME)
 * To execute: $>./FILENAME PORT MESSAGE
 * The PORT parameter is the PORT on which you want to listen.
 */

void errore(char* s, int n) {
	printf("ERRORE bloccante su %s\n", s);
	printf("#%d:%s\n", errno, strerror(errno));
	printf("Exiting with ret code = %d\n", n);
	exit(n);
}

int main(int argc, char** argv){
	int 	client_len; /*We're gonna put the size of the structure
						containing client's data here*/
	int 	msg_len;	/*This will contain the length of the message
						we have to send.*/
	int 	port;
	int 	ret;	/*This will contain the return value
					of some of the functions we are gonna use*/
	int 	sock_id;
	char 	buffer[MAX_MSG + 1];	/*This will be used to receive
									our client's message*/
	char* 	message;
	struct 	sockaddr_in client;	/*This structure will contain client's IP
								and port*/
	struct 	sockaddr_in myself;	/*This structure will contain
								our IP and port*/
/*Verifying correctness of input values*/
	if(argc != 3){
		printf("USAGE: %s PORT MESSAGE\n", argv[0]);
		return -1;
	}
/*Taking input values inside our variables*/
	port = atoi(argv[1]);
	message = argv[2];
/*Opening a socket for communication*/
	sock_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(!sock_id)errore("socket()",-2);
	printf("Socket opened\n");
/*Putting values inside the structure that should contain our IP and port*/
	myself.sin_family = AF_INET;
	inet_aton(MYSELF_IP, &myself.sin_addr);	/*Dovessimo passare l'ip del nostro pc sulla*/
	myself.sin_port = htons(port);		/*rete, useremmo "0.0.0.0"*/
	for(int i = 0; i < 8; i++)
		myself.sin_zero[i] = 0;
/*Verifying if any message from a client arrives*/
	client_len = sizeof(struct sockaddr_in);	
	ret = bind(sock_id,
		(struct sockaddr*) &myself,
		(socklen_t)client_len);
	if(ret)	errore("bind()", -3);
	ret = recvfrom(sock_id, buffer, MAX_MSG, 0,
		(struct sockaddr*)&client, (socklen_t*)&client_len);

	if(ret <= 0)	errore("recvfrom()", -4);
	buffer[ret] = '\0';
	printf("%s:%d - %s\nAnswering message...\n", inet_ntoa(client.sin_addr),
						ntohs(client.sin_port), buffer);
	msg_len = strlen(message) + 1;

	ret = sendto(sock_id, 
			message ,
			msg_len,
			0,
			(struct sockaddr*)&client,
			(size_t)sizeof(struct sockaddr_in));
	if(ret - msg_len)	errore("sendto()", -5);

	if(close(sock_id))	errore("close()", -6);
	printf("Socket closed\n");
	return 0;
}