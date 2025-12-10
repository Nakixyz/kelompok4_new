#ifndef MENU_UI_H
#define MENU_UI_H

#include "console_ui.h"
#include "../accounts.h"
#include "../karyawan.h"

/* forward declaration supaya boleh dipanggil sebelum didefinisikan */
static void menuKelolaDataKaryawan(void);

/* ---------- Menu: Kelola Akun Karyawan (Superadmin) ---------- */

/* CREATE: menambah akun karyawan baru melalui UI */
static void menuTambahKaryawan() {
    ui_updateSize();
    clearScreen();
    drawBox(0, 0, g_cols, g_rows, "Tambah Akun Karyawan");

    /* Cek kapasitas array */
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

    /* Cek apakah username sudah dipakai */
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

    /* Konversi pilihan angka ke enum Role */
    Role role;
    if (roleChoice == 1) role = ROLE_PEMBAYARAN;
    else if (roleChoice == 2) role = ROLE_JADWAL;
    else if (roleChoice == 3) role = ROLE_DATA;
    else role = ROLE_MANAGER;

    /* Tambah ke array + simpan ke file */
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

/* UPDATE: mengubah password / role karyawan yang sudah ada */
static void menuUbahKaryawan() {
    ui_updateSize();
    clearScreen();
    drawBox(0, 0, g_cols, g_rows, "Ubah Akun Karyawan");

    if (g_employeeCount == 0) {
        gotoXY(2, 2);
        printf("Belum ada akun karyawan.");
        gotoXY(2, 4);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    char username[MAX_INPUT];
    gotoXY(2, 2);
    printf("Masukkan username karyawan yang akan diubah (ESC untuk batal): ");
    if (!inputText(username, MAX_INPUT)) return;   /* ESC -> batal */

    /* Cari index akun berdasarkan username */
    int idx = findEmployeeByUsername(username);
    if (idx < 0) {
        gotoXY(2, 4);
        printf("Akun dengan username '%s' tidak ditemukan / tidak aktif.", username);
        gotoXY(2, 6);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    Employee e = g_employees[idx];  /* salin ke variabel lokal untuk diedit */

    int done = 0;
    while (!done) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows, "Ubah Akun Karyawan");

        /* Tampilkan info akun yang sedang diubah */
        gotoXY(2, 2);
        printf("Username : %s", e.username);
        gotoXY(2, 3);
        printf("Role     : %s", roleToString(e.role));
        gotoXY(2, 4);
        printf("Status   : %s", e.active ? "AKTIF" : "NONAKTIF");

        /* Menu pilihan perubahan */
        gotoXY(2, 6);
        printf("[1] Ubah Password");
        gotoXY(2, 7);
        printf("[2] Ubah Role");
        gotoXY(2, 8);
        printf("[0] Kembali");

        gotoXY(2, 10);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '0' || ch == 27) {
            /* kembali */
            done = 1;
        } else if (ch == '1') {
            /* Ubah password */
            char newPass[MAX_INPUT];
            gotoXY(2, 12);
            printf("Masukkan password baru (ESC batal): ");
            if (inputPassword(newPass, MAX_INPUT)) {
                strncpy(e.password, newPass, MAX_INPUT);
                e.password[MAX_INPUT - 1] = '\0';
                /* Simpan perubahan ke array + file */
                accounts_update(idx, &e);
                gotoXY(2, 14);
                printf("Password berhasil diubah.");
                _getch();
            }
        } else if (ch == '2') {
            /* Ubah role */
            gotoXY(2, 12);
            printf("Pilih role baru:");
            gotoXY(4, 13);
            printf("[1] Pengurus Pembayaran");
            gotoXY(4, 14);
            printf("[2] Pengurus Jadwal");
            gotoXY(4, 15);
            printf("[3] Pengurus Data");
            gotoXY(4, 16);
            printf("[4] Karyawan Manager");
            gotoXY(2, 18);
            printf("Masukkan pilihan (1-4, ESC batal): ");

            int rch = _getch();
            if (rch == 27) continue;   /* batal perubahan role */
            if (rch >= '1' && rch <= '4') {
                int rc = rch - '0';
                if (rc == 1) e.role = ROLE_PEMBAYARAN;
                else if (rc == 2) e.role = ROLE_JADWAL;
                else if (rc == 3) e.role = ROLE_DATA;
                else e.role = ROLE_MANAGER;

                /* Simpan perubahan role */
                accounts_update(idx, &e);
                gotoXY(2, 20);
                printf("Role berhasil diubah menjadi '%s'.", roleToString(e.role));
                _getch();
            }
        }
    }
}

