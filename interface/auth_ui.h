#ifndef AUTH_UI_H
#define AUTH_UI_H

#include "console_ui.h"
#include "../accounts.h"

#define SUPER_USER "superadmin"
#define SUPER_PASS "superadmin"

/* ---------- Layout Login (ASCII + panel) ---------- */

static void drawLoginStaticUI(int *uX, int *uY, int *pX, int *pY) {
    ui_updateSize();
    clearScreen();
    setCursorVisible(0);

    int outerLeft = 0, outerTop = 0;
    int outerWidth = g_cols;
    int outerHeight = g_rows;

    /* border luar */
    drawBox(outerLeft, outerTop, outerWidth, outerHeight, "");

    /* ASCII art header */
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

    /* area bawah logo */
    int baseTop = headerStartY + logoCount + 1;

    int leftWidth = 26;
    int leftHeight = outerHeight - (baseTop - outerTop) - 4;
    if (leftHeight < 10) leftHeight = 10;

    int dividerX = outerLeft + leftWidth + 4;

    for (int i = baseTop - 1; i < outerTop + outerHeight - 1; ++i) {
        gotoXY((short)dividerX, (short)i);
        putchar('|');
    }

    gotoXY((short)(dividerX + 1), (short)(baseTop - 1));
    printf("Login >");

    /* panel kiri */
    drawBox(outerLeft + 2, baseTop, leftWidth, leftHeight, "- - Kelompok 4 - -");
    drawBox(outerLeft + 4, baseTop + 2, leftWidth - 4, leftHeight - 6, "Menu");
    gotoXY((short)(outerLeft + 6), (short)(baseTop + 4));
    printf("[0] Keluar");

    /* hint bawah kiri */
    gotoXY((short)(outerLeft + 2), (short)(outerTop + outerHeight - 2));
    printf("Tekan TAB untuk melihat password");

    /* panel kanan login */
    int rightLeft = dividerX + 2;
    int rightTop = baseTop;
    int rightWidth = outerWidth - rightLeft - 2;
    int rightHeight = outerHeight - baseTop - 2;
    if (rightWidth < 20) rightWidth = 20;
    if (rightHeight < 10) rightHeight = 10;

    drawBox(rightLeft, rightTop, rightWidth, rightHeight, "");

    const char *welcome = "Welcome to Fourtrain";
    int welcomeX = rightLeft + (rightWidth - (int)strlen(welcome)) / 2;
    if (welcomeX < rightLeft + 1) welcomeX = rightLeft + 1;
    gotoXY((short)welcomeX, (short)(rightTop + 1));
    printf("%s", welcome);

    /* username */
    gotoXY((short)(rightLeft + 2), (short)(rightTop + 3));
    printf("Nama Pengguna");
    int boxLeft = rightLeft + 2;
    int boxWidth = rightWidth - 4;
    drawInputBox(boxLeft, rightTop + 4, boxWidth);

    /* password */
    gotoXY((short)(rightLeft + 2), (short)(rightTop + 8));
    printf("Kata Sandi");
    drawInputBox(boxLeft, rightTop + 9, boxWidth);

    /* tombol ENTER */
    int btnWidth = 16;
    int btnLeft = rightLeft + (rightWidth - btnWidth) / 2;
    int btnTop = rightTop + rightHeight - 5;
    drawBox(btnLeft, btnTop, btnWidth, 3, "");
    gotoXY((short)(btnLeft + (btnWidth - 5) / 2), (short)(btnTop + 1));
    printf("ENTER");

    /* posisi input actual */
    if (uX) *uX = boxLeft + 2;
    if (uY) *uY = rightTop + 5;
    if (pX) *pX = boxLeft + 2;
    if (pY) *pY = rightTop + 10;
}

/* ---------- Login Superadmin ---------- */

static int loginSuperadmin() {
    char username[MAX_INPUT];
    char password[MAX_INPUT];

    while (1) {
        int uX, uY, pX, pY;
        drawLoginStaticUI(&uX, &uY, &pX, &pY);

        gotoXY((short)uX, (short)uY);
        setCursorVisible(1);
        if (!inputText(username, MAX_INPUT)) {
            return 0; /* ESC */
        }

        gotoXY((short)pX, (short)pY);
        setCursorVisible(1);
        if (!inputPassword(password, MAX_INPUT)) {
            return 0; /* ESC */
        }

        setCursorVisible(0);

        if (strcmp(username, SUPER_USER) == 0 &&
            strcmp(password, SUPER_PASS) == 0) {
            gotoXY(35, 22);
            printf("Login berhasil sebagai Superadmin.");
            gotoXY(35, 23);
            printf("Tekan sembarang tombol untuk ke Menu Utama...");
            _getch();
            return 1;
        } else {
            gotoXY(35, 22);
            printf("Login gagal! Nama pengguna / kata sandi salah.");
            gotoXY(35, 23);
            printf("Tekan sembarang tombol untuk coba lagi (ESC untuk kembali)...");
            int ch = _getch();
            if (ch == 27) return 0;
        }
    }
}

/* ---------- Login Karyawan ---------- */

static int loginKaryawan(Role *outRole, char *outUsername, int outUsernameSize) {
    char username[MAX_INPUT];
    char password[MAX_INPUT];

    if (g_employeeCount == 0) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows, "");
        gotoXY(2, 2);
        printf("Belum ada akun karyawan. Silakan minta Superadmin untuk membuat akun.");
        gotoXY(2, 4);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return 0;
    }

    while (1) {
        int uX, uY, pX, pY;
        drawLoginStaticUI(&uX, &uY, &pX, &pY);

        gotoXY((short)uX, (short)uY);
        setCursorVisible(1);
        if (!inputText(username, MAX_INPUT)) {
            return 0; /* ESC */
        }

        gotoXY((short)pX, (short)pY);
        setCursorVisible(1);
        if (!inputPassword(password, MAX_INPUT)) {
            return 0; /* ESC */
        }

        setCursorVisible(0);

        int found = -1;
        for (int i = 0; i < g_employeeCount; ++i) {
            if (g_employees[i].active &&
                strcmp(g_employees[i].username, username) == 0 &&
                strcmp(g_employees[i].password, password) == 0) {
                found = i;
                break;
            }
        }

        if (found >= 0) {
            Employee *e = &g_employees[found];
            gotoXY(35, 22);
            printf("Login berhasil sebagai %s.", roleToString(e->role));
            gotoXY(35, 23);
            printf("Tekan sembarang tombol untuk ke Menu Utama...");
            _getch();
            if (outRole) *outRole = e->role;
            if (outUsername && outUsernameSize > 0) {
                strncpy(outUsername, e->username, outUsernameSize - 1);
                outUsername[outUsernameSize - 1] = '\0';
            }
            return 1;
        } else {
            gotoXY(35, 22);
            printf("Login gagal! Username/kata sandi salah atau akun tidak aktif.");
            gotoXY(35, 23);
            printf("Tekan sembarang tombol untuk coba lagi (ESC untuk kembali)...");
            int ch = _getch();
            if (ch == 27) return 0;
        }
    }
}

#endif /* AUTH_UI_H */
