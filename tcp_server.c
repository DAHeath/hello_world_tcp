#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

typedef struct sockaddr_in SocketAddress;

const unsigned short QUEUE_SIZE = 10;
const unsigned short MAX_MESSAGE_SIZE = 255;

// Function Prototypes
const unsigned short parse_args(int argc, char **argv);
const SocketAddress getSocketAddress(const unsigned short port);
int setupServer(SocketAddress serverAddr);
void handleClient(int sock);
void printMessageOfLength(char* message, int length);

// Function Implementations

void exitWithMessage(const char* message) {
  printf("%s\n", message);
  exit(1);
}

int main(int argc, char **argv) {
  unsigned int port = parse_args(argc, argv);
  int serverSock = setupServer(getSocketAddress(port));

  SocketAddress clientAddr;
  int clientSock;
  while(1) {
    unsigned int addrLen = sizeof(clientAddr);
    if ((clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &addrLen)) < 0)
      exitWithMessage("accept() failed");
    printf("Connection made!\n");
    handleClient(clientSock);
  }
  return 0;
}

const unsigned short parse_args(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr,
            "tcp_server: missing port operand\nUsage: tcp_server PORT\n");
    exit(1);
  }

  errno = 0;
  char *endptr = NULL;
  unsigned long ulPort = strtoul(argv[1], &endptr, 10);

  if (0 == errno) {
    // If no other error, check for invalid input and range
    if ('\0' != endptr[0])
      errno = EINVAL;
    else if (ulPort > USHRT_MAX)
      errno = ERANGE;
  }
  if (0 != errno) {
    // Report any errors and abort
    fprintf(stderr, "Failed to parse port number \"%s\": %s\n",
            argv[1], strerror(errno));
    abort();
  }
  printf("Starting TCP server on port: %hu\n", (unsigned int)ulPort);
  return ulPort;
}

const SocketAddress getSocketAddress(const unsigned short port) {
  SocketAddress addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  return addr;
}

int setupServer(SocketAddress serverAddr) {
  int serverSock;
  if ((serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    exitWithMessage("socket() failed");
  if ((bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) < 0)
    exitWithMessage("bind() failed");
  if (listen(serverSock, QUEUE_SIZE) < 0)
    exitWithMessage("listen() failed");
  return serverSock;
}

void handleClient(int socket) {
  char buffer[MAX_MESSAGE_SIZE+1];
  int length = 1;
  if((length = recv(socket, buffer, MAX_MESSAGE_SIZE, 0)) < 0)
    exitWithMessage("recv() failed");
  while (length > 0) {
    buffer[length] = '\0';
    printf("%s", buffer);
    if((length = recv(socket, buffer, MAX_MESSAGE_SIZE, 0)) < 0)
      exitWithMessage("recv() failed");
  }
  printf("\n");
  close(socket);
}
