#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Wrong number of inputs");
    return -1;
  }

  int sockfd, portn, n;
  portn = atoi(argv[2]);
  char *address = "127.0.0.1";
  char buffer[256];
  struct sockaddr_in server_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portn);
  server_addr.sin_addr.s_addr = inet_addr(address);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("Error opening socket\n");
    return -1;
  }

  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    printf("Error connecting");
    return -1;
  }

  printf("Please enter the message: ");
  bzero(buffer, 256);
  fgets(buffer, 255, stdin);

  n = write(sockfd, buffer, strlen(buffer));
  if (n < 0) {
    printf("Error writing to socket");
    return -1;
  }
  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);
  if (n < 0) {
    printf("Error reading from socket");
    return -1;
  }
  printf("%s\n", buffer);
  close(sockfd);

  return 0;
}
