#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>


#define DEBUG2
#define READING(X,Y,Z, ...) if (X != 2 && Y != Z)  { printf(__VA_ARGS__); exit(EXIT_FAILURE); }
#define CHECK(X, ...)       if (X == -1)           { printf(__VA_ARGS__); exit(EXIT_FAILURE); }

#define BUFLEN     1024
#define PORT       9876
#define SOCKADDRIN struct sockaddr_in

#define MAXLEN   50
#define MAXUSERS 10
#define MAXSTOCK 6 


typedef struct _ADMIN {
    char name[MAXLEN];
    char password[MAXLEN];
} ADMIN;

typedef struct _USER {
    char name[MAXLEN];
    char password[MAXLEN];
    char markets[MAXLEN][2];
    int  budget;
    int pos;
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
void  admin_usage (ADMIN, USER *);


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


int number_spaces(const char *string) {
    int count = 0;

    for (int i = 0; string[i] != '\0'; ++i) {
        if (string[i] == ' ')
            count++;
    }
    return count;
}

void admin_usage (ADMIN admin, USER *users) {
    int terminal_fd;
    SOCKADDRIN terminal_addr, admin_addr;
    socklen_t t_len = sizeof(admin_addr);

    CHECK((terminal_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)), "Erro no socket\n");
	terminal_addr.sin_family 	  = AF_INET;
	terminal_addr.sin_port   	  = htons(PORT);
	terminal_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	CHECK(bind(terminal_fd, (struct sockaddr*) &terminal_addr, sizeof(terminal_addr)), "Erro no bind");

    char buf[BUFLEN], username[MAXLEN], password[MAXLEN];
    char log_men[] = "Introduza o seu nick: \n";
    char pas_men[] = "Introduza a sua password: \n";

    CHECK(recvfrom(terminal_fd, buf, BUFLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
    printf("Admin said: %s\n", buf);
    while (true) {

	    CHECK(sendto(terminal_fd, (void *) log_men, strlen(log_men), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
        CHECK(recvfrom(terminal_fd, username, MAXLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
        #ifdef DEBUG
        printf("Admin said: %s\n", username);
        #endif

	    CHECK(sendto(terminal_fd, (void *) pas_men, strlen(pas_men), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
        CHECK(recvfrom(terminal_fd, password, MAXLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
        #ifdef DEBUG
        printf("Admin said: %s\n", password);
        #endif

        if (strcmp(username, admin.name) == 10 && strcmp(password, admin.password) == 10)
            break;
        
        printf("Credencias erradas\n");
    }
	CHECK(sendto(terminal_fd, (void *) "Logged in.\n\n", strlen("Logged in.\n\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");


    char command_line[BUFLEN * 5];
    char command[BUFLEN];
    while (true) {
        CHECK(recvfrom(terminal_fd, command_line, BUFLEN * 5, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");

        sscanf(command_line, "%s", command);

        if (strcmp(command, "QUIT") == 0) {
	        CHECK(sendto(terminal_fd, (void *) "Logged out.\n\n", strlen("Logged out.\n\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            break;
        } else if (strcmp(command, "ADD_USER") == 0) {
            if (users->pos < MAXUSERS) {
                char stock1[MAXLEN], stock2[MAXLEN];
                int  spaces = number_spaces(command_line);
                int  number_events, budget;

                if (spaces == 5) {
                    number_events = sscanf(command_line, "%s %s %s %s %s %d", command, username, password, stock1, stock2, &budget);

                    if (number_events  == 6) {
                        strcpy(users[users->pos].name, username);
                        strcpy(users[users->pos].password, password);
                        strcpy(users[users->pos].markets[0], stock1);
                        strcpy(users[users->pos].markets[1], stock2);
                        users[users->pos].budget = budget;
                        users->pos++;
                    } else {
	                    CHECK(sendto(terminal_fd, (void *) "Erro a ler os parametros\n", strlen("Erro a ler os parametros\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                    }
                } else if (spaces == 4) {
                    number_events = sscanf(command_line, "%s %s %s %s %d", command, username, password, stock1, &budget);

                    if (number_events  == 5) {
                        strcpy(users[users->pos].name, username);
                        strcpy(users[users->pos].password, password);
                        strcpy(users[users->pos].markets[0], stock1);
                        strcpy(users[users->pos].markets[1], "-");
                        users[users->pos].budget = budget;
                        users->pos++;
                    } else {
	                    CHECK(sendto(terminal_fd, (void *) "Erro a ler os parametros\n", strlen("Erro a ler os parametros\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                    }
                } else {
	                CHECK(sendto(terminal_fd, (void *) "Numero de parametros errado\n", strlen("Numero de parametros errado\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                }
            } else {
	            CHECK(sendto(terminal_fd, (void *) "Maximo de users atingido\n", strlen("Maximo de users atingido\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            }
        } else if (strcmp(command, "LIST") == 0) {
            char string[BUFLEN] = "";

            for(int i = 0; i < users->pos; ++i) {
                strcat(string, users[i].name);
                strcat(string, "\n");
            }
	        
            CHECK(sendto(terminal_fd, (void *) string, strlen(string), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
        }
    }
    close(terminal_fd);
}