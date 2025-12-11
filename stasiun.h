#ifndef STASIUN_H
#define STASIUN_H

#include <stdio.h>
#include <string.h>


#define MAX_STASIUN    100
#define STASIUN_FILE   "stasiun.dat"

/* Struktur Data Stasiun */
typedef struct {
    char id[11];            /* ID Stasiun (misal: GMR, BD, YK) */
    char nama[51];          /* Nama Stasiun (Gambir, Bandung) */
    char kota[51];          /* Kota Lokasi */
    int  active;            /* 1 = aktif, 0 = soft deleted */
} Stasiun;

/* Array global di memori */
static Stasiun g_stasiun[MAX_STASIUN];
static int g_stasiunCount = 0;

/* Helper: Cari index stasiun berdasarkan ID (hanya yang aktif) */
static int stasiun_findById(const char *id) {
    for (int i = 0; i < g_stasiunCount; ++i) {
        if (g_stasiun[i].active && strcmp(g_stasiun[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

/* --- FILE PROCESSING (.dat) --- */

static void stasiun_saveToFile() {
    FILE *f = fopen(STASIUN_FILE, "wb");
    if (!f) return;
    if (g_stasiunCount > 0) {
        fwrite(g_stasiun, sizeof(Stasiun), g_stasiunCount, f);
    }
    fclose(f);
}

static void stasiun_loadFromFile() {
    FILE *f = fopen(STASIUN_FILE, "rb");
    if (!f) {
        g_stasiunCount = 0;
        return;
    }
    g_stasiunCount = 0;
    while (g_stasiunCount < MAX_STASIUN) {
        Stasiun s;
        if (fread(&s, sizeof(Stasiun), 1, f) != 1) break;
        g_stasiun[g_stasiunCount++] = s;
    }
    fclose(f);
}

/* --- CRUD OPERATION --- */

/* Create */
static int stasiun_add(const char *id, const char *nama, const char *kota) {
    if (g_stasiunCount >= MAX_STASIUN) return 0; // Penuh
    if (stasiun_findById(id) >= 0) return 0;     // ID Duplikat

    Stasiun s;
    strncpy(s.id, id, sizeof(s.id)-1);   s.id[sizeof(s.id)-1] = '\0';
    strncpy(s.nama, nama, sizeof(s.nama)-1); s.nama[sizeof(s.nama)-1] = '\0';
    strncpy(s.kota, kota, sizeof(s.kota)-1); s.kota[sizeof(s.kota)-1] = '\0';
    s.active = 1;

    g_stasiun[g_stasiunCount++] = s;
    stasiun_saveToFile();
    return 1;
}

/* Update */
static void stasiun_update(int index, const char *nama, const char *kota) {
    if (index < 0 || index >= g_stasiunCount) return;
    
    if (nama) {
        strncpy(g_stasiun[index].nama, nama, sizeof(g_stasiun[index].nama)-1);
        g_stasiun[index].nama[sizeof(g_stasiun[index].nama)-1] = '\0';
    }
    if (kota) {
        strncpy(g_stasiun[index].kota, kota, sizeof(g_stasiun[index].kota)-1);
        g_stasiun[index].kota[sizeof(g_stasiun[index].kota)-1] = '\0';
    }
    stasiun_saveToFile();
}

/* Delete (Soft Delete) */
static void stasiun_delete(int index) {
    if (index < 0 || index >= g_stasiunCount) return;
    g_stasiun[index].active = 0;
    stasiun_saveToFile();
}

/* Init (Load saat start) */
static void stasiun_init() {
    stasiun_loadFromFile();
}

#endif