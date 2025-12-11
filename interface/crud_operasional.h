#ifndef CRUD_OPERASIONAL_H
#define CRUD_OPERASIONAL_H

#include <stdio.h>          /* Tambahan wajib */
#include "console_ui.h"
#include "console_ui.h"
#include "../stasiun.h"

/* ----------------- PLACEHOLDER CRUD STASIUN/PENUMPANG/KERETA ----------------- */

static void stasiun_create(void) {
    /* TODO: Implementasi Tambah Stasiun */
}

static void stasiun_rud(void) {
    /* TODO: Implementasi Lihat/Ubah/Hapus Stasiun */
}

static void penumpang_create(void) {
    /* TODO: Implementasi Tambah Penumpang */
}

static void penumpang_rud(void) {
    /* TODO: Implementasi Lihat/Ubah/Hapus Penumpang */
}

static void kereta_create(void) {
    /* TODO: Implementasi Tambah Kereta */
}

static void kereta_rud(void) {
    /* TODO: Implementasi Lihat/Ubah/Hapus Kereta */
}

/* --------- Submenu Kelola Stasiun --------- */
static void menuKelolaStasiun(void) {
    while (1) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows,
                "+- Menu - Kelola Stasiun -------------------------------------+");

        gotoXY(2, 2);
        printf("[1] Tambah Stasiun");
        gotoXY(2, 3);
        printf("[2] Lihat / Ubah / Hapus Stasiun");
        gotoXY(2, 5);
        printf("[0] Kembali");

        gotoXY(2, 7);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '1') {
            stasiun_create();   /* TODO */
        } else if (ch == '2') {
            stasiun_rud();      /* TODO */
        } else if (ch == '0' || ch == 27) {
            return;
        } else {
            gotoXY(2, 9);
            printf("Menu tidak valid. Tekan sembarang tombol...");
            _getch();
        }
    }
}

/* --------- Submenu Kelola Penumpang --------- */
static void menuKelolaPenumpang(void) {
    while (1) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows,
                "+- Menu - Kelola Penumpang -----------------------------------+");

        gotoXY(2, 2);
        printf("[1] Tambah Penumpang");
        gotoXY(2, 3);
        printf("[2] Lihat / Ubah / Hapus Penumpang");
        gotoXY(2, 5);
        printf("[0] Kembali");

        gotoXY(2, 7);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '1') {
            penumpang_create();   /* TODO */
        } else if (ch == '2') {
            penumpang_rud();      /* TODO */
        } else if (ch == '0' || ch == 27) {
            return;
        } else {
            gotoXY(2, 9);
            printf("Menu tidak valid. Tekan sembarang tombol...");
            _getch();
        }
    }
}

/* --------- Submenu Kelola Kereta --------- */
static void menuKelolaKereta(void) {
    while (1) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows,
                "+- Menu - Kelola Kereta --------------------------------------+");

        gotoXY(2, 2);
        printf("[1] Tambah Kereta");
        gotoXY(2, 3);
        printf("[2] Lihat / Ubah / Hapus Kereta");
        gotoXY(2, 5);
        printf("[0] Kembali");

        gotoXY(2, 7);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '1') {
            kereta_create();   /* TODO */
        } else if (ch == '2') {
            kereta_rud();      /* TODO */
        } else if (ch == '0' || ch == 27) {
            return;
        } else {
            gotoXY(2, 9);
            printf("Menu tidak valid. Tekan sembarang tombol...");
            _getch();
        }
    }
}

#endif /* CRUD_OPERASIONAL_H */