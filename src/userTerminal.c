#include "client_header.h"

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
    pthread_t market;

    impr = true;
    unsc = false;

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
    char buf[1024];
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

        char stock_buy[MAXLEN], quantity_buy[MAXLEN], price_buy[MAXLEN];

        switch(option){
            case 1:
                printf("1\n");
                read(fd, buf, sizeof(buf));
                printf("%s", buf);
                scanf("%s", send);
                printf("%s\n", send);
                CHECK(write(fd, send, 1), "ERRO A ESCREVER\n");

                read(fd, buf, sizeof(buf));
                if(strcmp(buf, "ACEITE") == 0){
                    //printf("ACEITE: %s\n", buf);
                    read(fd, buf, sizeof(buf));
                    //printf("IP: %s\n", buf);
                    //inicia a thread multicast
                    pthread_create(&market, NULL, multiSub, (void*) buf);
                } else {
                  printf("%s", buf);
                }
                break;
            case 2:
                printf("2\n");

                printf("Introduza a acao a comprar: \n");
                scanf("%s", stock_buy);
                printf("Introduza a quantidade a comprar: \n");
                scanf("%s", quantity_buy);
                printf("Introduza o preço: \n");
                scanf("%s", price_buy);

                CHECK(write(fd, stock_buy, sizeof(stock_buy)), "ERRO A ESCREVER\n");
                CHECK(write(fd, quantity_buy, sizeof(quantity_buy)), "ERRO A ESCREVER\n");
                CHECK(write(fd, price_buy, sizeof(price_buy)), "ERRO A ESCREVER\n");
                read(fd, buf, sizeof(buf));
                printf("%s", buf);

                break;
            case 3:
                printf("3\n");

                printf("Introduza a acao a vender: \n");
                scanf("%s", stock_buy);
                printf("Introduza a quantidade a vender: \n");
                scanf("%s", quantity_buy);
                printf("Introduza o preço: \n");
                scanf("%s", price_buy);

                CHECK(write(fd, stock_buy, sizeof(stock_buy)), "ERRO A ESCREVER\n");
                CHECK(write(fd, quantity_buy, sizeof(quantity_buy)), "ERRO A ESCREVER\n");
                CHECK(write(fd, price_buy, sizeof(price_buy)), "ERRO A ESCREVER\n");
                read(fd, buf, sizeof(buf));
                printf("%s", buf);

                break;
            case 4:
                printf("4\n");
                impr = !impr;
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

    sleep(2);
    close(fd);
    printf("LOGGED OUT\n");
    
    return 0;
}