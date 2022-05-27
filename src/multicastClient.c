#include "client_header.h"


void* multiSub(void* args){
  char* group = (char*) args;
  struct sockaddr_in addr;
  int addrlen, s, cnt;
  struct ip_mreq mreq;
  char message[1024];

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


  mreq.imr_multiaddr.s_addr = inet_addr(group);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if(setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
    perror("setsockopt mreq");
    exit(1);
  }
  while(1){
    //printf("ENTREI NO CICLO\n");
    if(unsc){
      printf("A sair do multicast\n");
      if(setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
        perror("setsockopt mreq");
        exit(1);
      }
      close(s);
      return 0;
    }
    //printf("A ESPERA DO MULTICAST, %s PORT %d\n", group, PORT);
    cnt = recvfrom(s, message, sizeof(message), 0, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
    if(cnt < 0){
      perror("recvfrom");
      exit(1);
    }
    else if(cnt == 0){
      break;
    }
    //printf("RECEBI DO multicast\n");
    //modificar para se adaptar ao que se quer do mercado
    if(impr){
      printf("%s", message);
    }
  }
  pthread_exit(NULL);
}