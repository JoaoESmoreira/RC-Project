#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>


#define DEBUG
#define READING(X,Y,Z, ...) if (X != 2 && Y != Z)  { printf(__VA_ARGS__); exit(EXIT_FAILURE); }
#define CHECK(X, ...)       if (X == -1)           { printf(__VA_ARGS__); exit(EXIT_FAILURE); }

#define BUFLEN     1024
#define PORT       9876
#define SOCKADDRIN struct sockaddr_in

#define MAXLEN   50
#define MAXSTOCK 6 


typedef struct _ADMIN {
    char name[MAXLEN];
    char password[MAXLEN];
} ADMIN;

typedef struct _USER {
    char name[MAXLEN];
    char password[MAXLEN];
    int  budget;
} USER;

typedef struct _STOCK_LIST {
    char  market[MAXLEN];
    char  name[MAXLEN];
    int   price;
} STOCK_LIST;


FILE* check_file (const char *);
ADMIN read_admin_file (FILE *);
int   read_number_users (FILE *);
void  read_user_file (FILE *, USER *, int);
void  init(STOCK_LIST *);
void  read_stock_file (FILE *, STOCK_LIST *);
void  admin_usage (int, ADMIN, SOCKADDRIN, socklen_t);


int main() {

    FILE *file  = check_file("configFile.txt");
    ADMIN admin = read_admin_file(file);

    #ifdef DEBUG
    printf("User: %s / Password: %s\n", admin.name, admin.password);
    #endif

    int number_user = read_number_users(file);

    #ifdef DEBUG
    printf("Number of users: %d\n", number_user);
    #endif

    USER users[number_user];
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
    int terminal_fd;
    SOCKADDRIN terminal_addr, admin_addr;
    socklen_t t_len = sizeof(admin_addr);

    CHECK((terminal_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)), "Erro no socket\n");
	terminal_addr.sin_family 	  = AF_INET;
	terminal_addr.sin_port   	  = htons(PORT);
	terminal_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	CHECK(bind(terminal_fd, (struct sockaddr*) &terminal_addr, sizeof(terminal_addr)), "Erro no bind");
    admin_usage (terminal_fd, admin, admin_addr, t_len);

    close(terminal_fd);
    return 0;
}

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


void admin_usage (int terminal_fd, ADMIN admin, SOCKADDRIN admin_addr, socklen_t t_len) {
    char buf[BUFLEN], username[BUFLEN], password[BUFLEN];
    char log_men[] = "Introduza o seu nick: \n";
    char pas_men[] = "Introduza a sua password\n";

        CHECK(recvfrom(terminal_fd, buf, BUFLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
        printf("Admin said: %s\n", buf);
    while (true) {

	    CHECK(sendto(terminal_fd, (void *) log_men, strlen(log_men), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
        CHECK(recvfrom(terminal_fd, username, BUFLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
        #ifdef DEBUG
        printf("Admin said: %s\n", username);
        #endif

	    CHECK(sendto(terminal_fd, (void *) pas_men, strlen(pas_men), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
        CHECK(recvfrom(terminal_fd, password, BUFLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");

        #ifdef DEBUG
        printf("Admin said: %s\n", password);
        #endif

        if (strcmp(username, admin.name) == 10 && strcmp(password, admin.password) == 10)
            break;
        
        printf("Credencias erradas\n");
    }
    printf("Loged in\n");
    // continuar o trabalho ou retornar true
}