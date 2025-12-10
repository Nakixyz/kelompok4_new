#ifndef UI_H
#define UI_H

#include "interface/console_ui.h"
#include "accounts.h"
#include "interface/auth_ui.h"
#include "interface/menu_ui.h"

/* inisialisasi: buffer besar, maximize, warna cyan */
void ui_initConsole() {
    /* buffer besar supaya window bisa dibesarkan penuh */
    system("mode con cols=200 lines=999");

    HWND hWnd = GetConsoleWindow();
    if (hWnd != NULL) {
        ShowWindow(hWnd, SW_MAXIMIZE);
    }

    Sleep(100);
    ui_updateSize();

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    /* warna cyan (hijau + biru + intensity) */
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    clearScreen();
    setCursorVisible(0);
}

/* flow utama: menu pilih role -> loading -> login -> menu utama */
void ui_run() {
    /* load semua akun dan data karyawan */
    accounts_init();
    karyawan_init();

    while (1) {
        int choice = ui_loginMenu();
        if (choice == 2) {
            return; /* Keluar */
        } else if (choice == 0) {
            /* Loading sebelum tampil form login Superadmin */
            ui_showLoading("Memuat Halaman Login", "Login Superadmin");
            if (loginSuperadmin()) {
                mainMenuSuperadmin();
            }
        } else if (choice == 1) {
            /* Loading sebelum tampil form login Karyawan */
            ui_showLoading("Memuat Halaman Login", "Login Karyawan");
            Role role;
            char username[MAX_INPUT];
            if (loginKaryawan(&role, username, MAX_INPUT)) {
                mainMenuKaryawan(role, username);
            }
        }
    }
}


#endif /* UI_H */
