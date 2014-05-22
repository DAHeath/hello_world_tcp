#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

typedef struct arguments {
  const char *message;
  const unsigned int messageLength;
  const char *serverIp;
  const unsigned short port;
} Args;

typedef struct sockaddr_in SocketAddress;

// Function Prototypes
const Args parse_args(int argc, char **argv);
const SocketAddress getServerAddress(const Args args);


// Function Implementations

int main(int argc, char **argv) {
  const Args args = parse_args(argc, argv);
  printf("Sending message \"%s\" to %s:%hu\n", args.message, args.serverIp, args.port);
  const SocketAddress addr = getServerAddress(args);
  int sock;
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("socket() failed");
    exit(1);
  }
  connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  printf("Connection made!\n");
  send(sock, args.message, args.messageLength, 0);
  close(sock);
  return 0;
}


const Args parse_args(int argc, char **argv) {
  if (argc < 4) {
    fprintf(stderr, "Usage: tcp_client SERVER PORT \"MESSAGE\"\n");
    exit(1);
  }

  errno = 0;
  char *endptr = NULL;
  unsigned long ulPort = strtoul(argv[2], &endptr, 10);

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
            argv[2], strerror(errno));
    abort();
  }
  return (Args){argv[3], strlen(argv[3]), argv[1], ulPort};
}

const SocketAddress getServerAddress(Args args) {
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  inet_aton(args.serverIp, &serverAddress.sin_addr);
  serverAddress.sin_port = htons(args.port);
  return serverAddress;
}

