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

  int serverfd, clientfd, portn, n;
  socklen_t clilen;
  portn = atoi(argv[1]);
  char *address = "192.168.178.25";
  char buffer[256];
  const char *res = "<html><body><p>baller</p><script>console.log('balled')</"
                    "script></body></html>";
  int resLength = strlen(res);
  struct sockaddr_in server_addr, cli_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portn);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  serverfd = socket(AF_INET, SOCK_STREAM, 0);
  if (serverfd < 0) {
    printf("problem connecting socket");
    return -1;
  }

  if (bind(serverfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    printf("problem binding");
    return -1;
  }
  listen(serverfd, 5);

  while (1) {
    clientfd = accept(serverfd, (struct sockaddr *)&cli_addr, &clilen);
    if (clientfd < 0) {
      perror("Accept failed");
      continue;
    }

    char response[512];

    read(clientfd, buffer, sizeof(buffer) - 1);
    printf("Received request:\n%s\n", buffer);

    snprintf(response, sizeof(response),
             "HTTP/1.1 200 bye\r\n"
             //"Location: https://www.youtube.com/watch?v=dQw4w9WgXcQ\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %d\r\n"
             "\r\n"
             "%s",
             resLength, res);

    write(clientfd, response, strlen(response));

    close(clientfd);
  }

  close(serverfd);

  return 0;
}