/* DELETE (soft): menonaktifkan akun karyawan (active = 0) */
static void menuHapusKaryawan() {
    ui_updateSize();
    clearScreen();
    drawBox(0, 0, g_cols, g_rows, "Hapus / Nonaktifkan Akun Karyawan");

    if (g_employeeCount == 0) {
        gotoXY(2, 2);
        printf("Belum ada akun karyawan.");
        gotoXY(2, 4);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    char username[MAX_INPUT];
    gotoXY(2, 2);
    printf("Masukkan username karyawan yang akan dihapus (ESC untuk batal): ");
    if (!inputText(username, MAX_INPUT)) return;   /* ESC -> batal */

    int idx = findEmployeeByUsername(username);
    if (idx < 0) {
        gotoXY(2, 4);
        printf("Akun dengan username '%s' tidak ditemukan / sudah nonaktif.", username);
        gotoXY(2, 6);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    Employee *e = &g_employees[idx];

    gotoXY(2, 4);
    printf("Yakin ingin menonaktifkan akun '%s' (role: %s)?",
           e->username, roleToString(e->role));
    gotoXY(2, 6);
    printf("Tekan Y untuk set NONAKTIF, tombol lain untuk batal.");

    int ch = _getch();
    if (ch == 'y' || ch == 'Y') {
        /* soft delete -> active = 0 + save ke file */
        accounts_softDelete(idx);
        gotoXY(2, 8);
        printf("Akun telah dinonaktifkan.");
        gotoXY(2, 10);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
    }
}

/* READ: menampilkan daftar akun karyawan dari array (data sudah hasil load file) */
/* READ: menampilkan daftar akun karyawan dalam bentuk tabel */
/* READ: menampilkan daftar akun karyawan dalam bentuk tabel fullscreen + paging */
/* READ: menampilkan daftar akun karyawan AKTIF dalam bentuk tabel fullscreen + paging */
static void menuLihatKaryawan() {
    /* kalau masih kosong, isi dummy dulu (kalau kamu pakai fitur ini) */
    accounts_seedDummyDataIfEmpty();

    /* Kumpulkan hanya akun yang active == 1 */
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

    int page = 0;

    while (1) {
        ui_updateSize();
        clearScreen();

        /* frame luar fullscreen */
        drawBox(0, 0, g_cols, g_rows, "Daftar Akun Karyawan (AKTIF)");

        int tableLeft = 2;
        int headerRow = 3;

        /* header tabel */
        gotoXY((short)tableLeft, (short)headerRow);
        printf("+----+----------------------+--------------------------+-----------+");
        gotoXY((short)tableLeft, (short)(headerRow + 1));
        printf("| No | Username             | Role                     | Status    |");
        gotoXY((short)tableLeft, (short)(headerRow + 2));
        printf("+----+----------------------+--------------------------+-----------+");

        int dataTop = headerRow + 3;

        /* berapa baris data per halaman? */
        int availableRows = g_rows - dataTop - 5;
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
            /* i+1 = nomor urut global berdasar data aktif */
            printf("| %2d | %-20s | %-24s | %-9s |",
                   i + 1,
                   e->username,
                   roleToString(e->role),
                   e->active ? "AKTIF" : "NONAKTIF");
            row++;
        }

        /* garis penutup tabel */
        gotoXY((short)tableLeft, (short)row);
        printf("+----+----------------------+--------------------------+-----------+");

        gotoXY(2, (short)(row + 1));
        printf("Total akun AKTIF: %d   Baris per halaman: %d", total, availableRows);
        gotoXY(2, (short)(row + 2));
        printf("Halaman %d dari %d", page + 1, totalPages);

        gotoXY(2, (short)(g_rows - 2));
        printf("N: Next   P: Prev   ESC: Kembali");

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int code = _getch();
            if (code == 77) {      /* Right arrow -> next */
                if (page < totalPages - 1) page++;
            } else if (code == 75) { /* Left arrow -> prev */
                if (page > 0) page--;
            }
        } else if (ch == 'n' || ch == 'N') {
            if (page < totalPages - 1) page++;
        } else if (ch == 'p' || ch == 'P') {
            if (page > 0) page--;
        } else if (ch == 27) { /* ESC */
            return;
        }
    }
}




