#ifndef MENU_UI_H
#define MENU_UI_H

#include "console_ui.h"
#include "../accounts.h"

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
static void menuLihatKaryawan() {
    ui_updateSize();
    clearScreen();
    drawBox(0, 0, g_cols, g_rows, "Daftar Akun Karyawan");

    gotoXY(2, 2);
    if (g_employeeCount == 0) {
        printf("Belum ada akun karyawan.");
        gotoXY(2, 4);
        printf("Tekan sembarang tombol untuk kembali...");
        _getch();
        return;
    }

    int row = 4;
    gotoXY(2, 2);
    printf("Total karyawan (termasuk nonaktif): %d", g_employeeCount);

    /* Loop semua akun, tampilkan username, status, dan role */
    for (int i = 0; i < g_employeeCount; ++i) {
        Employee *e = &g_employees[i];
        gotoXY(2, (short)row);
        printf("%2d. %-20s | %-9s | %s",
               i + 1,
               e->username,
               e->active ? "AKTIF" : "NONAKTIF",
               roleToString(e->role));
        row++;
        if (row >= g_rows - 2) break;   /* jika layar penuh, break */
    }

    gotoXY(2, (short)(g_rows - 2));
    printf("Tekan sembarang tombol untuk kembali...");
    _getch();
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
static void mainMenuSuperadmin() {
    int selected = 0;
    const char *items[] = {
        "Kelola Akun Karyawan",
        "Laporan Sistem (simulasi)",
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
                clearScreen();
                drawBox(0, 0, g_cols, g_rows, "Laporan Sistem");
                gotoXY(2, 2);
                printf("Laporan Sistem masih simulasi.");
                gotoXY(2, 4);
                printf("Tekan sembarang tombol untuk kembali...");
                _getch();
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

#endif /* MENU_UI_H */
