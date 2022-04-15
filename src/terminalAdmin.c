#include "server_header.h"

static int number_spaces(const char *string) {
    int count = 0;

    for (int i = 0; string[i] != '\0'; ++i) {
        if (string[i] == ' ')
            count++;
    }
    return count;
}

void admin_usage (ADMIN admin, USER *users) {
    SOCKADDRIN terminal_addr, admin_addr;
    socklen_t  t_len = sizeof(admin_addr);
    char       command_line[BUFLEN * 5];
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


        // comandos terminal
        while (true) {
            CHECK(recvfrom(terminal_fd, command_line, BUFLEN * 5, MSG_WAITALL, (struct sockaddr *) &admin_addr, (socklen_t *)&t_len), "Erro a recever");

            sscanf(command_line, "%s", command);

            if (strcmp(command, "QUIT") == 0 || strcmp(command, "QUIT_SERVER") == 0) {
	            CHECK(sendto(terminal_fd, (void *) "Logged out.\n\n", strlen("Logged out.\n\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                close(terminal_fd);
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

            } else if (strcmp(command, "REFRESH") == 0) {
                int number_events = sscanf(command_line, "%s %d", command, &REFRESH_TIME);
                if (number_events != 2) {
                    CHECK(sendto(terminal_fd, (void *) "Erro no comando\n", strlen("Erro no comando\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                }

                #ifdef DEBUG2
                printf("%d\n", REFRESH_TIME);
                #endif
            } else if (strcmp(command, "DEL") == 0) {
                char name[MAXLEN];
                int  number_events = sscanf(command_line, "%s %s", command, name);
                if (number_events != 2) {
                    CHECK(sendto(terminal_fd, (void *) "Erro no comando\n", strlen("Erro no comando\n"), MSG_CONFIRM, (struct sockaddr *) &admin_addr, sizeof(admin_addr)), "Erro a enviar\n");
                } else {
                    USER new_user[MAXUSERS]; new_user->pos = 0;

                    for (int i = 0; i < users->pos; ++i) {
                        if (strcmp(name, users[i].name) != 0) {
                            new_user[new_user->pos] = users[i];
                            new_user->pos++;
                        }
                    }
                    users = new_user;
                }
            }
        }
        close(terminal_fd);
        // dar tempo para o admin sair do processo dele
        sleep(2);
    }
}