/* MENU UTAMA CRUD: mengelola akun karyawan (C, R, U, D) */
static void menuKelolaAkunKaryawan() {
    int selected = 0;
    const char *items[] = {
        "Tambah Akun Karyawan",          /* C */
        "Ubah Akun Karyawan",            /* U */
        "Hapus / Nonaktifkan Akun",      /* D (soft) */
        "Lihat Akun Karyawan",           /* R */
        "Kembali"
    };
    int itemCount = 5;

    while (1) {
        ui_updateSize();
        clearScreen();

        drawBox(0, 0, g_cols, g_rows, "Kelola Akun Karyawan");

        int boxW = 60;
        int boxH = 11;
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
            if (code == 72) {           /* Up */
                selected = (selected - 1 + itemCount) % itemCount;
            } else if (code == 80) {    /* Down */
                selected = (selected + 1) % itemCount;
            }
        } else if (ch == 27) {
            /* ESC -> kembali ke menu superadmin */
            return;
        } else if (ch == 13) {
            /* ENTER -> eksekusi pilihan */
            if (selected == 0) {
                menuTambahKaryawan();       /* CREATE */
            } else if (selected == 1) {
                menuUbahKaryawan();         /* UPDATE */
            } else if (selected == 2) {
                menuHapusKaryawan();        /* DELETE (soft) */
            } else if (selected == 3) {
                menuLihatKaryawan();        /* READ */
            } else {
                return;                     /* Kembali */
            }
        }
    }
}

/* ---------- Menu Karyawan per Role (simulasi) ---------- */

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

static void menuKaryawanData(const char *username) {
    while (1) {
        ui_updateSize();
        clearScreen();
        char title[64];
        snprintf(title, sizeof(title), "Menu - Data (%s)", username);
        drawBox(0, 0, g_cols, g_rows, title);

        gotoXY(2, 2);
        printf("[1] Tambah Data (simulasi)");
        gotoXY(2, 3);
        printf("[2] Ubah Data (simulasi)");
        gotoXY(2, 4);
        printf("[3] Hapus Data (simulasi)");
        gotoXY(2, 5);
        printf("[4] Lihat Data (simulasi)");
        gotoXY(2, 7);
        printf("[0] Logout");

        gotoXY(2, 9);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '0' || ch == 27) return;
        else {
            gotoXY(2, 11);
            printf("Fitur belum diimplementasikan. Tekan sembarang tombol...");
            _getch();
        }
    }
}

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

/* ---------- Menu Utama Superadmin ---------- */

/* ---------- Menu Utama Superadmin (menu di panel kiri) ---------- */
/* ---------- Menu Utama Superadmin (menu kecil di kiri + ASCII art) ---------- */
/* ---------- Menu Utama Superadmin (layout mirip login) ---------- */
/* ---------- Menu Utama Superadmin (menu terpusat dengan kotak) ---------- */
/* ---------- Menu Utama Superadmin (tanpa panel kiri, menu di tengah) ---------- */
/* ---------- Menu Utama Superadmin (tanpa panel kiri, menu di tengah) ---------- */
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

        /* ===================== FRAME LUAR + HEADER ASCII ===================== */
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

        /* judul di bawah header */
        const char *panelTitle = "Menu Utama - Superadmin";
        int panelTitleX = outerLeft + (outerWidth - (int)strlen(panelTitle)) / 2;
        if (panelTitleX < outerLeft + 2) panelTitleX = outerLeft + 2;
        gotoXY((short)panelTitleX, (short)baseTop);
        printf("%s", panelTitle);

        /* ===================== AREA KONTEN UNTUK MENU ===================== */
        int usableLeft   = outerLeft + 1;
        int usableTop    = baseTop + 2;
        int usableWidth  = outerWidth - 2;
        int usableHeight = outerHeight - usableTop - 2;
        if (usableWidth < 20)  usableWidth = 20;
        if (usableHeight < 10) usableHeight = 10;

        /* ---------- KOTAK MENU DI TENGAH ---------- */
        int menuBoxW = 48;
        int menuBoxH = 9;

        if (menuBoxW > usableWidth - 2)  menuBoxW = usableWidth - 2;
        if (menuBoxH > usableHeight - 2) menuBoxH = usableHeight - 2;

        int menuBoxLeft = usableLeft + (usableWidth  - menuBoxW) / 2;
        int menuBoxTop  = usableTop  + (usableHeight - menuBoxH) / 2;

        drawBox(menuBoxLeft, menuBoxTop, menuBoxW, menuBoxH, "");

        /* isi kotak menu */
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

        /* petunjuk di baris bawah luar */
        gotoXY((short)(outerLeft + 2), (short)(outerTop + outerHeight - 2));
        printf("Panah atas/bawah: pindah   ENTER: pilih   ESC: logout");

        /* ===================== INPUT NAVIGASI ===================== */
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int code = _getch();
            if (code == 72) {           /* Up */
                selected = (selected - 1 + itemCount) % itemCount;
            } else if (code == 80) {    /* Down */
                selected = (selected + 1) % itemCount;
            }
        } else if (ch == 27) {
            /* ESC -> logout */
            return;
        } else if (ch == 13) {
            /* ENTER -> eksekusi menu */
            if (selected == 0) {
                menuKelolaAkunKaryawan();
            } else if (selected == 1) {
                menuKelolaDataKaryawan();
            } else {
                return; /* Logout */
            }
        }
    }
}




