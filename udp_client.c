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


// Globals
const char *g_szMessage;
const char *g_szServer;
unsigned short g_usPort;


// Function Prototypes
void parse_args(int argc, char **argv);


// Function Implementations

int main(int argc, char **argv)
{
  parse_args(argc, argv);
  printf("Sending message \"%s\" to %s:%hu\n",
         g_szMessage, g_szServer, g_usPort);

  return 0;
}


void parse_args(int argc, char **argv)
{
  if (argc < 4)
  {
    fprintf(stderr,
            "Usage: udp_client SERVER PORT \"MESSAGE\"\n");
    exit(1);
  }

  errno = 0;
  char *endptr = NULL;
  unsigned long ulPort = strtoul(argv[2], &endptr, 10);

  if (0 == errno)
  {
    // If no other error, check for invalid input and range
    if ('\0' != endptr[0])
      errno = EINVAL;
    else if (ulPort > USHRT_MAX)
      errno = ERANGE;
  }
  if (0 != errno)
  {
    // Report any errors and abort
    fprintf(stderr, "Failed to parse port number \"%s\": %s\n",
            argv[2], strerror(errno));
    abort();
  }
  g_usPort = ulPort;

  g_szServer = argv[1];
  g_szMessage = argv[3];
}
