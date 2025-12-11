#ifndef DASHBOARD_ROLES_H
#define DASHBOARD_ROLES_H

#include "console_ui.h"
#include "crud_operasional.h"

/* ===================================================================== */
/* MENU KARYAWAN PER ROLE                           */
/* ===================================================================== */

/* ----------------- MENU ROLE PEMBAYARAN ----------------- */
static void menuKaryawanPembayaran(const char *username) {
    while (1) {
        ui_updateSize();
        clearScreen();
        char title[64];
        snprintf(title, sizeof(title), "Menu - Pembayaran (%s)", username);
        drawBox(0, 0, g_cols, g_rows, title);

        gotoXY(2, 2);
        printf("[1] Input Pembayaran (simulasi)");
        gotoXY(2, 3);
        printf("[2] Ubah Status Pembayaran (simulasi)");
        gotoXY(2, 4);
        printf("[3] Lihat Daftar Pembayaran (simulasi)");
        gotoXY(2, 6);
        printf("[0] Logout");

        gotoXY(2, 8);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '0' || ch == 27) return;
        else {
            gotoXY(2, 10);
            printf("Fitur belum diimplementasikan. Tekan sembarang tombol...");
            _getch();
        }
    }
}

/* ----------------- MENU ROLE JADWAL ----------------- */
static void menuKaryawanJadwal(const char *username) {
    while (1) {
        ui_updateSize();
        clearScreen();
        char title[64];
        snprintf(title, sizeof(title), "Menu - Jadwal (%s)", username);
        drawBox(0, 0, g_cols, g_rows, title);

        gotoXY(2, 2);
        printf("[1] Buat / Ubah Jadwal (simulasi)");
        gotoXY(2, 3);
        printf("[2] Lihat Jadwal (simulasi)");
        gotoXY(2, 5);
        printf("[0] Logout");

        gotoXY(2, 7);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '0' || ch == 27) return;
        else {
            gotoXY(2, 9);
            printf("Fitur belum diimplementasikan. Tekan sembarang tombol...");
            _getch();
        }
    }
}

/* ----------------- MENU ROLE DATA ----------------- */
/* sekarang berisi Kelola Stasiun/Penumpang/Kereta */
/* ... kode sebelumnya ... */

static void menuKaryawanData(const char *username) {
    while (1) {
        ui_updateSize();
        clearScreen();

        char title[128];
        snprintf(title, sizeof(title), "Menu - Data (%s)", username);
        drawBox(0, 0, g_cols, g_rows, title);

        gotoXY(2, 2); printf("[1] Kelola Stasiun");
        gotoXY(2, 3); printf("[2] Kelola Penumpang");
        gotoXY(2, 4); printf("[3] Kelola Kereta");
        gotoXY(2, 6); printf("[0] Logout");
        gotoXY(2, 8); printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '1') {
            menuKelolaStasiun(); /* <-- Pastikan ini memanggil fungsi dari crud_operasional.h */
        } else if (ch == '2') {
            menuKelolaPenumpang();
        } else if (ch == '3') {
            menuKelolaKereta();
        } else if (ch == '0' || ch == 27) {
            return;
        }
    }
}
/* ... kode selanjutnya ... */

static void menuKaryawanManager(const char *username) {
    while (1) {
        ui_updateSize();
        clearScreen();
        char title[64];
        snprintf(title, sizeof(title), "Menu - Manager (%s)", username);
        drawBox(0, 0, g_cols, g_rows, title);

        gotoXY(2, 2);
        printf("[1] Lihat Laporan Pembayaran (simulasi)");
        gotoXY(2, 3);
        printf("[2] Lihat Laporan Jadwal (simulasi)");
        gotoXY(2, 4);
        printf("[3] Lihat Laporan Data (simulasi)");
        gotoXY(2, 6);
        printf("[0] Logout");

        gotoXY(2, 8);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '0' || ch == 27) return;
        else {
            gotoXY(2, 10);
            printf("Fitur belum diimplementasikan. Tekan sembarang tombol...");
            _getch();
        }
    }
}

static void mainMenuKaryawan(Role role, const char *username) {
    switch (role) {
        case ROLE_PEMBAYARAN:
            menuKaryawanPembayaran(username);
            break;
        case ROLE_JADWAL:
            menuKaryawanJadwal(username);
            break;
        case ROLE_DATA:
            menuKaryawanData(username);
            break;
        case ROLE_MANAGER:
            menuKaryawanManager(username);
            break;
        default:
            break;
    }
}

#endif /* DASHBOARD_ROLES_H */