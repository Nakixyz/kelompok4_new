#ifndef KARYAWAN_H
#define KARYAWAN_H

#include <stdio.h>
#include <string.h>

#define MAX_KARYAWAN   200
#define KARYAWAN_FILE  "karyawan.dat"

/* Struktur detail karyawan (mirip tabel ERD) */
typedef struct {
    char id[11];            /* id_karyawan      VARCHAR(10)  */
    char nama[51];          /* nama_karyawan    VARCHAR(50)  */
    char email[101];        /* email            VARCHAR(100) */
    char tanggalLahir[11];  /* tanggal_lahir    "YYYY-MM-DD" */
    char jenisKelamin;      /* jenis_kelamin    'L' / 'P'    */
    char noTelp[11];        /* no_telp          VARCHAR(10)  */
    char jabatan[21];       /* jabatan          VARCHAR(20)  */
    int  active;            /* 1 = aktif, 0 = nonaktif */
} Karyawan;

/* array global di memori */
static Karyawan g_karyawan[MAX_KARYAWAN];
static int g_karyawanCount = 0;

/* ----- helper cari by id (hanya yang aktif) ----- */
static int karyawan_findById(const char *id) {
    for (int i = 0; i < g_karyawanCount; ++i) {
        if (g_karyawan[i].active &&
            strcmp(g_karyawan[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

/* ---------------- FILE PROCESSING .dat ---------------- */

static void karyawan_loadFromFile() {
    FILE *f = fopen(KARYAWAN_FILE, "rb");
    if (!f) {
        g_karyawanCount = 0;
        return;
    }

    g_karyawanCount = 0;
    while (g_karyawanCount < MAX_KARYAWAN) {
        Karyawan k;
        size_t n = fread(&k, sizeof(Karyawan), 1, f);
        if (n != 1) break;
        g_karyawan[g_karyawanCount++] = k;
    }
    fclose(f);
}

static void karyawan_saveToFile() {
    FILE *f = fopen(KARYAWAN_FILE, "wb");
    if (!f) return;

    if (g_karyawanCount > 0) {
        fwrite(g_karyawan, sizeof(Karyawan), g_karyawanCount, f);
    }
    fclose(f);
}

/* ---------------- CRUD LEVEL DATA ---------------- */

/* CREATE */
static int karyawan_add(const Karyawan *src) {
    if (g_karyawanCount >= MAX_KARYAWAN) return 0;
    if (karyawan_findById(src->id) >= 0) return 0;

    g_karyawan[g_karyawanCount] = *src;
    g_karyawan[g_karyawanCount].active = 1;
    g_karyawanCount++;

    karyawan_saveToFile();
    return 1;
}

/* UPDATE (by index) */
static void karyawan_update(int index, const Karyawan *src) {
    if (index < 0 || index >= g_karyawanCount) return;
    g_karyawan[index] = *src;
    karyawan_saveToFile();
}

/* SOFT DELETE (active = 0) */
static void karyawan_softDelete(int index) {
    if (index < 0 || index >= g_karyawanCount) return;
    g_karyawan[index].active = 0;
    karyawan_saveToFile();
}

/* ===================================================================== */
/*               SEED: 50 DATA DUMMY KA001 .. KA050                      */
/* ===================================================================== */
static void karyawan_seedDummyIfEmpty() {
    /* kalau sudah ada data di file, jangan seed lagi */
    if (g_karyawanCount > 0) return;

    const char *jabatanPattern[] = {
        "Pembayaran",
        "Jadwal",
        "Data",
        "Manager"
    };
    int jabatanCount = 4;

    for (int i = 1; i <= 50 && g_karyawanCount < MAX_KARYAWAN; ++i) {
        Karyawan k;
        memset(&k, 0, sizeof(Karyawan));

        /* ID: KA001 .. KA050 */
        snprintf(k.id, sizeof(k.id), "KA%03d", i);

        /* Nama: Karyawan 01 .. Karyawan 50 */
        snprintf(k.nama, sizeof(k.nama), "Karyawan %02d", i);

        /* Email: karyawan01@example.com */
        snprintf(k.email, sizeof(k.email), "karyawan%02d@example.com", i);

        /* Tanggal lahir: 1990-01-01 .. 1990-01-28, lalu muter */
        int day = ((i - 1) % 28) + 1;
        snprintf(k.tanggalLahir, sizeof(k.tanggalLahir), "1990-01-%02d", day);

        /* Jenis kelamin: L, P, L, P, ... */
        k.jenisKelamin = (i % 2 == 0) ? 'P' : 'L';

        /* No telp: 0811110001 .. 0811110050 (panjang max 10) */
        snprintf(k.noTelp, sizeof(k.noTelp), "811110%03d", i);  /* tanpa 0 depan biar muat 10 digit */
        /* kalau mau dengan 0 depan dan masih muat 10 digit:
           snprintf(k.noTelp, sizeof(k.noTelp), "08%08d", i);
        */

        /* Jabatan dirotasi */
        const char *jab = jabatanPattern[(i - 1) % jabatanCount];
        strncpy(k.jabatan, jab, sizeof(k.jabatan));
        k.jabatan[sizeof(k.jabatan) - 1] = '\0';

        k.active = 1;

        /* tambah ke array + simpan */
        karyawan_add(&k);
    }
}

/* dipanggil saat awal program */
static void karyawan_init() {
    karyawan_loadFromFile();
    /* jika file kosong, isi 50 dummy KA001..KA050 */
    karyawan_seedDummyIfEmpty();
}

#endif /* KARYAWAN_H */
