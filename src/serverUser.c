#include "server_header.h"


static void list_stock_avaible(STOCK_LIST *stock, USER * users, const char *username, char *dest) {
    bool flag = false;
    char string[1024];
    char aux[200];
    sprintf(string, "Stocks inscritos: ");


    for (int i = 0; i < users->size; ++i) {
        if (strcmp(username, users[i].name) == 10) {
            for (int j = 0; j < stock->size; ++j) {
                if (strcmp(users[i].markets[0], stock[j].name) == 0 || strcmp(users[i].markets[1], stock[j].name) == 0) {

                    strcat(string, stock[j].name);
                    sprintf(aux, " Preço: %f | ", stock[i].price);
                    strcat(string, aux);

                    flag = true;
                }
            }
            break;
        }
    }
    if (!flag)
        strcat(string, "Sem bolsas");
    char ch = '\n';
    strncat(string, &ch, 1);
    printf("%s", string);

    for (int i = 0; string[i] != '\0'; ++i) {
        dest[i] = string[i];
    }
}

// check if the credentials it's ok
static bool check_credentials(USER *users, char *username, char * password) {
    for (int i = 0; i < users->size; ++i) {
        if (strcmp(username, users[i].name) == 10 && strcmp(password, users[i].password) == 10) {
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
    total_users_loged = total_users_loged + 1;

    if (total_users_loged < 6) {
        char username[MAXLEN], password[MAXLEN];

        do {
            CHECK(write(client_fd, "Intoduza o seu nick: \n", sizeof("Intoduza o seu nick: \n")), "ERRO A ESCREVER\n");
            CHECK(read(client_fd, username, sizeof(username)), "ERRO A LER\n");
            CHECK(write(client_fd, "Intoduza sua password\n", sizeof("Intoduza sua password\n")), "ERRO A ESCREVER\n");
            CHECK(read(client_fd, password, sizeof(password)), "ERRO A LER\n");

        } while (!check_credentials(users, username, password));
        CHECK(write(client_fd, "Logged in\n", sizeof("Logged in\n")), "ERRO A ESCREVER\n");

        char string[BUFLEN];
        list_stock_avaible(stock, users, username, string);
        CHECK(write(client_fd, string, strlen(string)), "ERRO A ESCREVER\n");

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