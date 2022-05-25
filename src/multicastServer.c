#include "server_header.h"

// 239.0.0.1 - market 1
// 239.0.0.2 - market 2
// Ao iniciar o servidor chamar a função para o mercado 1 e para o mercado 2
#define PORT 6000


void multiMerc(int num){
  char group[MAXLEN]
  if(num == 1){
    strcpy(group, "239.0.0.1");
  }
  else{
    strcpy(group, "239.0.0.2");
  }

  struct sockaddr_in addr;
  int addrlen, s, cnt;
  char message[50];

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
  addr.sin_port = htons(PORT);
  addrlen = sizeof(addr);

  addr.sin_addr.s_addr = inet_addr(group);
  while(1){
    //ENVIAR OS UPDATES QUE ACONTECEM NO MERcado
  }
}
