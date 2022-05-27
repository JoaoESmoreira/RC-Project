
#include "server_header.h"

// 239.0.0.1 - market 1
// 239.0.0.2 - market 2
// Ao iniciar o servidor chamar a função para o mercado 1 e para o mercado 2
#define PORTM 6000


void* multiMerc(void* args){
  char group[MAXLEN];
  char market[MAXLEN];
  MULTI mult = *((MULTI*) args);
  strcpy(market, mult.market);

  if(mult.aux == 1){
    strcpy(group, "239.0.0.1");
  }
  else{
    strcpy(group, "239.0.0.2");
  }

  struct sockaddr_in addr;
  int addrlen, s, cnt;
  char message[124];

  s = socket(AF_INET, SOCK_DGRAM, 0);
  if(s<0){
    perror("socket");
    exit(1);
  }

  int multicastTTL = 255;
  if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&multicastTTL, sizeof(multicastTTL)) < 0){
    perror("socket opt");
    exit(1);
  }

  bzero((char*)&addr, sizeof(addr));
  addr.sin_family= AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORTM);
  addrlen = sizeof(addr);

  addr.sin_addr.s_addr = inet_addr(group);
  //printf("VIM PARA MANDAR\n");
  while(1){
    //printf("VOU MADAR PARA %s PORTO %d\n", group, PORTM);
    if (!control)
      break;

    for(int i=0; i< mult.stock->size; ++i){
      if(strcmp(mult.stock[i].market, market) == 0){
        sprintf(message, "Market: %s / Name: %s / Price: %f / volume: %d\n", mult.stock[i].market, mult.stock[i].name, mult.stock[i].price, mult.stock[i].volume);
        cnt = sendto(s, message, sizeof(message), 0, (struct sockaddr *)&addr, addrlen);
        if(cnt < 0){
          perror("sendto");
          exit(1);
        }
      }
    }
    sleep(REFRESH_TIME);
  }

  printf("MulticastServer off\n");
  pthread_exit(NULL);
}