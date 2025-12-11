#ifndef CRUD_AKUN_H
#define CRUD_AKUN_H

#include <stdlib.h>
#include "console_ui.h"
#include "../accounts.h"

/* ===================================================================== */
/* CRUD AKUN KARYAWAN (LOGIN)                      */
/* ===================================================================== */

/* CREATE: menambah akun karyawan baru melalui UI */
static void menuTambahKaryawan() {
    ui_updateSize();
    clearScreen();
    drawBox(0, 0, g_cols, g_rows, "Tambah Akun Karyawan");

    if (g_employeeCount >= MAX_EMPLOYEES) {
        gotoXY(2, 2);
        printf("Kapasitas akun karyawan penuh (%d).", MAX_EMPLOYEES);
        gotoXY(2, 4);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    char username[MAX_INPUT];
    char password[MAX_INPUT];
    int roleChoice = 0;

    /* Input username */
    gotoXY(2, 2);
    printf("Masukkan username karyawan (ESC untuk batal): ");
    if (!inputText(username, MAX_INPUT)) return;   /* ESC -> batal */

    /* Cek apakah username sudah dipakai (AKTIF saja) */
    if (findEmployeeByUsername(username) >= 0) {
        gotoXY(2, 4);
        printf("Username sudah digunakan. Silakan pilih username lain.");
        gotoXY(2, 6);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    /* Input password */
    gotoXY(2, 4);
    printf("Masukkan password karyawan (ESC untuk batal): ");
    if (!inputPassword(password, MAX_INPUT)) return;  /* ESC -> batal */

    /* Pilih role */
    while (1) {
        gotoXY(2, 6);
        printf("Pilih role karyawan:");
        gotoXY(4, 7);
        printf("[1] Pengurus Pembayaran");
        gotoXY(4, 8);
        printf("[2] Pengurus Jadwal");
        gotoXY(4, 9);
        printf("[3] Pengurus Data");
        gotoXY(4, 10);
        printf("[4] Karyawan Manager");
        gotoXY(2, 12);
        printf("Masukkan pilihan (1-4, ESC batal): ");

        int ch = _getch();
        if (ch == 27) return;           /* ESC -> batal */
        if (ch >= '1' && ch <= '4') {
            roleChoice = ch - '0';
            break;
        }
    }

    Role role;
    if (roleChoice == 1)      role = ROLE_PEMBAYARAN;
    else if (roleChoice == 2) role = ROLE_JADWAL;
    else if (roleChoice == 3) role = ROLE_DATA;
    else                      role = ROLE_MANAGER;

    if (!accounts_add(username, password, role)) {
        gotoXY(2, 14);
        printf("Gagal menambah akun (kapasitas penuh / duplikat).");
    } else {
        gotoXY(2, 14);
        printf("Akun karyawan '%s' dengan role '%s' berhasil dibuat.",
               username, roleToString(role));
    }

    gotoXY(2, 16);
    printf("Tekan sembarang tombol untuk kembali...");
    _getch();
}

/* READ + UPDATE + DELETE (dari tabel) untuk akun karyawan */
static void menuLihatKaryawan() {
    int page = 0;

    while (1) {
        /* Kumpulkan ulang hanya akun aktif setiap loop (karena bisa berubah) */
        int idxList[MAX_EMPLOYEES];
        int activeCount = 0;
        for (int i = 0; i < g_employeeCount; ++i) {
            if (g_employees[i].active) {
                idxList[activeCount++] = i;
            }
        }

        if (activeCount == 0) {
            ui_updateSize();
            clearScreen();
            drawBox(0, 0, g_cols, g_rows, "Daftar Akun Karyawan (AKTIF)");
            gotoXY(2, 2);
            printf("Belum ada akun karyawan AKTIF.");
            gotoXY(2, 4);
            printf("Tekan sembarang tombol untuk kembali...");
            _getch();
            return;
        }

        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows, "Daftar Akun Karyawan (AKTIF)");

        /* center tabel akun */
        int tableWidth = 68; /* panjang baris tabel */
        int tableLeft = (g_cols - tableWidth) / 2;
        if (tableLeft < 2) tableLeft = 2;

        int headerRow = 3;

        gotoXY((short)tableLeft, (short)headerRow);
        printf("+----+----------------------+--------------------------+-----------+");
        gotoXY((short)tableLeft, (short)(headerRow + 1));
        printf("| No | Username             | Role                     | Status    |");
        gotoXY((short)tableLeft, (short)(headerRow + 2));
        printf("+----+----------------------+--------------------------+-----------+");

        int dataTop = headerRow + 3;
        int availableRows = g_rows - dataTop - 6; /* sisakan ruang footer */
        if (availableRows < 1) availableRows = 1;

        int total = activeCount;
        int totalPages = (total + availableRows - 1) / availableRows;
        if (page < 0) page = 0;
        if (page >= totalPages) page = totalPages - 1;

        int startIndex = page * availableRows;
        int endIndex   = startIndex + availableRows;
        if (endIndex > total) endIndex = total;

        int row = dataTop;
        for (int i = startIndex; i < endIndex; ++i) {
            int idx = idxList[i];
            Employee *e = &g_employees[idx];

            gotoXY((short)tableLeft, (short)row);
            printf("| %2d | %-20s | %-24s | %-9s |",
                   i + 1,              /* No global (1..activeCount) */
                   e->username,
                   roleToString(e->role),
                   e->active ? "AKTIF" : "NONAKTIF");
            row++;
        }

        gotoXY((short)tableLeft, (short)row);
        printf("+----+----------------------+--------------------------+-----------+");

        gotoXY(2, (short)(row + 1));
        printf("Total akun AKTIF: %d   Baris per halaman: %d", total, availableRows);
        gotoXY(2, (short)(row + 2));
        printf("Halaman %d dari %d", page + 1, totalPages);

        gotoXY(2, (short)(row + 3));
        printf("N/P atau Panah Kanan/Kiri: pindah halaman  |  E: Edit/Hapus  |  ESC: Kembali");

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int code = _getch();
            if (code == 77) {        /* Right */
                if (page < totalPages - 1) page++;
            } else if (code == 75) { /* Left */
                if (page > 0) page--;
            }
        } else if (ch == 'n' || ch == 'N') {
            if (page < totalPages - 1) page++;
        } else if (ch == 'p' || ch == 'P') {
            if (page > 0) page--;
        } else if (ch == 'e' || ch == 'E') {
            /* pilih baris dari kolom No */
            char buf[16];
            gotoXY(2, (short)(row + 5));
            printf("Masukkan No (1-%d) yang akan diubah/hapus (ESC batal): ", total);
            if (!inputText(buf, sizeof(buf))) {
                continue; /* ESC -> kembali ke tabel */
            }
            int no = atoi(buf);
            if (no < 1 || no > total) {
                gotoXY(2, (short)(row + 6));
                printf("Nomor tidak valid. Tekan sembarang tombol...");
                _getch();
                continue;
            }

            int idx = idxList[no - 1];

            /* Sub-menu R/U/D untuk akun ini */
            int doneSub = 0;
            while (!doneSub) {
                ui_updateSize();
                clearScreen();
                drawBox(0, 0, g_cols, g_rows, "Detail Akun Karyawan");

                Employee *e = &g_employees[idx];

                gotoXY(4, 2);
                printf("No       : %d", no);
                gotoXY(4, 3);
                printf("Username : %s", e->username);
                gotoXY(4, 4);
                printf("Role     : %s", roleToString(e->role));
                gotoXY(4, 5);
                printf("Status   : %s", e->active ? "AKTIF" : "NONAKTIF");

                gotoXY(4, 7);
                printf("[1] Ubah Password");
                gotoXY(4, 8);
                printf("[2] Ubah Role");
                gotoXY(4, 9);
                printf("[3] Hapus / Nonaktifkan Akun");
                gotoXY(4, 10);
                printf("[0] Kembali ke Tabel");

                gotoXY(4, 12);
                printf("Pilih menu: ");

                int ch2 = _getch();
                if (ch2 == '0' || ch2 == 27) {
                    doneSub = 1;
                } else if (ch2 == '1') {
                    char newPass[MAX_INPUT];
                    gotoXY(4, 14);
                    printf("Password baru (ESC batal): ");
                    if (inputPassword(newPass, MAX_INPUT)) {
                        strncpy(e->password, newPass, MAX_INPUT);
                        e->password[MAX_INPUT - 1] = '\0';
                        accounts_update(idx, e);
                        gotoXY(4, 16);
                        printf("Password berhasil diubah. Tekan sembarang tombol...");
                        _getch();
                    }
                } else if (ch2 == '2') {
                    gotoXY(4, 14);
                    printf("Pilih role baru:");
                    gotoXY(6, 15);
                    printf("[1] Pengurus Pembayaran");
                    gotoXY(6, 16);
                    printf("[2] Pengurus Jadwal");
                    gotoXY(6, 17);
                    printf("[3] Pengurus Data");
                    gotoXY(6, 18);
                    printf("[4] Karyawan Manager");
                    gotoXY(4, 20);
                    printf("Masukkan pilihan (1-4, ESC batal): ");

                    int rch = _getch();
                    if (rch == 27) continue;
                    if (rch >= '1' && rch <= '4') {
                        int rc = rch - '0';
                        if      (rc == 1) e->role = ROLE_PEMBAYARAN;
                        else if (rc == 2) e->role = ROLE_JADWAL;
                        else if (rc == 3) e->role = ROLE_DATA;
                        else              e->role = ROLE_MANAGER;

                        accounts_update(idx, e);
                        gotoXY(4, 22);
                        printf("Role berhasil diubah. Tekan sembarang tombol...");
                        _getch();
                    }
                } else if (ch2 == '3') {
                    gotoXY(4, 14);
                    printf("Yakin hapus/nonaktifkan akun '%s'? (Y/N): ", e->username);
                    int c = _getch();
                    if (c == 'y' || c == 'Y') {
                        accounts_softDelete(idx);
                        gotoXY(4, 16);
                        printf("Akun dinonaktifkan. Tekan sembarang tombol...");
                        _getch();
                        doneSub = 1; /* kembali ke tabel, data sudah berubah */
                    }
                }
            }
        } else if (ch == 27) {
            return;
        }
    }
}

