#include "server_header.h"

/*

    programa main responsavel por iniciar todo o sistema do servidor

*/

// check if a string is digit
bool is_number(char *number) {
    for (int i = 0; number[i] != '\0'; ++i) {
        if (!(number[i]>= '0' && number[i] <= '9'))
            return false;
    }
    return true;
}

bool validate_args(int argc, char *argv[]) {

    if (argc != 4) {
        printf("stock_server {PORTO_BOLSA} {PORTO_CONFIG} {ficheiro configuração}\n");
        return false;
    }
    if (!is_number(argv[1]) || !is_number(argv[2])) {
        printf("stock_server {PORTO_BOLSA} {PORTO_CONFIG} {ficheiro configuração}\n");
        return false;
    }

    PORT_BOLSA = atoi(argv[1]);
    PORT       = atoi(argv[2]);

    return true;
}


int main(int argc, char *argv[]) {

    if (!validate_args(argc, argv)) {
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    REFRESH_TIME = 2;
    control = true;
    FILE *file;
    file  = check_file(argv[3]);
    ADMIN admin = read_admin_file(file);

    #ifdef DEBUG
    printf("User: %s / Password: %s\n", admin.name, admin.password);
    #endif

    int number_user = read_number_users(file);

    #ifdef DEBUG
    printf("Number of users: %d\n", number_user);
    #endif

    USER users[MAXUSERS]; users->size = 0;
    read_user_file(file, users, number_user);

    #ifdef DEBUG
    for (int i = 0; i < number_user; ++i) {
        printf("User: %s / Password: %s / Budget: %f\n",users[i].name, users[i].password, users[i].budget);
    }
    #endif

    STOCK_LIST stock[MAXSTOCK];
    init_stock(stock);

    #ifdef DEBUG
    for (int i = 0; i < MAXSTOCK; ++i) {
        if (strlen(stock[i].market) == 0)
            printf("\nHere1\n");
        if (strlen(stock[i].name) == 0)
            printf("Here2\n");
        if (stock[i].price == 0)
            printf("Here3\n");
    }
    #endif

    read_stock_file(file, stock);

    #ifdef DEBUG
        for (int i = 0; i < MAXSTOCK; ++i) {
            if (strlen(stock[i].market) != 0) {
                printf("Market: %s / Name: %s / Price: %f\n", stock[i].market, stock[i].name, stock[i].price);
            }
        }
        printf("%d\n", stock->size);
    #endif

    for (int k = 0; k < users->size; ++k) {
        for (int i = 0; i < MAXSTOCK; ++i) {
            strcpy(users[k].stock[i].name, stock[i].name);
            users[k].stock[i].volume = 0;
        }
    }

    #ifdef DEBUG
        for (int k = 0; k < users->size; ++k) {
            for (int i = 0; i < MAXSTOCK; ++i) {
                printf("User stock: %s, Quantity: %d\n", users[k].stock[i].name, users[k].stock[i].volume);
            }
        }
    #endif

    // server
    ADMIN_SERVER_ARGS argumento_adn;
    argumento_adn.admin = admin;
    argumento_adn.users = users;
    argumento_adn.stock = stock;

    CLIENT_SERVER_ARGS argument_cli;
    argument_cli.users = users;
    argument_cli.stock = stock;

    MULTI multi1;
    multi1.stock = stock;
    strcpy(multi1.market, stock[0].market);
    multi1.aux = 1;

    MULTI multi2;
    multi2.stock = stock;
    strcpy(multi2.market, stock[3].market);
    multi2.aux = 2;

    total_users_loged = 0;

    CHECK_PTHR(pthread_create(&admin_server, NULL, admin_terminal, (void *) &argumento_adn), "Erro a crear thread\n");
    CHECK_PTHR(pthread_create(&market_manager_server, NULL, market_manager, (void *) &stock[0]), "Erro a crear thread\n");
    CHECK_PTHR(pthread_create(&user_interaction_server, NULL, user_interaction, (void *) &argument_cli), "Erro a crear thread\n");
    CHECK_PTHR(pthread_create(&market1, NULL, multiMerc, (void *) &multi1), "Erro a criar thread\n");
    CHECK_PTHR(pthread_create(&market2, NULL, multiMerc, (void *) &multi2), "Erro a criar thread\n");

    CHECK_PTHR(pthread_join(admin_server, NULL), "Erro a esperar pela thread\n");
    CHECK_PTHR(pthread_join(market_manager_server, NULL), "Erro a esperar pela thread\n");
    CHECK_PTHR(pthread_join(market1, NULL), "Erro a esperar pela thread\n");
    CHECK_PTHR(pthread_join(market2, NULL), "Erro a esperar pela thread\n");

    /*if (fork() == 0) {
        execlp("./clientTerminal", "./clientTerminal", "localhost", "8000", NULL);
        exit(EXIT_SUCCESS);
    }*/

    //pthread_kill(user_interaction_server, SIGKILL);
    for (int i = 0; i < total_users; ++i)
        pthread_cancel(id[i]);
    pthread_cancel(user_interaction_server);

    CHECK_PTHR(pthread_join(user_interaction_server, NULL), "Erro a esperar pela thread\n");

    fclose(file);

    for (int i = 0; i < total_users; ++i) {
        close(client_fd[i]);
    }
    close(sock_fd);

    printf("SHUTDOWN\n");
    return 0;
}
