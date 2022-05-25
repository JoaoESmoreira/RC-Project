#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
  int fd;
  char endServer[100];
  struct sockaddr_in addr;
  struct hostent *hostPtr;

  if (argc != 3) {
    printf("cliente <host> <port>\n");
    exit(-1);
  }

  strcpy(endServer, argv[1]);
  if ((hostPtr = gethostbyname(endServer)) == 0){
    perror("Não consegui obter endereço");
    exit(1);
  }


  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[2]));

  if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
    perror("socket");
    exit(1);
  }

  if (connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0){
    perror("Connect");
    exit(1);
  }

  int nread = 0;
  char buffer[1024];
  char send[50];

  nread = read(fd, buffer, 1024);
  buffer[nread] = '\0';
  while(strcmp("Logged in\n", buffer) != 0){
    printf("%s",buffer);
    scanf("%s", send);
    write(fd, send, sizeof(send));
    nread = read(fd, buffer, 1024);
    buffer[nread] = '\0';
    printf("%s", buffer);
    scanf("%s", send);
    write(fd, send, sizeof(send));
    nread = read(fd, buffer, 1024);
    buffer[nread] = '\0';
  }

  while(1){
    nread = read(fd, buffer, 1024);
    buffer[nread] = '\0';
    printf("%s",buffer);
    scanf("%s", send);
    write(fd, send, sizeof(send));

    if(strcmp(send, "1") == 0){
      //Vai ter de indicar qual o MERcado
      //Depois recebe os dados de ip e passa-o para uma thread que irá ler o multicast
    }
    if(strcmp(send, "0") == 0){
      return;
    }
  }


}