/* MENU UTAMA CRUD: mengelola akun karyawan (C + R/U/D) */
static void menuKelolaAkunKaryawan() {
    int selected = 0;
    const char *items[] = {
        "Tambah Akun Karyawan",
        "Lihat / Ubah / Hapus Akun",
        "Kembali"
    };
    int itemCount = 3;

    while (1) {
        ui_updateSize();
        clearScreen();

        drawBox(0, 0, g_cols, g_rows, "Kelola Akun Karyawan");

        int boxW = 60;
        int boxH = 9;
        int left = (g_cols - boxW) / 2;
        int top  = (g_rows - boxH) / 2;

        drawBox(left, top, boxW, boxH, "");

        gotoXY((short)(left + 2), (short)(top + 2));
        printf("Pilih menu:");

        for (int i = 0; i < itemCount; ++i) {
            gotoXY((short)(left + 4), (short)(top + 3 + i));
            if (i == selected)
                printf("-> %s", items[i]);
            else
                printf("   %s", items[i]);
        }

        gotoXY((short)(left + 2), (short)(top + boxH - 2));
        printf("Panah atas/bawah, ENTER pilih, ESC kembali");

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int code = _getch();
            if (code == 72)
                selected = (selected - 1 + itemCount) % itemCount;
            else if (code == 80)
                selected = (selected + 1) % itemCount;
        } else if (ch == 27) {
            return;
        } else if (ch == 13) {
            if      (selected == 0) menuTambahKaryawan();
            else if (selected == 1) menuLihatKaryawan();   /* R/U/D */
            else return;
        }
    }
}

#endif /* CRUD_AKUN_H */