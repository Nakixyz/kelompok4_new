//
// Created by lenovo on 12/10/2025.
//

#ifndef REVIEW2_1_CREATESTASIUN_H
#define REVIEW2_1_CREATESTASIUN_H

#include <stdio.h>
#include <stdlib.h>

// PROSEDUR untuk input data stasiun & simpan ke stasiun.dat
void inputStasiun() {
    FILE *fp = fopen("stasiun.dat", "a");
    if (!fp) {
        printf("Gagal membuka file stasiun.dat!\n");
        return;
    }

    int lagi = 1;

    while (lagi == 1) {

        char id[10], kode[10], nama[50], kota[50], alamat[100];

        printf("Masukkan ID Stasiun     : ");
        fflush(stdin);
        fgets(id, sizeof(id), stdin);

        printf("Masukkan Kode Stasiun   : ");
        fflush(stdin);
        fgets(kode, sizeof(kode), stdin);

        printf("Masukkan Nama Stasiun   : ");
        fflush(stdin);
        fgets(nama, sizeof(nama), stdin);

        printf("Masukkan Kota           : ");
        fflush(stdin);
        fgets(kota, sizeof(kota), stdin);

        printf("Masukkan Alamat         : ");
        fflush(stdin);
        fgets(alamat, sizeof(alamat), stdin);

        // Tulis ke file dat
        fprintf(fp, "%s|%s|%s|%s|%s\n", id, kode, nama, kota, alamat);

        printf("\nTambah data lagi? (1 = Ya, 0 = Tidak): ");
        scanf("%d", &lagi);
        getchar(); // bersihkan newline
    }

    fclose(fp);
    printf("\nData berhasil disimpan ke stasiun.dat!\n");
}

#endif //REVIEW2_1_CREATESTASIUN_H