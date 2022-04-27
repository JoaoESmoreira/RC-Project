#include "server_header.h"

void* market_manager(void *args) {
    STOCK_LIST *stock = (STOCK_LIST *) args;
    int num;

    while (true) {

        for (int i = 0; i < stock->size; ++i) {
            num = rand() % 2;
            if (num == 0 && stock[i].price > 0) {
                stock[i].price -= 0.1;
            } else {
                stock[i].price += 0.1;
            }
        }

        sleep(REFRESH_TIME);
        #ifdef DEBUG
            for (int i = 0; i < MAXSTOCK; ++i) {
                if (strlen(stock[i].market) != 0) {
                    printf("Market: %s / Name: %s / Price: %f\n", stock[i].market, stock[i].name, stock[i].price);
                }
            }
        #endif
    }

    pthread_exit(NULL);
}