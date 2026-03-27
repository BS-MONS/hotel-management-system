#include <stdio.h>
#include <string.h>
#include "login.h"

int login() {
    char user[30], pass[30];
    char fuser[30], fpass[30];
    int role;
    FILE *f;

    printf("\n===== LOGIN =====\n");
    printf("1. Admin\n");
    printf("2. Client\n");
    printf("Choice: ");
    scanf("%d", &role);

    if (role == 1)
        f = fopen("admins.txt", "r");
    else
        f = fopen("clients.txt", "r");

    if (!f) {
        printf("Login file error!\n");
        return 0;
    }

    printf("Username: ");
    scanf("%s", user);
    printf("Password: ");
    scanf("%s", pass);

    while (fscanf(f, "%s %s", fuser, fpass) != EOF) {
        if (strcmp(user, fuser) == 0 && strcmp(pass, fpass) == 0) {
            fclose(f);
            return role;
        }
    }

    fclose(f);
    printf("Wrong login!\n");
    return 0;
}
