#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#define PORT 6000

void multiSub(char* group){
  struct sockaddr_in addr;
  int addrlen, s, cnt;
  struct ip_mreq mreq;
  char message[50];

  s = socket(AF_INET, SOCK_DGRAM, 0);
  if(s<0){
    perror("socket");
    exit(1);
  }

  bzero((char*)&addr, sizeof(addr));
  addr.sin_family= AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORT);
  addrlen = sizeof(addr);

  if(bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0){
    perror("bind");
    exit(1);
  }

  int count = 0;
  mreq.imr_multiaddr.s_addr = inet_addr(group);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if(setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
    perror("setsockopt mreq");
    exit(1);
  }
  while(1){
    /*
    if(count == 5){
      printf("A sair\n");
      if(setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
        perror("setsockopt mreq");
        exit(1);
      }
      return 0;
    }*/
    cnt = recvfrom(s, message, sizeof(message), 0, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
    if(cnt < 0){
      perror("recvfrom");
      exit(1);
    }
    else if(cnt == 0){
      break;
    }
    count++;
    //modificar para se adaptar ao que se quer do mercado
    printf("%s: message = \"%s\"\n", inet_ntoa(addr.sin_addr), message);
  }
}
