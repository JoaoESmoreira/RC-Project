#include "server_header.h"


FILE *file;

void clean_resources() {
    #ifdef DEBUG
        printf("\n\nA limpar os recursos...\n");
    #endif
    fclose(file);
}


int main() {	
    srand(time(NULL));
    REFRESH_TIME = 2;
    control = true;
    file  = check_file("configFile.txt");
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
        printf("User: %s / Password: %s / Budget: %d\n",users[i].name, users[i].password, users[i].budget);
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

    // server
    ADMIN_SERVER_ARGS argumento_adn;
    argumento_adn.admin = admin;
    argumento_adn.users = users;
    argumento_adn.stock = stock;

    CLIENT_SERVER_ARGS argument_cli;
    argument_cli.users = users;
    argument_cli.stock = stock;

    total_users_loged = 0;

    CHECK_PTHR(pthread_create(&admin_server, NULL, admin_terminal, (void *) &argumento_adn), "Erro a crear thread\n");
    CHECK_PTHR(pthread_create(&market_manager_server, NULL, market_manager, (void *) &stock[0]), "Erro a crear thread\n");
    CHECK_PTHR(pthread_create(&user_interaction_server, NULL, user_interaction, (void *) &argument_cli), "Erro a crear thread\n");
    CHECK_PTHR(pthread_join(user_interaction_server, NULL), "Erro a esperar pela thread\n");
    CHECK_PTHR(pthread_join(market_manager_server, NULL), "Erro a esperar pela thread\n");
    CHECK_PTHR(pthread_join(admin_server, NULL), "Erro a esperar pela thread\n");

    clean_resources();
    return 0;
}
