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
#include <arpa/inet.h>


#define CHECK(X, ...) if (X == -1) { printf(__VA_ARGS__); exit(EXIT_FAILURE); }


int main(int argc, char *argv[]) {
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
        perror("não consegui obter endereço");
        exit(1);
    }   
    CHECK((fd = socket(AF_INET, SOCK_STREAM, 0)), "socket");
    bzero((void *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr.sin_port = htons((short) atoi(argv[2]));   
    CHECK(connect(fd,(struct sockaddr *)&addr,sizeof (addr)), "connect");


    char buffer[1024];
    char send[50];  

    read(fd, buffer, sizeof(buffer));
    do {
        // receber o close    
        printf("%s", buffer);
        scanf("%s", send);
        write(fd, send, strlen(send));

        read(fd, buffer, sizeof(buffer));
        printf("%s", buffer);
        scanf("%s", send);
        write(fd, send, strlen(send));


        read(fd, buffer, 1024);
    } while(strcmp("Logged in!\n", buffer) != 0);
    printf("%s", buffer);

    // read stock allowed    
    read(fd, buffer, sizeof(buffer));
    printf("here: %s\n", buffer);

    /*while(1){
        read(fd, buffer, 1024);
        printf("%s",buffer);
        scanf("%s", send);
        write(fd, send, sizeof(send));    
        if(strcmp(send, "1") == 0){
            //vai ter de indicar qual o mercado
            //depois recebe os dados de ip e passa-o para uma thread que irá ler o multicast
        }
        if(strcmp(send, "0") == 0){
            return 0;
        }
    }*/

    close(fd);
    printf("HERE\n");
    
    return 0;
}