#include "server_header.h"


void* user_interaction() {
    SOCKADDRIN server_addr, client_addr;
    int sock_fd, client_fd[MAXUSERS], len_addr = sizeof(client_addr), total_users = 0;

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
    }

    for (int i = 0; i < total_users; ++i) {
        close(client_fd[i]);
    }
    close(sock_fd);
    pthread_exit(0);
}