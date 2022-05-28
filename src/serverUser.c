#include "server_header.h"

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


USER* getUser(USER *users, const char *username) {
    for (int i = 0; i < users->size; ++i) {
        if (strcmp(username, users[i].name) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

void list_stock_wallet(USER *User, char *dest) {
    char string[BUFLEN];
    string[0] = '\0';
    char aux[200];

    for (int i = 0; i < MAXSTOCK; ++i) {
        sprintf(aux, "Nome stock: ");
        strcat(string, aux);
        strcat(string, User->stock[i].name);
        sprintf(aux, "; Stock: %d\n", User->stock[i].volume);
        strcat(string, aux);
    }
    sprintf(aux, "Saldo: %f\n", User->budget);
    strcat(string, aux);

    // copy to dest
    for (int i = 0; string[i] != '\0'; ++i) {
        dest[i] = string[i];
    }
}

static void list_stock_avaible(STOCK_LIST *stock, USER *User, char *dest) {
    bool flag = false;
    char string[1024];
    char aux[200];
    sprintf(string, "Stocks inscritos: ");

    for (int j = 0; j < stock->size; ++j) {
        //printf("%d - %d", strcmp(User->markets[0], stock[j].name) == 0, strcmp(User->markets[1], stock[j].name) == 0);
        if (strcmp(User->markets[0], stock[j].market) == 0 || strcmp(User->markets[1], stock[j].market) == 0) {
            strcat(string, stock[j].name);
            sprintf(aux, " Preço: %f | ", stock[j].price);
            strcat(string, aux);
            flag = true;
        }
    }

    if (!flag)
        strcat(string, "Sem bolsas");
    char ch = '\n';
    strncat(string, &ch, 1);

    // copy to dest
    for (int i = 0; string[i] != '\0'; ++i) {
        dest[i] = string[i];
    }
}

// check if the credentials it's ok
static bool check_credentials(USER *users, char *username, char * password) {
    for (int i = 0; i < users->size; ++i) {
        if (strcmp(username, users[i].name) == 0 && strcmp(password, users[i].password) == 0) {
            return true;
        }
    }
    return false;
}

static int check_stock(STOCK_LIST *stock, const USER *user, const char *name_stock) {

    for (int i = 0; i < MAXSTOCK; ++i) {
        if (strcmp(stock[i].name, name_stock) == 0) {
            if(strcmp(stock[i].market, user->markets[0]) == 0 || strcmp(stock[i].market, user->markets[1]) == 0) {
                printf("DEU CERTO\n\n\n");
                return i;
            }
        }
    }
    return -1;
}

static bool buy_auctions(STOCK_LIST *stock, USER *user, const char *name_stock, const char *quantity_buy, const char *price_buy) {
    int   quantity, pos;
    float price;

    if ((pos = check_stock(stock, user, name_stock)) == -1 || (price = strtof(price_buy, NULL)) == 0 || (quantity = atoi(quantity_buy)) == 0 )
        return false;

    quantity = quantity - quantity % 10;
    if (stock[pos].volume - quantity < 0 || user->budget - price < 0 || stock[pos].price + 0.02 > price)
        return false;

    user->budget            -= price;
    stock[pos].volume       -= quantity;
    user->stock[pos].volume += quantity;
     
    return true;
}

static bool sell_auctions(STOCK_LIST *stock, USER *user, const char *name_stock, const char *quantity_buy, const char *price_buy) {
    int   quantity, pos;
    float price;

    if ((pos = check_stock(stock, user, name_stock)) == -1 || (price = strtof(price_buy, NULL)) == 0 || (quantity = atoi(quantity_buy)) == 0 )
        return false;

    printf("HERE\n");

    quantity = quantity - quantity % 10;
    if (user->stock[pos].volume - quantity < 0 || stock[pos].price < price + 0.02)
        return false;
    printf("HERE\n");

    user->budget            += price;
    stock[pos].volume       += quantity;
    user->stock[pos].volume -= quantity;
     
    return true;
}

static bool verify_permission(USER *User,  STOCK_LIST *stock, int market){
  if(market == 1){
    if(strcmp(stock[0].market, User->markets[0]) == 0){
      return true;
    }
  }
  else if(market == 2){
    if(strcmp(stock[3].market, User->markets[1]) == 0){
      return true;
    }
  }
  return false;
}


// all interaction between server ao client
void* user(void *args) {
    USER_ARGS *arg    = (USER_ARGS *) args;
    int client_fd     = arg->client_fd;
    USER *users       = arg->users;
    STOCK_LIST *stock = arg->stock;

    if (total_users_loged < 6) {
        total_users_loged = total_users_loged + 1;
        char username[MAXLEN], password[MAXLEN];
        int option = -1;

        do {
            CHECK(write(client_fd, "Intoduza o seu nick: \n", sizeof("Intoduza o seu nick: \n")), "ERRO A ESCREVER\n");
            CHECK(read(client_fd, username, sizeof(username)), "ERRO A LER\n");
            CHECK(write(client_fd, "Intoduza sua password: \n", sizeof("Intoduza sua password: \n")), "ERRO A ESCREVER\n");
            CHECK(read(client_fd, password, sizeof(password)), "ERRO A LER\n");

        } while (!check_credentials(users, username, password));
        CHECK(write(client_fd, "Logged in!\n", sizeof("Logged in!\n")), "ERRO A ESCREVER\n");

        USER *User = getUser(users, username);


        char string[BUFLEN];
        list_stock_avaible(stock, User, string);
        sleep(1);
        CHECK(write(client_fd, string, sizeof(string)), "ERRO A ESCREVER\n");

        while(option != 0){
            CHECK(write(client_fd, "Menu:\n1) Subscrever um mercado.\n2) Comprar.\n3) Vender.\n4) Ligar ou desligar o feed.\n5) Conteudo da carteira e saldo.\n0) Sair\n", sizeof("Menu:\n1) Subscrever um mercado.\n2) Comprar.\n3) Vender.\n4) Ligar ou desligar o feed.\n5) Conteudo da carteira e saldo.\n0) Sair\n")), "ERRO A ESCREVER\n");
            CHECK(read(client_fd, &option, sizeof(option)), "ERRO A LER\n");

            bzero(string, sizeof(string));
            
            char stock_buy[MAXLEN], quantity_buy[MAXLEN], price_buy[MAXLEN];

            //switch de opções
            switch(option){
                case 1:
                    printf("VIM AO 1\n");
                    int option2;
                    sprintf(string, "Mercados:\n1)%s\n2)%s\n", stock[0].market, stock[3].market);
                    CHECK(write(client_fd, string, sizeof(string)), "ERRO A ESCREVER\n");
                    CHECK(read(client_fd, &option2, sizeof(option2)), "ERRO A LER\n");

                    printf("RECEBIDO: %d\n", option2);
                    //verify_permission(User,  stock, option2)
                    if(true){
                      CHECK(write(client_fd, "ACEITE", sizeof("ACEITE")), "ERRO A ESCREVER\n");
                      sleep(1);
                      printf("%d\n", option2);
                      if(option2 == 1){
                        CHECK(write(client_fd, "239.0.0.1", sizeof("239.0.0.1")), "ERRO A ESCREVER\n");
                      }
                      else{
                        CHECK(write(client_fd, "239.0.0.2", sizeof("239.0.0.2")), "ERRO A ESCREVER\n");
                      }
                    }
                    else{
                      CHECK(write(client_fd, "Nao possui acesso a este mercado.\n", sizeof("Nao possui acesso a este mercado.\n")), "ERRO A ESCREVER\n");

                    }
                    sleep(1);
                    break;
                case 2:
                    printf("2\n");

                    CHECK(read(client_fd, stock_buy, sizeof(stock_buy)), "ERRO A ESCREVER\n");
                    CHECK(read(client_fd, quantity_buy, sizeof(quantity_buy)), "ERRO A ESCREVER\n");
                    CHECK(read(client_fd, price_buy, sizeof(price_buy)), "ERRO A ESCREVER\n");

                    printf("%s,%s,%s\n", stock_buy, quantity_buy, price_buy);

                    //buy_auctions(stock, User, stock_buy, quantity_buy, price_buy);
                    if (buy_auctions(stock, User, stock_buy, quantity_buy, price_buy)) {
                        CHECK(write(client_fd, "Acao comprada\n", sizeof("Acao comprada\n")), "ERRO A ESCREVER\n");
                    } else {
                        CHECK(write(client_fd, "Acao nao comprada\n", sizeof("Acao nao comprada\n")), "ERRO A ESCREVER\n");
                    }
                    sleep(1);

                    break;
                case 3:
                    printf("3\n");
                    
                    CHECK(read(client_fd, stock_buy, sizeof(stock_buy)), "ERRO A ESCREVER\n");
                    CHECK(read(client_fd, quantity_buy, sizeof(quantity_buy)), "ERRO A ESCREVER\n");
                    CHECK(read(client_fd, price_buy, sizeof(price_buy)), "ERRO A ESCREVER\n");

                    printf("%s,%s,%s\n", stock_buy, quantity_buy, price_buy);
                    
                    if (sell_auctions(stock, User, stock_buy, quantity_buy, price_buy)) {
                        CHECK(write(client_fd, "Acao vendida\n", sizeof("Acao vendida\n")), "ERRO A ESCREVER\n");
                    } else {
                        CHECK(write(client_fd, "Acao nao vendida\n", sizeof("Acao nao vendida\n")), "ERRO A ESCREVER\n");
                    }
                    sleep(1);

                    break;
                case 4:
                    printf("4\n");
                    break;
                case 5:
                    printf("5\n");
                    list_stock_wallet(User, string);
                    printf("%s", string);
                    CHECK(write(client_fd, string, sizeof(string)), "ERRO A ESCREVER\n");
                    break;
                case 0:
                    printf("LOGGED OUT\n");
                    break;
                /*default:
                    CHECK(write(client_fd, "Opcao introduzida inválida.", sizeof("Opcao introduzida inválida.")),"ERRO A ESCREVER\n");*/
            }
        }
        total_users_loged--;
    } else {

        CHECK(write(client_fd, "Maximo de utilizadores atingido\n", strlen("Maximo de utilizadores atingido\n")), "ERRO A ESCREVER\n");
    }

    close(client_fd);
    sleep(2);
    pthread_exit(NULL);
}

// accept the clients and send them to the user function
void* user_interaction(void *args) {
    CLIENT_SERVER_ARGS arg = *((CLIENT_SERVER_ARGS *) args);
    STOCK_LIST *stock      = arg.stock;
    USER       *users      = arg.users;


    SOCKADDRIN server_addr, client_addr;
    int sock_fd, client_fd[MAXUSERS], len_addr = sizeof(client_addr), total_users = 0;
    pthread_t id[MAXUSERS];
    total_users_loged = 0;

    CHECK((sock_fd = socket(AF_INET, SOCK_STREAM, 0)), "ERRO A CRIAR SOCKET TCP\n");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT_BOLSA);

    CHECK(bind(sock_fd, (SOCKADDR *) &server_addr, sizeof(server_addr)), "ERRO NO BIND\n");
    CHECK(listen(sock_fd, 5), "ERRO NO LISTEN\n");

    
    while (control) {
        if (total_users < MAXUSERS) {
            CHECK((client_fd[total_users++] = accept(sock_fd, (SOCKADDR *) &client_addr, (socklen_t *) &len_addr)), "ERRO NO ACCEPT\n");
        }
        if (!control)
        // enviar um close
            break;

        USER_ARGS arg;
        arg.client_fd = client_fd[total_users-1];
        arg.users     = users;
        arg.stock     = stock;

        pthread_create(&id[total_users-1], NULL, user, (void *) &arg);
    }

    // wait for all threads to dead
    for (int i = 0; i < total_users; ++i) {
        pthread_join(id[i], NULL);
    }

    // clean files
    for (int i = 0; i < total_users; ++i) {
        close(client_fd[i]);
    }
    close(sock_fd);

    #ifdef DEBUG
    printf("User server off\n");
    #endif

    pthread_exit(NULL);
}