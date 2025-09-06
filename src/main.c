#include <stdio.h>
#include <stdlib.h>		// For exit()
#include <string.h>		// For bzero() or memset()
#include <unistd.h>		// For read(), write(), close()
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 100

/*	struct sockaddr_in {
		short			sin_family;		// e.g. AF_INET
		unsigned short	sin_port;		// Port number
		struct in_addr	sin_addr;		// IP address
		char			sin_zero[8];	// Not used, just for padding
};*/


int main(void) {
	int sockfd;
	char buffer[BUFFER_SIZE];

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("ERROR opening socket");
		exit(1);
	 } else {
		printf("Socket created successfully; sockfd: %d\n", sockfd);
	}
	

	struct sockaddr_in server_addr = {
		.sin_family			= AF_INET,
		.sin_port			= htons(8080),
		.sin_addr.s_addr	= INADDR_ANY
	};

	struct sockaddr_in client_addr = {0};
	socklen_t client_len;
	int client_sockfd;

	if ( bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ) {
		perror("ERROR binding socket");
		exit(1);
	} else {
		printf("Socket bind successful.\n");
	}

	if ( listen(sockfd, 10) < 0 ) {
		perror("ERROR listening on socket");
		exit(1);
	} else {
		printf("Listening...\n");
	}

	size_t received_bytes;
	while(1) {
		client_len = sizeof(client_addr);
		client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

		if (client_sockfd < 0) {
			perror("ERROR accepting connection");
		} else {
			printf("Connection accepted successfully.\n");
		}

		if ( (received_bytes = read(client_sockfd, buffer, BUFFER_SIZE) ) < 0 ) {
			perror("Error receiving data on socket");
			exit(1);
		} else {
			printf("Received data: [%s]\n", buffer);
		}

		if ( write(client_sockfd, buffer, received_bytes) < 0 ) {
			perror("ERROR sending data");
			exit(1);
		}

		if ( close(client_sockfd) < 0 ) {
			perror("ERROR closing the socket");
			exit(1);
		}
	}

	return 0;
}
