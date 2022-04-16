#include "server_header.h"

FILE* check_file(const char *file_name) {
    if (strcmp(file_name, "configFile.txt") != 0) {
        printf("Ficheiro invalido\n");
        exit(EXIT_FAILURE);
    }
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Erro a abrir o ficheiro\n");
        exit(EXIT_FAILURE);
    }
    return file;
}

ADMIN read_admin_file(FILE *file) {
    ADMIN admin;
    char  end_char, end_line = '\n', end_Uname = '/';
    int   num_events;

    num_events = fscanf(file, "%50[^/]%c", admin.name, &end_char);
    READING(num_events, end_char, end_Uname, "ERRO NA PRIMEIRA LINHA\n");

    num_events = fscanf(file, "%50[^\n]%c", admin.password, &end_char);
    READING(num_events, end_char, end_line, "ERRO NA PRIMEIRA LINHA\n");

    return admin;
}

int read_number_users (FILE *file) {
    int  total, num_events;
    char end_char, end_line = '\n';

    num_events = fscanf(file, "%d%c", &total, &end_char);
    READING(num_events, end_char, end_line, "ERRO NA LINHA 2\n");

    if (total > MAXUSERS || total < 2) {
        printf("Numero de usuarios invalidos\n");
        exit(EXIT_FAILURE);
    }

    return total;
}

void read_user_file (FILE *file, USER *users, int max_users) {

    int  num_events;
    char end_char, end_line = '\n', sparator = ';';


    for (int i = 0; i < max_users; ++i) {

        num_events = fscanf(file, "%50[^;]%c", users[i].name, &end_char);
        READING(num_events, end_char, sparator, "ERRO NO %dº USUARIO\n", i + 1);

        num_events = fscanf(file, "%50[^;]%c", users[i].password, &end_char);
        READING(num_events, end_char, sparator, "ERRO NO %dº USUARIO\n", i + 1);

        num_events = fscanf(file, "%d%c", &users[i].budget, &end_char);
        READING(num_events, end_line, sparator, "ERRO NO %dº USUARIO\n", i + 1);

        strcpy(users[i].markets[0], "-");
        strcpy(users[i].markets[1], "-");
        users->pos++;
    }
}

void read_stock_file (FILE *file, STOCK_LIST *stock) {
    int  num_events;
    char end_char, end_line = '\n', sparator = ';';

    for (int i = 0; i < MAXSTOCK && !feof(file); ++i) {

        num_events = fscanf(file, "%50[^;]%c", stock[i].market, &end_char);
        READING(num_events, end_char, sparator, "ERRO NA %dº ACAO\n", i + 1);

        //count = 0;
        /*for (int j = 0; j < i; ++j) {
            if (strcmp(stock[i].market, stock[j].market) != 0)
                count++;
        }
        if (count > 3) {
            printf("MERCADOS A MAIS\n");
            exit(EXIT_FAILURE);
        }*/

        num_events = fscanf(file, "%50[^;]%c", stock[i].name, &end_char);
        READING(num_events, end_char, sparator, "ERRO NA %dº ACAO\n", i + 1);

        num_events = fscanf(file, "%d%c", &stock[i].price, &end_char);
        READING(num_events, end_line, sparator, "ERRO NA %dº ACAO\n", i + 1);
    }
}

void init(STOCK_LIST *stock) {
    for (int i = 0; i < MAXSTOCK; ++i) {
        *stock[i].market = '\0';
        *stock[i].name   = '\0';
        stock[i].price   = 0;
    }
}
