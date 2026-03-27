#include <stdio.h>
#include "client.h"

void client_menu() {
    int c;
    char name[30];
    int room;

    do {
        printf("\n===== CLIENT MENU =====\n");
        printf("1. Make reservation\n");
        printf("0. Logout\n");
        printf("Choice: ");
        scanf("%d", &c);

        if (c == 1) {
            FILE *f = fopen("reservations.txt", "a");
            printf("Your name: ");
            scanf("%s", name);
            printf("Room number: ");
            scanf("%d", &room);
            fprintf(f, "Client: %s | Room: %d\n", name, room);
            fclose(f);
            printf("Reservation saved!\n");
        }

    } while (c != 0);
}
