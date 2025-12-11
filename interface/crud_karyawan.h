#ifndef CRUD_KARYAWAN_H
#define CRUD_KARYAWAN_H

#include <stdlib.h>
#include "console_ui.h"
#include "../karyawan.h"

/* ===================================================================== */
/* CRUD DATA KARYAWAN (DETAIL)                     */
/* ===================================================================== */

static void dataKaryawan_tambah() {
    ui_updateSize();
    clearScreen();
    drawBox(0, 0, g_cols, g_rows, "Tambah Data Karyawan");

    if (g_karyawanCount >= MAX_KARYAWAN) {
        gotoXY(2, 2);
        printf("Kapasitas data karyawan penuh (%d).", MAX_KARYAWAN);
        gotoXY(2, 4);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    Karyawan k;
    memset(&k, 0, sizeof(Karyawan));

    gotoXY(2, 2);
    printf("Masukkan ID Karyawan (max 10, ESC batal): ");
    if (!inputText(k.id, sizeof(k.id))) return;

    if (karyawan_findById(k.id) >= 0) {
        gotoXY(2, 4);
        printf("ID karyawan sudah terdaftar.");
        gotoXY(2, 6);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    gotoXY(2, 4);
    printf("Nama Karyawan (max 50, ESC batal): ");
    if (!inputText(k.nama, sizeof(k.nama))) return;

    gotoXY(2, 6);
    printf("Email (max 100, ESC batal): ");
    if (!inputText(k.email, sizeof(k.email))) return;

    gotoXY(2, 8);
    printf("Tanggal Lahir (YYYY-MM-DD, ESC batal): ");
    if (!inputText(k.tanggalLahir, sizeof(k.tanggalLahir))) return;

    gotoXY(2, 10);
    printf("Jenis Kelamin (L/P, ESC batal): ");
    char jkBuf[4] = {0};
    if (!inputText(jkBuf, sizeof(jkBuf))) return;
    k.jenisKelamin = (jkBuf[0] == 'p' || jkBuf[0] == 'P') ? 'P' : 'L';

    gotoXY(2, 12);
    printf("No. Telp (max 10, ESC batal): ");
    if (!inputText(k.noTelp, sizeof(k.noTelp))) return;

    gotoXY(2, 14);
    printf("Jabatan (max 20, ESC batal): ");
    if (!inputText(k.jabatan, sizeof(k.jabatan))) return;

    k.active = 1;

    if (!karyawan_add(&k)) {
        gotoXY(2, 16);
        printf("Gagal menambah data karyawan (kapasitas penuh / ID duplikat).");
    } else {
        gotoXY(2, 16);
        printf("Data karyawan dengan ID '%s' berhasil ditambahkan.", k.id);
    }

    gotoXY(2, 18);
    printf("Tekan sembarang tombol untuk kembali...");
    _getch();
}

/* READ + UPDATE + DELETE dari tabel Data Karyawan */
static void dataKaryawan_lihat() {
    int page = 0;

    while (1) {
        /* kumpulkan ulang hanya yang aktif */
        int idxList[MAX_KARYAWAN];
        int activeCount = 0;
        for (int i = 0; i < g_karyawanCount; ++i) {
            if (g_karyawan[i].active) {
                idxList[activeCount++] = i;
            }
        }

        if (activeCount == 0) {
            ui_updateSize();
            clearScreen();
            drawBox(0, 0, g_cols, g_rows, "Daftar Data Karyawan (AKTIF)");
            gotoXY(2, 2);
            printf("Belum ada data karyawan AKTIF.");
            gotoXY(2, 4);
            printf("Tekan sembarang tombol untuk kembali...");
            _getch();
            return;
        }

        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows, "Daftar Data Karyawan (AKTIF)");

        /* center tabel data karyawan */
        int tableWidth = 118;
        int left = (g_cols - tableWidth) / 2;
        if (left < 2) left = 2;

        int headerRow = 3;

        gotoXY((short)left, (short)headerRow);
        printf("+----+------------+----------------------+--------------------------+------------+-----+------------+----------------+");
        gotoXY((short)left, (short)(headerRow + 1));
        printf("| No | ID         | Nama                 | Email                    | Tgl Lahir  | JK  | No. Telp   | Jabatan        |");
        gotoXY((short)left, (short)(headerRow + 2));
        printf("+----+------------+----------------------+--------------------------+------------+-----+------------+----------------+");

        int dataTop = headerRow + 3;
        int availableRows = g_rows - dataTop - 6;
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
            Karyawan *k = &g_karyawan[idx];

            gotoXY((short)left, (short)row);
            printf("| %2d | %-10s | %-20s | %-24s | %-10s | %-3c | %-10s | %-14s |",
                   i + 1,
                   k->id,
                   k->nama,
                   k->email,
                   k->tanggalLahir,
                   k->jenisKelamin,
                   k->noTelp,
                   k->jabatan);
            row++;
        }

        gotoXY((short)left, (short)row);
        printf("+----+------------+----------------------+--------------------------+------------+-----+------------+----------------+");

        gotoXY(2, (short)(row + 1));
        printf("Total karyawan AKTIF: %d   Baris per halaman: %d", total, availableRows);
        gotoXY(2, (short)(row + 2));
        printf("Halaman %d dari %d", page + 1, totalPages);

        gotoXY(2, (short)(row + 3));
        printf("N/P atau Panah Kanan/Kiri: pindah halaman  |  E: Edit/Hapus  |  ESC: Kembali");

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int code = _getch();
            if (code == 77) {
                if (page < totalPages - 1) page++;
            } else if (code == 75) {
                if (page > 0) page--;
            }
        } else if (ch == 'n' || ch == 'N') {
            if (page < totalPages - 1) page++;
        } else if (ch == 'p' || ch == 'P') {
            if (page > 0) page--;
        } else if (ch == 'e' || ch == 'E') {
            char buf[16];
            gotoXY(2, (short)(row + 5));
            printf("Masukkan No (1-%d) yang akan diubah/hapus (ESC batal): ", total);
            if (!inputText(buf, sizeof(buf))) {
                continue;
            }
            int no = atoi(buf);
            if (no < 1 || no > total) {
                gotoXY(2, (short)(row + 6));
                printf("Nomor tidak valid. Tekan sembarang tombol...");
                _getch();
                continue;
            }

            int idx = idxList[no - 1];

            int doneSub = 0;
            while (!doneSub) {
                ui_updateSize();
                clearScreen();
                drawBox(0, 0, g_cols, g_rows, "Detail Data Karyawan");

                Karyawan *k = &g_karyawan[idx];

                gotoXY(4, 2);
                printf("No       : %d", no);
                gotoXY(4, 3);
                printf("ID       : %s", k->id);
                gotoXY(4, 4);
                printf("Nama     : %s", k->nama);
                gotoXY(4, 5);
                printf("Email    : %s", k->email);
                gotoXY(4, 6);
                printf("Tgl Lahir: %s", k->tanggalLahir);
                gotoXY(4, 7);
                printf("JK       : %c", k->jenisKelamin);
                gotoXY(4, 8);
                printf("No. Telp : %s", k->noTelp);
                gotoXY(4, 9);
                printf("Jabatan  : %s", k->jabatan);
                gotoXY(4, 10);
                printf("Status   : %s", k->active ? "AKTIF" : "NONAKTIF");

                gotoXY(4, 12);
                printf("[1] Ubah Nama");
                gotoXY(4, 13);
                printf("[2] Ubah Email");
                gotoXY(4, 14);
                printf("[3] Ubah Tanggal Lahir");
                gotoXY(4, 15);
                printf("[4] Ubah Jenis Kelamin");
                gotoXY(4, 16);
                printf("[5] Ubah No. Telp");
                gotoXY(4, 17);
                printf("[6] Ubah Jabatan");
                gotoXY(4, 18);
                printf("[7] Hapus / Nonaktifkan Data");
                gotoXY(4, 19);
                printf("[0] Kembali ke Tabel");

                gotoXY(4, 21);
                printf("Pilih menu: ");

                int ch2 = _getch();
                if (ch2 == '0' || ch2 == 27) {
                    doneSub = 1;
                } else if (ch2 == '1') {
                    gotoXY(4, 23);
                    printf("Nama baru (ESC batal): ");
                    if (inputText(k->nama, sizeof(k->nama))) {
                        karyawan_update(idx, k);
                    }
                } else if (ch2 == '2') {
                    gotoXY(4, 23);
                    printf("Email baru (ESC batal): ");
                    if (inputText(k->email, sizeof(k->email))) {
                        karyawan_update(idx, k);
                    }
                } else if (ch2 == '3') {
                    gotoXY(4, 23);
                    printf("Tgl lahir baru YYYY-MM-DD (ESC batal): ");
                    if (inputText(k->tanggalLahir, sizeof(k->tanggalLahir))) {
                        karyawan_update(idx, k);
                    }
                } else if (ch2 == '4') {
                    gotoXY(4, 23);
                    printf("Jenis kelamin baru (L/P, ESC batal): ");
                    char tmp[4] = {0};
                    if (inputText(tmp, sizeof(tmp))) {
                        k->jenisKelamin = (tmp[0] == 'p' || tmp[0] == 'P') ? 'P' : 'L';
                        karyawan_update(idx, k);
                    }
                } else if (ch2 == '5') {
                    gotoXY(4, 23);
                    printf("No. telp baru (ESC batal): ");
                    if (inputText(k->noTelp, sizeof(k->noTelp))) {
                        karyawan_update(idx, k);
                    }
                } else if (ch2 == '6') {
                    gotoXY(4, 23);
                    printf("Jabatan baru (ESC batal): ");
                    if (inputText(k->jabatan, sizeof(k->jabatan))) {
                        karyawan_update(idx, k);
                    }
                } else if (ch2 == '7') {
                    gotoXY(4, 23);
                    printf("Yakin hapus/nonaktifkan karyawan '%s'? (Y/N): ", k->id);
                    int c = _getch();
                    if (c == 'y' || c == 'Y') {
                        karyawan_softDelete(idx);
                        gotoXY(4, 25);
                        printf("Data karyawan dinonaktifkan. Tekan sembarang tombol...");
                        _getch();
                        doneSub = 1;
                    }
                }
            }
        } else if (ch == 27) {
            return;
        }
    }
}

/* MENU UTAMA CRUD Data Karyawan (C + R/U/D) */
static void menuKelolaDataKaryawan() {
    int selected = 0;
    const char *items[] = {
        "Tambah Data Karyawan",
        "Lihat / Ubah / Hapus Data",
        "Kembali"
    };
    int itemCount = 3;

    while (1) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows, "Kelola Data Karyawan");

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
            if      (selected == 0) dataKaryawan_tambah();
            else if (selected == 1) dataKaryawan_lihat();   /* R/U/D */
            else return;
        }
    }
}

#endif /* CRUD_KARYAWAN_H */