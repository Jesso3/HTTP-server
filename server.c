#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Wrong number of inputs");
    return -1;
  }

  int sockfd, newsockfd, portn, n;
  socklen_t clilen;
  portn = atoi(argv[1]);
  char *address = "127.0.0.1";
  char buffer[256];
  struct sockaddr_in server_addr, cli_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portn);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("problem connecting socket");
    return -1;
  }

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    printf("problem binding");
    return -1;
  }
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  if (newsockfd < 0) {
    printf("problem with newsock");
    return -1;
  }

  bzero(buffer, 256);
  n = read(newsockfd, buffer, 255);
  if (n < 0) {
    printf("error reading from socket");
    return -1;
  }
  printf("here is you message: %s", buffer);
  printf("Sending response message\n");
  n = write(newsockfd, "I got your message", 18);
  if (n < 0) {
    printf("error writing to socket");
    return -1;
  }
  close(newsockfd);
  close(sockfd);

  return 0;
}
