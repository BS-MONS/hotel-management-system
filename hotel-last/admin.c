#include <stdio.h>
#include "admin.h"

void admin_menu() {
    int c;
    char line[100];

    do {
        printf("\n===== ADMIN MENU =====\n");
        printf("1. View reservations\n");
        printf("2. Delete all reservations\n");
        printf("0. Logout\n");
        printf("Choice: ");
        scanf("%d", &c);

        if (c == 1) {
            FILE *f = fopen("reservations.txt", "r");
            if (!f) {
                printf("No reservations found.\n");
            } else {
                while (fgets(line, sizeof(line), f))
                    printf("%s", line);
                fclose(f);
            }
        }
        else if (c == 2) {
            FILE *f = fopen("reservations.txt", "w");
            fclose(f);
            printf("All reservations deleted.\n");
        }

    } while (c != 0);
}
