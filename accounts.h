#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <stdio.h>
#include <string.h>

#define MAX_INPUT      64
#define MAX_EMPLOYEES  100

/* Nama file tempat menyimpan data akun karyawan (biner) */
#define ACCOUNTS_FILE  "accounts.dat"

/* ----- ENUM ROLE KARYAWAN ----- */
typedef enum {
    ROLE_SUPERADMIN = 0,     /* biasanya untuk superadmin (hardcode, tidak wajib disimpan di file) */
    ROLE_PEMBAYARAN,         /* karyawan pengurus pembayaran */
    ROLE_JADWAL,             /* karyawan pengurus jadwal */
    ROLE_DATA,               /* karyawan pengurus data */
    ROLE_MANAGER             /* karyawan manager */
} Role;

/* ----- STRUCT DATA KARYAWAN ----- */
typedef struct {
    char username[MAX_INPUT]; /* username untuk login */
    char password[MAX_INPUT]; /* password untuk login */
    Role role;                /* peran karyawan */
    int  active;              /* 1 = aktif, 0 = nonaktif (soft delete) */
} Employee;

/* ----- ARRAY GLOBAL PENYIMPAN SEMENTARA DI MEMORI ----- */
static Employee g_employees[MAX_EMPLOYEES];  /* menampung semua akun yang sudah di-load */
static int g_employeeCount = 0;              /* jumlah akun yang terisi di array */

/* ===================================================================== */
/*                  HELPER ROLE & AKUN (TIDAK MENYENTUH FILE)            */
/* ===================================================================== */

/* Mengubah enum Role menjadi string untuk ditampilkan di UI */
static const char *roleToString(Role r) {
    switch (r) {
        case ROLE_SUPERADMIN:  return "Superadmin";
        case ROLE_PEMBAYARAN:  return "Pengurus Pembayaran";
        case ROLE_JADWAL:      return "Pengurus Jadwal";
        case ROLE_DATA:        return "Pengurus Data";
        case ROLE_MANAGER:     return "Karyawan Manager";
        default:               return "Unknown";
    }
}

/* Mencari index karyawan di array berdasarkan username (hanya yang aktif)
   return:
     - index (0..g_employeeCount-1) jika ditemukan
     - -1 jika tidak ditemukan
*/
static int findEmployeeByUsername(const char *username) {
    for (int i = 0; i < g_employeeCount; ++i) {
        if (g_employees[i].active &&
            strcmp(g_employees[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

/* ===================================================================== */
/*                        BAGIAN FILE PROCESSING (.dat)                  */
/* ===================================================================== */

/* Membaca (load) semua akun dari file biner ke array g_employees */
static void accounts_loadFromFile() {
    FILE *f = fopen(ACCOUNTS_FILE, "rb");
    if (!f) {
        /* Jika file belum ada, anggap belum ada akun karyawan */
        g_employeeCount = 0;
        return;
    }

    g_employeeCount = 0;
    while (g_employeeCount < MAX_EMPLOYEES) {
        Employee e;
        size_t n = fread(&e, sizeof(Employee), 1, f);
        if (n != 1) {
            break; /* EOF atau error baca */
        }
        g_employees[g_employeeCount++] = e;
    }

    fclose(f);
}

/* Menyimpan (save) semua akun dari array g_employees ke file biner .dat
   – setiap kali ada perubahan data (tambah/ubah/hapus) kita panggil fungsi ini
*/
static void accounts_saveToFile() {
    FILE *f = fopen(ACCOUNTS_FILE, "wb");
    if (!f) {
        /* Untuk kesederhanaan: jika gagal menyimpan, kita hanya keluar dari fungsi. */
        return;
    }

    if (g_employeeCount > 0) {
        fwrite(g_employees, sizeof(Employee), g_employeeCount, f);
    }

    fclose(f);
}

/* Dipanggil SEKALI saat awal program (misalnya di ui_run())
   untuk meload semua akun karyawan dari file .dat. */
static void accounts_init() {
    accounts_loadFromFile();
}

/* ===================================================================== */
/*              FUNGSI CRUD LEVEL DATA (ARRAY + FILE)                    */
/* ===================================================================== */

/* CREATE:
   Menambahkan akun baru ke array DAN langsung simpan ke file biner.
   Validasi:
   - kapasitas belum penuh
   - username belum dipakai
*/
static int accounts_add(const char *username,
                        const char *password,
                        Role role) {
    if (g_employeeCount >= MAX_EMPLOYEES) return 0;
    if (findEmployeeByUsername(username) >= 0) return 0;

    Employee e;
    strncpy(e.username, username, MAX_INPUT);
    e.username[MAX_INPUT - 1] = '\0';

    strncpy(e.password, password, MAX_INPUT);
    e.password[MAX_INPUT - 1] = '\0';

    e.role   = role;
    e.active = 1;    /* akun baru = aktif */

    g_employees[g_employeeCount++] = e;

    /* setiap kali ada perubahan, simpan ke file .dat */
    accounts_saveToFile();
    return 1;
}

/* UPDATE:
   Meng-update data akun pada index tertentu, lalu simpan seluruh array ke file .dat.
   (Misalnya ketika ubah password atau role.)
*/
static void accounts_update(int index, const Employee *updated) {
    if (index < 0 || index >= g_employeeCount) return;
    g_employees[index] = *updated;
    accounts_saveToFile();
}

/* DELETE (soft delete):
   Menandai akun sebagai NONAKTIF (active = 0), lalu menyimpan ke file .dat.
   (Login hanya mengizinkan akun dengan active == 1.)
*/
static void accounts_softDelete(int index) {
    if (index < 0 || index >= g_employeeCount) return;
    g_employees[index].active = 0;
    accounts_saveToFile();
}
/* ===================================================================== */
/*                   SEED: 50 DATA DUMMY KARYAWAN                        */
/* ===================================================================== */
/* Dipakai kalau mau isi awal accounts.dat dengan data contoh.
   Pola:
     username : emp01 .. emp50
     password : pw01 .. pw50
     role     : di-rotasi (Pembayaran, Jadwal, Data, Manager)
*/
static void accounts_seedDummyDataIfEmpty() {
    /* Kalau sudah ada data di memori, jangan seed lagi
       supaya tidak dobel-dobel */
    if (g_employeeCount > 0) return;

    /* pola role untuk dirotasi */
    const Role rolePattern[] = {
        ROLE_PEMBAYARAN,
        ROLE_JADWAL,
        ROLE_DATA,
        ROLE_MANAGER
    };
    int roleCount = 4;

    char uname[32];
    char pass[32];

    for (int i = 1; i <= 50 && g_employeeCount < MAX_EMPLOYEES; ++i) {
        /* username: emp01, emp02, ..., emp50 */
        snprintf(uname, sizeof(uname), "emp%02d", i);
        /* password: pw01, pw02, ..., pw50 */
        snprintf(pass, sizeof(pass), "pw%02d", i);

        /* rotasi role: 1: pembayaran, 2: jadwal, 3: data, 4: manager, lalu ulang */
        Role r = rolePattern[(i - 1) % roleCount];

        /* accounts_add akan set active = 1 dan langsung simpan ke file .dat */
        accounts_add(uname, pass, r);
    }
}

#endif /* ACCOUNTS_H */
