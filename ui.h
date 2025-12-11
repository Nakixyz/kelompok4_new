#ifndef UI_H
#define UI_H

#include "interface/console_ui.h"
#include "accounts.h"
#include "karyawan.h"       /* Pastikan ini ada */
#include "stasiun.h"        /* WAJIB ADA: Agar fungsi stasiun_init() dikenali */
#include "interface/auth_ui.h"
#include "interface/main_menu.h" /* WAJIB GANTI: Jangan pakai menu_ui.h lagi */

void ui_initConsole() {
    /* ... kode init console (biarkan sama) ... */
    system("mode con cols=200 lines=999");
    HWND hWnd = GetConsoleWindow();
    if (hWnd != NULL) ShowWindow(hWnd, SW_MAXIMIZE);
    Sleep(100);
    ui_updateSize();
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    clearScreen();
    setCursorVisible(0);
}

void ui_run() {
    /* LOAD DATA SAAT APLIKASI MULAI */
    accounts_init();
    karyawan_init();
    stasiun_init();

    while (1) {
        ui_showLoading("Memuat Halaman Login", "Silakan Login");

        Role role;
        char username[MAX_INPUT];

        /* Login logic */
        if (!loginKaryawan(&role, username, MAX_INPUT)) {
            return;
        }

        if (role == ROLE_SUPERADMIN) {
            mainMenuSuperadmin();
        } else {
            mainMenuKaryawan(role, username);
        }
    }
}

#endif