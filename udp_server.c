#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>


// Globals
unsigned short g_usPort;


// Function Prototypes
void parse_args(int argc, char **argv);


// Function Implementations

int main(int argc, char **argv)
{
  parse_args(argc, argv);
  printf("Starting UDP server on port: %hu\n", g_usPort);

  return 0;
}


void parse_args(int argc, char **argv)
{
  if (argc < 2)
  {
    fprintf(stderr,
            "udp_server: missing port operand\nUsage: udp_server PORT\n");
    exit(1);
  }

  errno = 0;
  char *endptr = NULL;
  unsigned long ulPort = strtoul(argv[1], &endptr, 10);

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
            argv[1], strerror(errno));
    abort();
  }
  g_usPort = ulPort;
}
