#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <stdlib.h>         /* atoi */
#include "console_ui.h"
#include "crud_akun.h"
#include "crud_karyawan.h"
#include "dashboard_roles.h"

/* ===================================================================== */
/* MENU UTAMA SUPERADMIN & LOGIN                     */
/* ===================================================================== */

/* Menu Utama Superadmin: kotak di tengah + ASCII header */
static void mainMenuSuperadmin() {
    int selected = 0;
    const char *items[] = {
        "Kelola Akun Karyawan",
        "Kelola Data Karyawan",
        "Logout"
    };
    int itemCount = 3;

    while (1) {
        ui_updateSize();
        clearScreen();

        int outerLeft   = 0;
        int outerTop    = 0;
        int outerWidth  = g_cols;
        int outerHeight = g_rows;

        drawBox(outerLeft, outerTop, outerWidth, outerHeight, "");

        const char *logoLines[] = {
            "  _      __    __                     __         ____              __           _    ",
            " | | /| / /__ / /______  __ _  ___   / /____    / __/__  __ ______/ /________ _(_)__ ",
            " | |/ |/ / -_) / __/ _ \\/  ' \\/ -_) / __/ _ \\  / _// _ \\/ // / __/ __/ __/ _ `/ / _ \\",
            " |__/|__/\\__/_/\\__/\\___/_/_/_/\\__/  \\__/\\___/ /_/  \\___/\\_,_/_/  \\__/_/  \\_,_/_/_//_/"
        };
        int logoCount = 4;
        int logoWidth = (int)strlen(logoLines[0]);

        int headerStartX = outerLeft + (outerWidth - logoWidth) / 2;
        if (headerStartX < outerLeft + 1) headerStartX = outerLeft + 1;
        int headerStartY = outerTop + 1;

        for (int i = 0; i < logoCount; ++i) {
            gotoXY((short)headerStartX, (short)(headerStartY + i));
            printf("%s", logoLines[i]);
        }

        int baseTop = headerStartY + logoCount + 1;

        const char *panelTitle = "Menu Utama - Superadmin";
        int panelTitleX = outerLeft + (outerWidth - (int)strlen(panelTitle)) / 2;
        if (panelTitleX < outerLeft + 2) panelTitleX = outerLeft + 2;
        gotoXY((short)panelTitleX, (short)baseTop);
        printf("%s", panelTitle);

        int usableLeft   = outerLeft + 1;
        int usableTop    = baseTop + 2;
        int usableWidth  = outerWidth - 2;
        int usableHeight = outerHeight - usableTop - 2;
        if (usableWidth < 20)  usableWidth = 20;
        if (usableHeight < 10) usableHeight = 10;

        int menuBoxW = 48;
        int menuBoxH = 9;

        if (menuBoxW > usableWidth - 2)  menuBoxW = usableWidth - 2;
        if (menuBoxH > usableHeight - 2) menuBoxH = usableHeight - 2;

        int menuBoxLeft = usableLeft + (usableWidth  - menuBoxW) / 2;
        int menuBoxTop  = usableTop  + (usableHeight - menuBoxH) / 2;

        drawBox(menuBoxLeft, menuBoxTop, menuBoxW, menuBoxH, "");

        int innerLeft = menuBoxLeft + 2;
        int innerTop  = menuBoxTop + 1;

        gotoXY((short)innerLeft, (short)innerTop);
        printf("Pilih menu:");

        for (int i = 0; i < itemCount; ++i) {
            short y = (short)(innerTop + 2 + i);
            gotoXY((short)innerLeft, y);
            if (i == selected)
                printf("-> %s", items[i]);
            else
                printf("   %s", items[i]);
        }

        gotoXY((short)(outerLeft + 2), (short)(outerTop + outerHeight - 2));
        printf("Panah atas/bawah: pindah   ENTER: pilih   ESC: logout");

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int code = _getch();
            if (code == 72) {
                selected = (selected - 1 + itemCount) % itemCount;
            } else if (code == 80) {
                selected = (selected + 1) % itemCount;
            }
        } else if (ch == 27) {
            return;
        } else if (ch == 13) {
            if (selected == 0) {
                menuKelolaAkunKaryawan();
            } else if (selected == 1) {
                menuKelolaDataKaryawan();
            } else {
                return;
            }
        }
    }
}

/* Menu awal: pilih login sebagai Superadmin / Karyawan / Keluar */
static int ui_loginMenu() {
    int selected = 0;
    const char *items[] = {
        "Login sebagai Superadmin",
        "Login sebagai Karyawan",
        "Keluar"
    };
    int itemCount = 3;

    while (1) {
        ui_updateSize();
        clearScreen();

        drawBox(0, 0, g_cols, g_rows, "");

        int boxW = 50;
        int boxH = 9;
        int left = (g_cols - boxW) / 2;
        int top  = (g_rows - boxH) / 2;

        drawBox(left, top, boxW, boxH, " Menu Login ");

        gotoXY((short)(left + 2), (short)(top + 2));
        printf("Pilih jenis login:");

        for (int i = 0; i < itemCount; ++i) {
            gotoXY((short)(left + 4), (short)(top + 3 + i));
            if (i == selected)
                printf("-> %s", items[i]);
            else
                printf("   %s", items[i]);
        }

        gotoXY((short)(left + 2), (short)(top + boxH - 2));
        printf("Panah atas/bawah, ENTER pilih, ESC = Keluar");

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int code = _getch();
            if (code == 72) {
                selected = (selected - 1 + itemCount) % itemCount;
            } else if (code == 80) {
                selected = (selected + 1) % itemCount;
            }
        } else if (ch == 13) {
            return selected;  /* 0=superadmin,1=karyawan,2=keluar */
        } else if (ch == 27) {
            return 2;         /* treat ESC as Keluar */
        }
    }
}

#endif /* MAIN_MENU_H */