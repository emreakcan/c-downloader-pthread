/*Project : Simple HTTP Client

  
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOCAL_SERVER_PORT 9798
#define MAX_MSG 100
#define END_LINE 0x0A

#define SUCCESS 0
#define ERROR   1

#define MAX_STR_LEN 512
#define BUFFER_SIZE 20000

/*---------------------------------------------------------------------------*
 * 
 * Parses an url into hostname, port number and resource identifier.
 *
 *---------------------------------------------------------------------------*/
void
parse_URL (char *url, char *hostname, int *port, char *identifier)
{
  char protocol[MAX_STR_LEN], scratch[MAX_STR_LEN], *ptr = 0, *nptr = 0;

  strcpy (scratch, url);
  ptr = (char *) strchr (scratch, ':');
  if (!ptr)
    {
      fprintf (stderr, "Wrong url: no protocol specified\n");
      exit (ERROR);
    }
  strcpy (ptr, "\0");
  strcpy (protocol, scratch);
  /* if (strcmp (protocol, "http"))
     {
     fprintf (stderr, "Wrong protocol: %s\n", protocol);
     exit (ERROR);
     }
   */
  strcpy (scratch, url);
  ptr = (char *) strstr (scratch, "//");
  if (!ptr)
    {
      fprintf (stderr, "Wrong url: no server specified\n");
      exit (ERROR);
    }
  ptr += 2;

  strcpy (hostname, ptr);
  nptr = (char *) strchr (ptr, ':');
  if (!nptr)
    {
      *port = 80;		/* use the default HTTP port number */
      nptr = (char *) strchr (hostname, '/');
    }
  else
    {
      sscanf (nptr, ":%d", port);
      nptr = (char *) strchr (hostname, ':');
    }

  if (nptr)
    *nptr = '\0';

  nptr = (char *) strchr (ptr, '/');

  if (!nptr)
    {
      fprintf (stderr, "Wrong url: no file specified\n");
      exit (ERROR);
    }

  strcpy (identifier, nptr);

}


