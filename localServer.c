#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAINFILE "index.html"
#define ABOUTFILE "about.html"
#define MISSING "404 not found"

char *readFile(const char *filename) {

  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("file not found");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  char *content = (char *)malloc(file_size + 1);
  if (!content) {
    perror("Memory error");
    fclose(file);
    return NULL;
  }

  fread(content, 1, file_size, file);
  content[file_size] = '\0';

  fclose(file);
  return content;
}

struct thread_data {
  int client;
  char *response;
  int response_length;
};

void *handle_client(void *arg) {
  struct thread_data *data = (struct thread_data *)arg;
  char buffer[1024];
  char response[1024];

  read(data->client, buffer, sizeof(buffer) - 1);
  printf("Received request:\n%s\n", buffer);

  snprintf(response, sizeof(response),
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: %d\r\n"
           "\r\n"
           "%s",
           data->response_length, data->response);

  write(data->client, response, strlen(response));
  close(data->client);
  free(data);

  return NULL;
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    perror("Wrong number of inputs\n");
    return -1;
  }

  int serverfd, clientfd, portn;
  socklen_t clilen;
  portn = atoi(argv[1]);
  char *address = "192.168.178.25";
  const char *res = readFile(MAINFILE);
  int resLength = strlen(res);
  struct sockaddr_in server_addr, cli_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portn);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  serverfd = socket(AF_INET, SOCK_STREAM, 0);
  if (serverfd < 0) {
    perror("Problem creating socket\n");
    return -1;
  }

  if (bind(serverfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Problem binding\n");
    return -1;
  }

  listen(serverfd, 5);

  printf("Server listening on port %d...\n", portn);

  while (1) {
    clilen = sizeof(cli_addr);
    clientfd = accept(serverfd, (struct sockaddr *)&cli_addr, &clilen);
    if (clientfd < 0) {
      perror("Accept failed");
      continue;
    }

    struct thread_data *data =
        (struct thread_data *)malloc(sizeof(struct thread_data));
    if (data == NULL) {
      perror("Failed to allocate memory for thread data");
      close(clientfd);
      continue;
    }

    data->client = clientfd;
    data->response = (char *)res;
    data->response_length = resLength;

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, handle_client, (void *)data) != 0) {
      perror("Failed to create thread");
      close(clientfd);
      free(data);
      continue;
    }

    pthread_detach(thread_id);
  }

  close(serverfd);

  free((char *)res);

  return 0;
}
