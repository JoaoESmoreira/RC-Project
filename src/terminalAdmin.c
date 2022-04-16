#include "server_header.h"

static int number_spaces(const char *string) {
    int count = 0;

    for (int i = 0; string[i] != '\0'; ++i) {
        if (string[i] == ' ')
            count++;
    }
    return count;
}

static bool user_in_list (const USER *users, const char *username) {
    for (int i = 0; i < users->size; ++i) {
        if (strcmp(users[i].name, username) == 0)
            return true;
    }
    return false;
}

static void add_user_atributs(USER *users, const char *name, const char *password, const char * stock1, const char * stock2, const int budget) {
    strcpy(users[users->size].name, name);
    strcpy(users[users->size].password, password);
    strcpy(users[users->size].markets[0], stock1);
    strcpy(users[users->size].markets[1], stock2);
    users[users->size].budget = budget;
}

static bool in_stock (const STOCK_LIST *stock, const char *stock1, const char *stock2) {
    bool flag1 = false, flag2 = false;

    if (strcmp(stock2, "-") == 0)
        flag2 = true;

    for (int i = 0; i < stock->size; ++i) {
        if (strcmp(stock1, stock[i].name) == 0) {
            flag1 = true;
        }
        if (strcmp(stock2, stock[i].name) == 0) {
            flag1 = true;
        }
    }
    return flag1 && flag2;
}