/* ---------- Menu Utama Karyawan (router per role) ---------- */

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

/* ---------- Menu Pilih Jenis Login ---------- */

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
            if (code == 72) {           /* Up */
                selected = (selected - 1 + itemCount) % itemCount;
            } else if (code == 80) {    /* Down */
                selected = (selected + 1) % itemCount;
            }
        } else if (ch == 13) {
            return selected;  /* 0=superadmin,1=karyawan,2=keluar */
        } else if (ch == 27) {
            return 2;         /* treat ESC as Keluar */
        }
    }
}
/* ---------- CRUD DATA KARYAWAN (DETAIL) ---------- */

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

static void dataKaryawan_ubah() {
    ui_updateSize();
    clearScreen();
    drawBox(0, 0, g_cols, g_rows, "Ubah Data Karyawan");

    if (g_karyawanCount == 0) {
        gotoXY(2, 2);
        printf("Belum ada data karyawan.");
        gotoXY(2, 4);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    char id[11];
    gotoXY(2, 2);
    printf("Masukkan ID Karyawan yang akan diubah (ESC batal): ");
    if (!inputText(id, sizeof(id))) return;

    int idx = karyawan_findById(id);
    if (idx < 0) {
        gotoXY(2, 4);
        printf("Data dengan ID '%s' tidak ditemukan / nonaktif.", id);
        gotoXY(2, 6);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    Karyawan k = g_karyawan[idx];

    int done = 0;
    while (!done) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows, "Ubah Data Karyawan");

        gotoXY(2, 2);
        printf("ID          : %s", k.id);
        gotoXY(2, 3);
        printf("Nama        : %s", k.nama);
        gotoXY(2, 4);
        printf("Email       : %s", k.email);
        gotoXY(2, 5);
        printf("Tgl Lahir   : %s", k.tanggalLahir);
        gotoXY(2, 6);
        printf("Jenis Kelamin : %c", k.jenisKelamin);
        gotoXY(2, 7);
        printf("No. Telp    : %s", k.noTelp);
        gotoXY(2, 8);
        printf("Jabatan     : %s", k.jabatan);
        gotoXY(2, 9);
        printf("Status      : %s", k.active ? "AKTIF" : "NONAKTIF");

        gotoXY(2, 11);
        printf("[1] Ubah Nama");
        gotoXY(2, 12);
        printf("[2] Ubah Email");
        gotoXY(2, 13);
        printf("[3] Ubah Tanggal Lahir");
        gotoXY(2, 14);
        printf("[4] Ubah Jenis Kelamin");
        gotoXY(2, 15);
        printf("[5] Ubah No. Telp");
        gotoXY(2, 16);
        printf("[6] Ubah Jabatan");
        gotoXY(2, 17);
        printf("[0] Kembali");

        gotoXY(2, 19);
        printf("Pilih menu: ");

        int ch = _getch();
        if (ch == '0' || ch == 27) {
            done = 1;
        } else if (ch == '1') {
            gotoXY(2, 21);
            printf("Nama baru (ESC batal): ");
            if (inputText(k.nama, sizeof(k.nama))) {
                karyawan_update(idx, &k);
            }
        } else if (ch == '2') {
            gotoXY(2, 21);
            printf("Email baru (ESC batal): ");
            if (inputText(k.email, sizeof(k.email))) {
                karyawan_update(idx, &k);
            }
        } else if (ch == '3') {
            gotoXY(2, 21);
            printf("Tgl lahir baru YYYY-MM-DD (ESC batal): ");
            if (inputText(k.tanggalLahir, sizeof(k.tanggalLahir))) {
                karyawan_update(idx, &k);
            }
        } else if (ch == '4') {
            gotoXY(2, 21);
            printf("Jenis kelamin baru (L/P, ESC batal): ");
            char buf[4] = {0};
            if (inputText(buf, sizeof(buf))) {
                k.jenisKelamin = (buf[0] == 'p' || buf[0] == 'P') ? 'P' : 'L';
                karyawan_update(idx, &k);
            }
        } else if (ch == '5') {
            gotoXY(2, 21);
            printf("No. telp baru (ESC batal): ");
            if (inputText(k.noTelp, sizeof(k.noTelp))) {
                karyawan_update(idx, &k);
            }
        } else if (ch == '6') {
            gotoXY(2, 21);
            printf("Jabatan baru (ESC batal): ");
            if (inputText(k.jabatan, sizeof(k.jabatan))) {
                karyawan_update(idx, &k);
            }
        }
    }
}

