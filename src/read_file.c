#include "server_header.h"

// check if the file has the correct name and open it
// if everything it's ok return a file pointer
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

// read the credentials of admin and return it stored in a ADMIN type
ADMIN read_admin_file(FILE *file) {
    ADMIN admin;
    char  end_char, end_line = '\n', end_Uname = '/';
    int   num_events;

    num_events = fscanf(file, "%50[^/]%c", admin.name, &end_char);
    READING(num_events, end_char, end_Uname, "ERRO A LER ADMIN\n");

    num_events = fscanf(file, "%50[^\n]%c", admin.password, &end_char);
    READING(num_events, end_char, end_line, "ERRO A LER ADMIN\n");

    return admin;
}

// read the number of user and return it
int read_number_users (FILE *file) {
    int  total, num_events;
    char end_char, end_line = '\n';

    num_events = fscanf(file, "%d%c", &total, &end_char);
    READING(num_events, end_char, end_line, "ERRO A LER USERS\n");

    if (total > MAXUSERS || total < 2) {
        printf("Numero de usuarios invalidos\n");
        exit(EXIT_FAILURE);
    }

    return total;
}

// read all users atributes and store them in a USER struct
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
        users->size++;
    }
}

// check if a string is int the array
// return true in affirmative case
static bool in_array(const char *string, int count, char str[6][MAXLEN]) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(str[i], string) == 0) {
            return true;
        }
    }
    return false;
}

// check if there are more than 2 markets
// finish all program in affirmative case
static void check_number_markets (const STOCK_LIST *stock) {
    int count = 0;
    char str[6][MAXLEN];

    for (int i = 0; i < stock->size; ++i) {
        if (!in_array(stock[i].market, count, str)) {
            strcpy(str[count], stock[i].market);
            count++;
        }
    }
    #ifdef DEBUG
    for (int i = 0; i < count; ++i) {
        printf("Check_number_markets: %s - %d\n", str[i], count);
    }
    #endif

    if (count > 2) {
        printf("MERCADOS A MAIS %d\n", count);
        exit(EXIT_FAILURE);
    }
}

// check if there are more than 3 stock per market
// finish all program in affirmative case
static void check_number_stock (const STOCK_LIST *stock) {
    int  total_m1 = 0, total_m2 = 0, max = 0;
    char markets[2][MAXLEN], stock1[6][MAXLEN], stock2[6][MAXLEN];

    for (int i = 0; i < stock->size; ++i) {
        if (!in_array(stock[i].market, max, markets)) {
            strcpy(markets[max], stock[i].market);
            max++;
        }
    }

    for (int i = 0; i < stock->size; ++i) {
        if (strcmp(markets[0], stock[i].market) == 0) {
            if (!in_array(stock[i].name, total_m1, stock1)) {
                strcpy(stock1[total_m1], stock[i].name);
                total_m1++;
            }
        }
        
        if (strcmp(markets[1], stock[i].market) == 0) {
            if (!in_array(stock[i].name, total_m1, stock1)) {
                strcpy(stock2[total_m2], stock[i].name);
                total_m2++;
            }
        }
    }
    if (total_m1 > 3 || total_m2 > 3) {
        printf("MERCADOS A MAIS %d - %d\n", total_m1, total_m2);
        exit(EXIT_FAILURE);
    }
}

// read all info about the markets and stock. store it in an struct
void read_stock_file (FILE *file, STOCK_LIST *stock) {
    int  num_events;
    char end_char, end_line = '\n', sparator = ';';

    for (int i = 0; i < MAXSTOCK; ++i) {
        printf("%d\n", i);

        num_events = fscanf(file, "%50[^;]%c", stock[i].market, &end_char);
        READING(num_events, end_char, sparator, "ERRO A LER STOCK\n");

        num_events = fscanf(file, "%50[^;]%c", stock[i].name, &end_char);
        READING(num_events, end_char, sparator, "ERRO A LER STOCK\n");

        num_events = fscanf(file, "%d%c", &stock[i].price, &end_char);
        READING(num_events, end_line, sparator, "ERRO A LER STOCK\n");

        stock->size++;
    }

    if (stock->size > 0) {
        check_number_markets(stock);
        check_number_stock(stock);
    }
}

void init_stock(STOCK_LIST *stock) {
    stock->size = 0;
}
