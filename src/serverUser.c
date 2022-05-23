#include "server_header.h"


static bool check_credentials(USER *users, char *username, char * password) {
    for (int i = 0; i < users->size; ++i) {
        if (strcmp(username, users[i].name) == 10 && strcmp(password, users[i].password) == 10) {
            return true;
        }
        #ifdef DEBUG
            printf("%d - %d\n", strcmp(username, users[i].name), strcmp(password, users[i].password));
        #endif
    }
    return false;
}

void* user(void *args) {
    CLIENT_ARGS *arg = (CLIENT_ARGS *) args;
    int client_fd    = arg->client_fd;
    USER *users      = arg->users;

    char buf[BUFLEN], username[MAXLEN], password[MAXLEN];

    do {
        CHECK(write(client_fd, "Intoduza o seu nick: \n", sizeof("Intoduza o seu nick: \n")), "ERRO A ESCREVER\n");
        CHECK(read(client_fd, username, sizeof(username)), "ERRO A LER\n");
        CHECK(write(client_fd, "Intoduza sua password\n", sizeof("Intoduza sua password\n")), "ERRO A ESCREVER\n");
        CHECK(read(client_fd, password, sizeof(password)), "ERRO A LER\n");

    } while (!check_credentials(users, username, password));
    CHECK(write(client_fd, "Logged in\n", sizeof("Logged in\n")), "ERRO A ESCREVER\n");


    #ifdef DEBUG
        printf("User said: %s - %s\n", username, password);
    #endif
    close(client_fd);
    sleep(2);
    pthread_exit(NULL);
}


void* user_interaction(void *args) {
    USER *users = (USER *) args;


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

        CLIENT_ARGS arg;
        arg.client_fd = client_fd[total_users-1];
        arg.users     = users;

        pthread_create(&id[total_users-1], NULL, user, (void *) &arg);
    }

    for (int i = 0; i < total_users; ++i) {
        pthread_join(id[i], NULL);
    }

    for (int i = 0; i < total_users; ++i) {
        close(client_fd[i]);
    }
    close(sock_fd);

    #ifdef DEBUG
    printf("User server off\n");
    #endif

    pthread_exit(NULL);
}