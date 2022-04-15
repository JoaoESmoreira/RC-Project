#include "biblio.h"

int main() {
    REFRESH_TIME = 10;
    FILE *file  = check_file("configFile.txt");
    ADMIN admin = read_admin_file(file);

    #ifdef DEBUG
    printf("User: %s / Password: %s\n", admin.name, admin.password);
    #endif

    int number_user = read_number_users(file);

    #ifdef DEBUG
    printf("Number of users: %d\n", number_user);
    #endif

    USER users[MAXUSERS]; users->pos = 0;
    read_user_file(file, users, number_user);

    #ifdef DEBUG
    for (int i = 0; i < number_user; ++i) {
        printf("User: %s / Password: %s / Budget: %d\n",users[i].name, users[i].password, users[i].budget);
    }
    #endif

    STOCK_LIST stock[MAXSTOCK];
    init(stock);

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
                printf("Market: %s / Name: %s / Price: %d\n", stock[i].market, stock[i].name, stock[i].price);
            }
        }
    #endif

    // server
    admin_usage(admin, users);

    fclose(file);
    return 0;
}
