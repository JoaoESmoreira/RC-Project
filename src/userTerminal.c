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



bool input_option(int *op) {
    char aux;
    
    int res = scanf("%d%c", op, &aux);

    if (res < 1 || (res == 2 && aux != '\n') ) {
        scanf("%*[^\n]%*c");
        printf("Invalid numbers\n");
        return false;
    }
    return true;
}


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


        read(fd, buffer, sizeof(buffer));
    } while(strcmp("Logged in!\n", buffer) != 0);
    printf("%s", buffer);

    // read stock allowed
    read(fd, buffer, sizeof(buffer));
    printf("%s\n", buffer);

    int option = -1;
    while(option != 0){
        read(fd, buffer, 1024);
        printf("%s", buffer);

        while (input_option(&option) != true)
            printf("%s", buffer);
        write(fd, &option, sizeof(option));    

        /*if(strcmp(send, "1") == 0){
            //vai ter de indicar qual o mercado
            //depois recebe os dados de ip e passa-o para uma thread que irá ler o multicast
        }*/

        switch(option){
            case 1:
                printf("1\n");
                break;
            case 2:
                printf("2\n");
                break;
            case 3:
                printf("3\n");
                break;
            case 4:
                printf("4\n");
                break;
            case 5:
                printf("5\n");
                read(fd, buffer, sizeof(buffer));
                printf("%s", buffer);
                break;
            case 0:
                printf("LOGGED OUT\n");
                break;
        }
    }

    close(fd);
    
    return 0;
}