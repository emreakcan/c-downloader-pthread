/*Project : Simple HTTP Client

  
*/

#include "util.h"
int main(int argc, char * argv[]) {
  char url[MAX_STR_LEN]; //="http://popeye.cs.iastate.edu:9798/index.html/";
  char hostname[MAX_STR_LEN];
  int port;
  char identifier[MAX_STR_LEN];

  int sd, rc, i;
  struct sockaddr_in localAddr, servAddr;
  struct hostent * h;

  char * request = 0; //[MAX_STR_LEN];
  char buf[MAX_STR_LEN];

  char line[MAX_MSG];
  char buffer[BUFFER_SIZE];
  int len, size;

  int head_flag = 0, date_flag = 0, default_flag = 0;

  //      printf("\nargc = %d\n", argc);
  request = buf;

  if (1 == argc) {
    printf("\n Usage: client [-h] <URL> \n");
    exit(ERROR);
  }

  for (i = 1; i < argc; i++) {
    if ((strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "-L") == 0)) {
      default_flag = 1;
    }

    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "-H") == 0))
      head_flag = 1;

    else {
      strcpy(url, argv[i]);
    }
  }

  if (default_flag)
    strcpy(url, "http://www.cs.deu.edu.tr:80/");

  // In client.c, you need to parse the <URL> given in the command line, connect to 
  // the server, construct an HTTP request based on the options specified in the 
  // command line, send the HTTP request to server, receive an HTTP response and 
  // display the response on the screen. 
  parse_URL(url, hostname, & port, identifier);

  printf("\n-- Hostname = %s , Port = %d , Identifier = %s\n", hostname,
    port, identifier);

  if (head_flag)
    strcpy(request, "HEAD ");
  else
    strcpy(request, "GET ");

  request = (char * ) strcat(request, identifier);
  request = (char * ) strcat(request, " HTTP/1.1\r\nHOST: ");
  request = (char * ) strcat(request, hostname);
  request = (char * ) strcat(request, "\r\n");

  request = (char * ) strcat(request, "\r\n");

  //      printf("HTTP request =\n%s\nLEN = %d", request, (request));
  //      exit(1);
  //      getch();

  h = gethostbyname(hostname);
  if (h == NULL) {
    printf("unknown host: %s \n ", hostname);
    exit(ERROR);
  }

  servAddr.sin_family = h -> h_addrtype;
  memcpy((char * ) & servAddr.sin_addr.s_addr, h -> h_addr_list[0], h -> h_length);
  servAddr.sin_port = htons(port); //(LOCAL_SERVER_PORT);

  // create socket 
  printf("-- Create socket...    ");
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    perror("cannot open socket ");
    exit(ERROR);
  }

  //  bind port number 
  printf("Bind port number...  ");

  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(0);

  rc = bind(sd, (struct sockaddr * ) & localAddr, sizeof(localAddr));
  if (rc < 0) {
    printf("%s: cannot bind port TCP %u\n", argv[0], port); //(LOCAL_SERVER_PORT);
    perror("error ");
    exit(ERROR);
  }

  // connect to server 
  printf("Connect to server...\n");
  rc = connect(sd, (struct sockaddr * ) & servAddr, sizeof(servAddr));
  if (rc < 0) {
    perror("cannot connect ");
    exit(ERROR);
  }

  // send request
  printf("-- Send HTTP request:\n\n%s", request);
  rc = write(sd, request, strlen(request));
  if (rc < 0) {
    perror("cannot send data ");
    close(sd);
    exit(ERROR);
  }

  // display response
  memset(buffer, 0x0, BUFFER_SIZE); //  init line 
  if ((rc = read(sd, buffer, BUFFER_SIZE)) < 0) {
    perror("read");
    exit(1);
  }
  printf
    ("-- Recieved response:\n\tfrom server: %s, IP = %s,\n\tlength = %d,%lu\n\n%s\n\n",
      url, inet_ntoa(servAddr.sin_addr), rc, strlen(buffer), buffer);

  /*printf ("-- Recieved response:\n\tfrom server: %s, IP = %s,\n\n\n\tlength = %d,%d\n\n",
     url, inet_ntoa (servAddr.sin_addr));
   */
  char tag[100];
  int filesize;
  strcpy(tag, "Content-Length: ");
  sprintf(tag, "%s", strstr(buffer, tag) + strlen(tag));
  filesize = atoi(tag);
  printf("Size: %d\n", filesize);

  size = rc - (strstr(buffer, "\r\n\r\n") - buffer + 4);

  int remaining = filesize - size;
  printf("  %8s  %8s %8s\n", "So far", "bytes", "Remaining");
  if (!head_flag) {
    FILE * fp;
    fp = fopen("out.jpg", "w+b");
    fwrite( & buffer[strstr(buffer, "\r\n\r\n") - buffer + 4], 1, size, fp);
    rc = 0;
    do {
      memset(buffer, 0x0, BUFFER_SIZE); //  init line 
      if (remaining < BUFFER_SIZE)
        rc = read(sd, buffer, remaining);
      else
        rc = read(sd, buffer, BUFFER_SIZE);
      if (rc > 0) {
        //printf ("%s", buffer);
        fwrite(buffer, 1, rc, fp);
        size += rc;
        remaining -= rc;
        printf("  %8d  %8d %8d\n", size, rc, remaining);
      }
      //              printf("  %d  ",size);
    }
    while ((rc > 0) && (remaining > 0));
    fclose(fp);
    printf("\n   Total recieved response bytes: %d\n", size);
  }
  // return
  close(sd);
  return SUCCESS;
}