static void add_user (USER *users, const char * command_line, const STOCK_LIST *stock, int terminal_fd, SOCKADDRIN admin_addr) {
    char command[MAXLEN], username[MAXLEN], password[MAXLEN];

    if (users->size < MAXUSERS) {
        char stock1[MAXLEN], stock2[MAXLEN];
        int  spaces = number_spaces(command_line);
        int  number_events, budget;

        if (spaces == 5) {
            number_events = sscanf(command_line, "%s %s %s %s %s %d", command, username, password, stock1, stock2, &budget);

            if (!in_stock(stock, stock1, stock2)) {
                printf("Nome de bolsas enesistentes\n");
                return;
            }

            if (number_events  == 6) {
                if (!user_in_list(users, username)) {
                    add_user_atributs(users, username, password, stock1, stock2, budget);
                    users->size++;
                } else {
                    for (int i = 0; i < users->size; ++i) {
                        if (strcmp(users[i].name, username) == 0) {
                            add_user_atributs(users, username, password, stock1, stock2, budget);
                            break;
                        }
                    }
                }
            } else {
	            CHECK(sendto(terminal_fd, (void *) "Erro a ler os parametros\n", strlen("Erro a ler os parametros\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            }
        } else if (spaces == 4) {
            number_events = sscanf(command_line, "%s %s %s %s %d", command, username, password, stock1, &budget);

            if (!in_stock(stock, stock1, "-")) {
	            CHECK(sendto(terminal_fd, (void *) "Nome de bolsas enesistentes\n", strlen("Nome de bolsas enesistentes\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                return;
            }

            if (number_events  == 5) {
                if (!user_in_list(users, username)) {
                    add_user_atributs(users, username, password, stock1, "-", budget);
                    users->size++;
                } else {
                    for (int i = 0; i < users->size; ++i) {
                        if (strcmp(users[i].name, username) == 0) {
                            add_user_atributs(users, username, password, stock1, "-", budget);
                            break;
                        }
                    }
                }
            } else {
	            CHECK(sendto(terminal_fd, (void *) "Erro a ler os parametros\n", strlen("Erro a ler os parametros\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            }
        } else {
	        CHECK(sendto(terminal_fd, (void *) "Numero de parametros errado\n", strlen("Numero de parametros errado\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
        }
    } else {
	    CHECK(sendto(terminal_fd, (void *) "Maximo de users atingido\n", strlen("Maximo de users atingido\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
    }
}

static void list (USER *users, char *string) {
    char aux[MAXLEN];

    if (users->size == 0)
        strcat(string, "Lista vazia\n");

    for (int i = 0; i < users->size; ++i) {
        strcat(string, users[i].name); strcat(string, " ");
        strcat(string, users[i].password); strcat(string, " ");
        strcat(string, users[i].markets[0]); strcat(string, " ");
        strcat(string, users[i].markets[1]); strcat(string, " ");
        sprintf(aux, "%d\n", users[i].budget);
        strcat(string, aux);
    }
    strcat(string, "\n");
}

static void delete (USER *users, const char *name) {
    if (users->size > 0) {

        for (int i = 0; i < users->size; ++i) {
            if (strcmp(name, users[i].name) == 0) {
                #ifdef DEBUG
                printf("%s - %s\n", users[i].name, name);
                printf("DELETE: %d\n", strcmp(name, users[i].name));
                #endif

                // eliminar o ultimo
                if (i == users->size - 1) {
                    users->size--;
                    break;
                } 

                // trocar o ultimo com o user a eliminar
                int pos     = users->size;
                users->size = i;
                add_user_atributs(users, users[pos - 1].name, users[pos - 1].password, users[pos - 1].markets[0], users[pos - 1].markets[1], users[pos - 1].budget);
                users->size = pos - 1;
                break;
            }
        }
    } 
}

void* admin_usage (void *args) {
    // read args
    ADMIN_SERVER_ARGS argumento = *((ADMIN_SERVER_ARGS *) args);
    ADMIN admin  = argumento.admin;
    USER  *users = argumento.users;
    STOCK_LIST *stock = argumento.stock;

    SOCKADDRIN terminal_addr, admin_addr;
    socklen_t  t_len = sizeof(admin_addr);
    char       command_line[BUFLEN];
    char       command[BUFLEN];
    int        terminal_fd;


    while (strcmp(command, "QUIT_SERVER")) {

        CHECK((terminal_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)), "Erro no socket\n");
	    terminal_addr.sin_family 	  = AF_INET;
	    terminal_addr.sin_port   	  = htons(PORT);
	    terminal_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	    CHECK(bind(terminal_fd, (struct sockaddr*) &terminal_addr, sizeof(terminal_addr)), "Erro no bind");

        char buf[BUFLEN], username[MAXLEN], password[MAXLEN];
        char log_men[] = "Introduza o seu nick: \n";
        char pas_men[] = "Introduza a sua password: \n";

        CHECK(recvfrom(terminal_fd, buf, BUFLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");

        // login
        while (true) {

	        CHECK(sendto(terminal_fd, (void *) log_men, strlen(log_men), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            CHECK(recvfrom(terminal_fd, username, MAXLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
            #ifdef DEBUG
            printf("Admin said: %s - %d\n", username, strcmp(username, admin.name));
            #endif

	        CHECK(sendto(terminal_fd, (void *) pas_men, strlen(pas_men), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
            CHECK(recvfrom(terminal_fd, password, MAXLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");
            #ifdef DEBUG
            printf("Admin said: %s - %d\n", password, strcmp(password, admin.password));
            #endif

            if (strcmp(username, admin.name) == 10 && strcmp(password, admin.password) == 10)
                break;

            printf("Credencias erradas\n");
        }
	    CHECK(sendto(terminal_fd, (void *) "Logged in.\n\n", strlen("Logged in.\n\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");


        // comandos terminal
        while (true) {
            CHECK(recvfrom(terminal_fd, command_line, BUFLEN, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");

            sscanf(command_line, "%s", command);

            if (strcmp(command, "QUIT") == 0 || strcmp(command, "QUIT_SERVER") == 0) {
	            CHECK(sendto(terminal_fd, (void *) "Logged out.\n\n", strlen("Logged out.\n\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");

                close(terminal_fd);
                break;
            } else if (strcmp(command, "ADD_USER") == 0) {

                add_user(users, command_line, stock, terminal_fd, admin_addr);

            } else if (strcmp(command, "LIST") == 0) {
                char string[BUFLEN] = "";
                
                list(users, string);

                CHECK(sendto(terminal_fd, (void *) string, strlen(string), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");

            } else if (strcmp(command, "REFRESH") == 0) {
                int number_events = sscanf(command_line, "%s %d", command, &REFRESH_TIME);
                if (number_events != 2) {
                    CHECK(sendto(terminal_fd, (void *) "Erro no comando\n", strlen("Erro no comando\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                }

                #ifdef DEBUG
                printf("%d\n", REFRESH_TIME);
                #endif
            } else if (strcmp(command, "DEL") == 0) {
                char name[MAXLEN];
                int  number_events = sscanf(command_line, "%s %s", command, name);

                if (number_events != 2) {
                    CHECK(sendto(terminal_fd, (void *) "Erro no comando\n", strlen("Erro no comando\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                } else {
                    delete(users, name);
                }
            }
        }
        close(terminal_fd);
        // dar tempo para o admin sair do processo dele
        sleep(2);
    }
    pthread_exit(NULL);
}