#include <stdio.h>
#include <stdlib.h>		// For exit()
#include <string.h>		// For bzero() or memset()
#include <unistd.h>		// For read(), write(), close()
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 100


void* handle_client(void* arg) {
	int *client_sock_ptr = (int *)arg;
	int client_sockfd = *client_sock_ptr;
	free(client_sock_ptr);

	char buffer[BUFFER_SIZE];

	size_t received_bytes;
	if ( (received_bytes = read(client_sockfd, buffer, BUFFER_SIZE) ) < 0 ) {
		perror("Error receiving data on socket");
		return NULL;
	} else {
		printf("Received data: [%s]\n", buffer);
	}

	if ( write(client_sockfd, buffer, received_bytes) < 0 ) {
		perror("ERROR sending data");
		return NULL;
	}

	if ( close(client_sockfd) < 0 ) {
		perror("ERROR closing the socket");
		return NULL;
	}
	return (void*)"Connection closed successfully.";
}

int main(void) {
	int sockfd;
	pthread_t client_handler_thread;

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

	while(1) {
		client_len = sizeof(client_addr);
		client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
		
		if (client_sockfd < 0) {
			perror("ERROR accepting connection");
		} else {
			printf("Connection accepted successfully.\n");
		}

		int *new_sock = (int *)malloc(sizeof(int));
		*new_sock = client_sockfd;

		pthread_create(&client_handler_thread, NULL, handle_client, (void*)new_sock);


	}

	return 0;
}
