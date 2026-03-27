#include <stdio.h>
#include "login.h"
#include "admin.h"
#include "client.h"

int main() {
    int role = login();

    if (role == 1)
        admin_menu();
    else if (role == 2)
        client_menu();
    else
        printf("Exiting program.\n");

    return 0;
}