static void dataKaryawan_hapus() {
    ui_updateSize();
    clearScreen();
    drawBox(0, 0, g_cols, g_rows, "Hapus / Nonaktifkan Data Karyawan");

    if (g_karyawanCount == 0) {
        gotoXY(2, 2);
        printf("Belum ada data karyawan.");
        gotoXY(2, 4);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    char id[11];
    gotoXY(2, 2);
    printf("Masukkan ID Karyawan yang akan dihapus (ESC batal): ");
    if (!inputText(id, sizeof(id))) return;

    int idx = karyawan_findById(id);
    if (idx < 0) {
        gotoXY(2, 4);
        printf("Data dengan ID '%s' tidak ditemukan / sudah nonaktif.", id);
        gotoXY(2, 6);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    Karyawan *k = &g_karyawan[idx];

    gotoXY(2, 4);
    printf("Yakin ingin menonaktifkan karyawan '%s' (%s)?",
           k->id, k->nama);
    gotoXY(2, 6);
    printf("Tekan Y untuk konfirmasi, tombol lain untuk batal.");

    int ch = _getch();
    if (ch == 'y' || ch == 'Y') {
        karyawan_softDelete(idx);
        gotoXY(2, 8);
        printf("Data karyawan telah dinonaktifkan.");
        gotoXY(2, 10);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
    }
}

/* Lihat data karyawan AKTIF dalam bentuk tabel fullscreen + paging */
static void dataKaryawan_lihat() {
    /* kumpulkan hanya yang active == 1 */
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

    int page = 0;

    while (1) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows, "Daftar Data Karyawan (AKTIF)");

        int left = 2;
        int headerRow = 3;

        /* header tabel (sesuai struktur ERD) */
        gotoXY((short)left, (short)headerRow);
        printf("+----+------------+----------------------+--------------------------+------------+-----+------------+----------------+");
        gotoXY((short)left, (short)(headerRow + 1));
        printf("| No | ID         | Nama                 | Email                    | Tgl Lahir  | JK  | No. Telp   | Jabatan        |");
        gotoXY((short)left, (short)(headerRow + 2));
        printf("+----+------------+----------------------+--------------------------+------------+-----+------------+----------------+");

        int dataTop = headerRow + 3;
        int availableRows = g_rows - dataTop - 5;
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

        gotoXY(2, (short)(g_rows - 2));
        printf("N: Next   P: Prev   ESC: Kembali");

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int code = _getch();
            if (code == 77) { /* Right arrow */
                if (page < totalPages - 1) page++;
            } else if (code == 75) { /* Left arrow */
                if (page > 0) page--;
            }
        } else if (ch == 'n' || ch == 'N') {
            if (page < totalPages - 1) page++;
        } else if (ch == 'p' || ch == 'P') {
            if (page > 0) page--;
        } else if (ch == 27) {
            return;
        }
    }
}


static void menuKelolaDataKaryawan() {
    int selected = 0;
    const char *items[] = {
        "Tambah Data Karyawan",
        "Ubah Data Karyawan",
        "Hapus / Nonaktifkan Data",
        "Lihat Data Karyawan",
        "Kembali"
    };
    int itemCount = 5;

    while (1) {
        ui_updateSize();
        clearScreen();
        drawBox(0, 0, g_cols, g_rows, "Kelola Data Karyawan");

        int boxW = 60;
        int boxH = 11;
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
            if (code == 72)       selected = (selected - 1 + itemCount) % itemCount;
            else if (code == 80)  selected = (selected + 1) % itemCount;
        } else if (ch == 27) {
            return;
        } else if (ch == 13) {
            if      (selected == 0) dataKaryawan_tambah();
            else if (selected == 1) dataKaryawan_ubah();
            else if (selected == 2) dataKaryawan_hapus();
            else if (selected == 3) dataKaryawan_lihat();
            else return;
        }
    }
}


#endif /* MENU_UI_H */
