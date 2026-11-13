#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAKS_PRODUK        50
#define PANJANG_NAMA       40
#define BATAS_STOK_MENIPIS 5

/*DATA GLOBAL (ARRAY)*/
char nama[MAKS_PRODUK][PANJANG_NAMA];  /* nama produk            */
int  harga[MAKS_PRODUK];               /* harga satuan (Rupiah)  */
int  stok[MAKS_PRODUK];                /* stok tersedia          */
int  terjual[MAKS_PRODUK];             /* jumlah unit terjual    */
long pendapatan[MAKS_PRODUK];          /* pendapatan per produk  */
int  jumlahProduk = 0;                 /* jumlah data aktif      */

/*FUNGSI BANTU INPUT*/
int bacaBaris(char *buffer, int ukuran)
{
    int c;
    if (fgets(buffer, ukuran, stdin) == NULL) {
        return 0;
    }
    if (strchr(buffer, '\n') == NULL) {
        /* baris terlalu panjang: buang sisa karakter */
        while ((c = getchar()) != '\n' && c != EOF) { }
    } else {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
    return 1;
}

/* Membaca bilangan bulat. Return 1 jika valid, 0 jika bukan angka. */
int bacaAngka(const char *pesan, int *hasil)
{
    char baris[64];
    char *akhir;
    long nilai;

    printf("%s", pesan);
    if (!bacaBaris(baris, sizeof(baris))) {
        printf("\nInput berakhir. Program ditutup.\n");
        exit(0);
    }
    if (baris[0] == '\0') {
        printf("[!] Input tidak boleh kosong.\n");
        return 0;
    }
    nilai = strtol(baris, &akhir, 10);
    if (*akhir != '\0') {
        printf("[!] Input harus berupa bilangan bulat.\n");
        return 0;
    }
    if (nilai > INT_MAX || nilai < INT_MIN) {
        printf("[!] Angka terlalu besar.\n");
        return 0;
    }
    *hasil = (int)nilai;
    return 1;
}

/* Membaca teks (nama). */
void bacaTeks(const char *pesan, char *hasil, int ukuran)
{
    printf("%s", pesan);
    if (!bacaBaris(hasil, ukuran)) {
        printf("\nInput berakhir. Program ditutup.\n");
        exit(0);
    }
}

/* Membandingkan dua string tanpa membedakan huruf besar/kecil. */
int samaTanpaKapital(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

/* Mengecek apakah 'kata' ada di dalam 'teks' (tanpa membedakan kapital). */
int mengandungKata(const char *teks, const char *kata)
{
    int i, j;
    int pt = (int)strlen(teks);
    int pk = (int)strlen(kata);

    if (pk == 0) return 0;
    for (i = 0; i + pk <= pt; i++) {
        for (j = 0; j < pk; j++) {
            if (tolower((unsigned char)teks[i + j]) !=
                tolower((unsigned char)kata[j])) {
                break;
            }
        }
        if (j == pk) return 1;
    }
    return 0;
}

/* Meminta nomor produk (1..jumlahProduk). Return indeks array, atau -1. */
int pilihProduk(const char *pesan)
{
    int nomor;
    if (!bacaAngka(pesan, &nomor)) return -1;
    if (nomor < 1 || nomor > jumlahProduk) {
        printf("[!] Nomor produk %d tidak ditemukan.\n", nomor);
        return -1;
    }
    return nomor - 1;
}

/*DATA AWAL*/

void isiProduk(const char *n, int h, int s)
{
    strncpy(nama[jumlahProduk], n, PANJANG_NAMA - 1);
    nama[jumlahProduk][PANJANG_NAMA - 1] = '\0';
    harga[jumlahProduk]      = h;
    stok[jumlahProduk]       = s;
    terjual[jumlahProduk]    = 0;
    pendapatan[jumlahProduk] = 0;
    jumlahProduk++;
}

void muatDataAwal(void)
{
    isiProduk("Nasi Goreng", 12000, 20);
    isiProduk("Mie Ayam", 10000, 15);
    isiProduk("Es Teh Manis", 3000, 30);
    isiProduk("Kopi Hitam", 5000,  4);   /* sengaja < 5 untuk uji stok menipis */
    isiProduk("Gorengan", 1000, 50);
    isiProduk("Air Mineral", 3000, 25);
    isiProduk("Roti Bakar", 8000,  3);   /* sengaja < 5 untuk uji stok menipis */
}

/*MENU 1: LIHAT DAFTAR*/

void tampilBaris(int i)
{
    printf("%-4d %-25s Rp %-10d %-6d\n", i + 1, nama[i], harga[i], stok[i]);
}

void tampilDaftar(void)
{
    int i;
    printf("\n=== DAFTAR PRODUK ===\n");
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }
    printf("%-4s %-25s %-13s %-6s\n", "No", "Nama Produk", "Harga", "Stok");
    printf("--------------------------------------------------\n");
    for (i = 0; i < jumlahProduk; i++) {
        tampilBaris(i);
    }
    printf("--------------------------------------------------\n");
    printf("Jumlah produk: %d / %d\n", jumlahProduk, MAKS_PRODUK);
}

/*MENU 2: TAMBAH PRODUK*/

void tambahProduk(void)
{
    char namaBaru[PANJANG_NAMA];
    int hargaBaru, stokBaru, i;

    printf("\n=== TAMBAH PRODUK ===\n");
    if (jumlahProduk >= MAKS_PRODUK) {
        printf("[!] Kapasitas penuh (maksimal %d produk). Produk tidak dapat ditambah.\n",
               MAKS_PRODUK);
        return;
    }

    bacaTeks("Nama produk : ", namaBaru, sizeof(namaBaru));
    if (namaBaru[0] == '\0') {
        printf("[!] Nama produk tidak boleh kosong.\n");
        return;
    }
    for (i = 0; i < jumlahProduk; i++) {
        if (samaTanpaKapital(nama[i], namaBaru)) {
            printf("[!] Produk \"%s\" sudah ada (nomor %d).\n", nama[i], i + 1);
            return;
        }
    }

    if (!bacaAngka("Harga (Rp) : ", &hargaBaru)) return;
    if (hargaBaru < 0) {
        printf("[!] Harga tidak boleh negatif.\n");
        return;
    }
    if (hargaBaru == 0) {
        printf("[!] Harga harus lebih dari 0.\n");
        return;
    }
    if (!bacaAngka("Stok awal   : ", &stokBaru)) return;
    if (stokBaru < 0) {
        printf("[!] Stok tidak boleh negatif.\n");
        return;
    }

    isiProduk(namaBaru, hargaBaru, stokBaru);
    printf("[OK] Produk \"%s\" berhasil ditambahkan sebagai nomor %d.\n",
           nama[jumlahProduk - 1], jumlahProduk);
}

/*MENU 3: UBAH PRODUK*/

void ubahProduk(void)
{
    int idx, pilihan, nilai;

    printf("\n=== UBAH PRODUK ===\n");
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }
    tampilDaftar();

    idx = pilihProduk("\nNomor produk yang diubah: ");
    if (idx < 0) return;

    printf("Produk terpilih: %s (harga Rp %d, stok %d)\n", nama[idx], harga[idx], stok[idx]);
    printf("1. Ubah harga\n2. Ubah stok\n");
    if (!bacaAngka("Pilihan: ", &pilihan)) return;

    if (pilihan == 1) {
        if (!bacaAngka("Harga baru (Rp): ", &nilai)) return;
        if (nilai < 0) {
            printf("[!] Harga tidak boleh negatif.\n");
            return;
        }
        if (nilai == 0) {
            printf("[!] Harga harus lebih dari 0.\n");
            return;
        }
        harga[idx] = nilai;
        printf("[OK] Harga %s diubah menjadi Rp %d.\n", nama[idx], harga[idx]);
    } else if (pilihan == 2) {
        if (!bacaAngka("Stok baru: ", &nilai)) return;
        if (nilai < 0) {
            printf("[!] Stok tidak boleh negatif.\n");
            return;
        }
        stok[idx] = nilai;
        printf("[OK] Stok %s diubah menjadi %d.\n", nama[idx], stok[idx]);
    } else {
        printf("[!] Pilihan tidak tersedia.\n");
    }
}

/*MENU 4: TRANSAKSI PENJUALAN*/

void transaksiPenjualan(void)
{
    int keranjang[MAKS_PRODUK];   /* jumlah beli per produk (indeks sama dengan produk) */
    int i, idx, qty, adaItem;
    long subtotal, total;
    char yakin[16];

    printf("\n=== TRANSAKSI PENJUALAN ===\n");
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }

    for (i = 0; i < MAKS_PRODUK; i++) keranjang[i] = 0;

    tampilDaftar();
    printf("\nMasukkan nomor produk dan jumlah. Ketik 0 pada nomor produk untuk selesai.\n");

    while (1) {
        int nomor;
        if (!bacaAngka("\nNomor produk (0 = selesai): ", &nomor)) continue;
        if (nomor == 0) break;
        if (nomor < 1 || nomor > jumlahProduk) {
            printf("[!] Nomor produk %d tidak ditemukan.\n", nomor);
            continue;
        }
        idx = nomor - 1;

        if (!bacaAngka("Jumlah beli: ", &qty)) continue;
        if (qty <= 0) {
            printf("[!] Jumlah pembelian harus lebih dari 0.\n");
            continue;
        }
        if (qty + keranjang[idx] > stok[idx]) {
            printf("[!] Stok %s tidak cukup. Tersedia %d, sudah di keranjang %d, diminta %d.\n",
                   nama[idx], stok[idx], keranjang[idx], qty);
            continue;
        }
        keranjang[idx] += qty;
        printf("[OK] %d x %s dimasukkan ke keranjang.\n", qty, nama[idx]);
    }

    /*Cek apakah keranjang kosong*/
    adaItem = 0;
    for (i = 0; i < jumlahProduk; i++) {
        if (keranjang[i] > 0) { adaItem = 1; break; }
    }
    if (!adaItem) {
        printf("Keranjang kosong. Transaksi dibatalkan.\n");
        return;
    }

    /*Tampilkan rincian dan hitung total*/
    total = 0;
    printf("\n------------------- RINCIAN -------------------\n");
    printf("%-22s %5s %10s %12s\n", "Produk", "Qty", "Harga", "Subtotal");
    for (i = 0; i < jumlahProduk; i++) {
        if (keranjang[i] > 0) {
            subtotal = (long)keranjang[i] * harga[i];
            total += subtotal;
            printf("%-22s %5d %10d %12ld\n", nama[i], keranjang[i], harga[i], subtotal);
        }
    }
    printf("-----------------------------------------------\n");
    printf("%-22s %5s %10s %12ld\n", "TOTAL PEMBAYARAN", "", "", total);

    bacaTeks("\nSelesaikan transaksi? (y/n): ", yakin, sizeof(yakin));
    if (yakin[0] != 'y' && yakin[0] != 'Y') {
        printf("Transaksi dibatalkan. Stok tidak berubah.\n");
        return;
    }

    /* Transaksi berhasil: kurangi stok, tambah jumlah terjual dan pendapatan */
    for (i = 0; i < jumlahProduk; i++) {
        if (keranjang[i] > 0) {
            stok[i]       -= keranjang[i];
            terjual[i]    += keranjang[i];
            pendapatan[i] += (long)keranjang[i] * harga[i];
        }
    }
    printf("[OK] Transaksi berhasil. Total dibayar: Rp %ld\n", total);
}

/*MENU 5: TAMBAH STOK*/

void tambahStok(void)
{
    int idx, jumlah;

    printf("\n=== TAMBAH STOK ===\n");
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }
    tampilDaftar();

    idx = pilihProduk("\nNomor produk: ");
    if (idx < 0) return;

    if (!bacaAngka("Jumlah stok yang ditambahkan: ", &jumlah)) return;
    if (jumlah <= 0) {
        printf("[!] Jumlah tambahan stok harus lebih dari 0.\n");
        return;
    }
    if (jumlah > INT_MAX - stok[idx]) {
        printf("[!] Jumlah terlalu besar.\n");
        return;
    }
    stok[idx] += jumlah;
    printf("[OK] Stok %s sekarang %d.\n", nama[idx], stok[idx]);
}

/*MENU 6: CARI PRODUK*/

void cariProduk(void)
{
    char kata[PANJANG_NAMA];
    int i, ditemukan = 0;

    printf("\n=== CARI PRODUK ===\n");
    bacaTeks("Masukkan nama produk (boleh sebagian): ", kata, sizeof(kata));
    if (kata[0] == '\0') {
        printf("[!] Kata kunci tidak boleh kosong.\n");
        return;
    }

    for (i = 0; i < jumlahProduk; i++) {
        if (mengandungKata(nama[i], kata)) {
            if (!ditemukan) {
                printf("%-4s %-25s %-13s %-6s\n", "No", "Nama Produk", "Harga", "Stok");
                printf("--------------------------------------------------\n");
            }
            tampilBaris(i);
            ditemukan++;
        }
    }
    if (!ditemukan) {
        printf("Produk dengan kata kunci \"%s\" tidak ditemukan.\n", kata);
    } else {
        printf("Ditemukan %d produk.\n", ditemukan);
    }
}

/*MENU 7: URUTKAN PRODUK*/

void urutkanIndeks(int urutan[], int mode)
{
    int i, j, kunci, perluGeser;

    for (i = 0; i < jumlahProduk; i++) urutan[i] = i;

    for (i = 1; i < jumlahProduk; i++) {
        kunci = urutan[i];
        j = i - 1;
        while (j >= 0) {
            if (mode == 1) perluGeser = harga[urutan[j]]   > harga[kunci];
            else           perluGeser = terjual[urutan[j]] < terjual[kunci];
            if (!perluGeser) break;
            urutan[j + 1] = urutan[j];
            j--;
        }
        urutan[j + 1] = kunci;
    }
}

void urutkanProduk(void)
{
    int urutan[MAKS_PRODUK];
    int pilihan, i;

    printf("\n=== URUTKAN PRODUK ===\n");
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }
    printf("1. Harga dari termurah\n2. Jumlah terjual dari terbanyak\n");
    if (!bacaAngka("Pilihan: ", &pilihan)) return;
    if (pilihan != 1 && pilihan != 2) {
        printf("[!] Pilihan tidak tersedia.\n");
        return;
    }

    urutkanIndeks(urutan, pilihan);

    if (pilihan == 1) printf("\nUrutan berdasarkan harga (termurah):\n");
    else              printf("\nUrutan berdasarkan jumlah terjual (terbanyak):\n");

    printf("%-4s %-25s %-13s %-6s %-8s\n", "No", "Nama Produk", "Harga", "Stok", "Terjual");
    printf("---------------------------------------------------------\n");
    for (i = 0; i < jumlahProduk; i++) {
        int k = urutan[i];
        printf("%-4d %-25s Rp %-10d %-6d %-8d\n", k + 1, nama[k], harga[k], stok[k], terjual[k]);
    }
}

/*MENU 8: LAPORAN PENJUALAN*/

void laporanPenjualan(void)
{
    int i, totalUnit = 0, maks = 0;
    long totalPendapatan = 0;

    printf("\n=== LAPORAN PENJUALAN ===\n");
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }

    printf("%-4s %-25s %-8s %-14s\n", "No", "Nama Produk", "Terjual", "Pendapatan");
    printf("-------------------------------------------------------\n");
    for (i = 0; i < jumlahProduk; i++) {
        printf("%-4d %-25s %-8d Rp %-11ld\n", i + 1, nama[i], terjual[i], pendapatan[i]);
        totalUnit      += terjual[i];
        totalPendapatan += pendapatan[i];
        if (terjual[i] > maks) maks = terjual[i];
    }
    printf("-------------------------------------------------------\n");
    printf("Total unit terjual : %d\n", totalUnit);
    printf("Total pendapatan   : Rp %ld\n", totalPendapatan);

    if (maks == 0) {
        printf("Produk terlaris    : (belum ada penjualan)\n");
    } else {
        printf("Produk terlaris    : ");
        int pertama = 1;
        for (i = 0; i < jumlahProduk; i++) {
            if (terjual[i] == maks) {
                if (!pertama) printf(", ");
                printf("%s", nama[i]);
                pertama = 0;
            }
        }
        printf(" (%d unit)\n", maks);
    }
}

/*MENU 9: CEK STOK MENIPIS*/

void cekStokMenipis(void)
{
    int i, ada = 0;

    printf("\n=== STOK MENIPIS (stok < %d) ===\n", BATAS_STOK_MENIPIS);
    for (i = 0; i < jumlahProduk; i++) {
        if (stok[i] < BATAS_STOK_MENIPIS) {
            if (!ada) {
                printf("%-4s %-25s %-13s %-6s\n", "No", "Nama Produk", "Harga", "Stok");
                printf("--------------------------------------------------\n");
            }
            tampilBaris(i);
            ada = 1;
        }
    }
    if (!ada) {
        printf("Semua produk memiliki stok aman.\n");
    } else {
        printf("[PERINGATAN] Segera lakukan penambahan stok untuk produk di atas.\n");
    }
}

/*Menu*/

void tampilMenu(void)
{
    printf("\n==========================================\n");
    printf("  MONITORING STOK & PENJUALAN KANTIN KAMPUS\n");
    printf("==========================================\n");
    printf("1. Lihat Daftar Produk\n");
    printf("2. Tambah Produk\n");
    printf("3. Ubah Produk\n");
    printf("4. Transaksi Penjualan\n");
    printf("5. Tambah Stok\n");
    printf("6. Cari Produk\n");
    printf("7. Urutkan Produk\n");
    printf("8. Laporan Penjualan\n");
    printf("9. Cek Stok Menipis\n");
    printf("0. Keluar\n");
}

int main(void)
{
    int pilihan;

    muatDataAwal();

    do {
        tampilMenu();
        if (!bacaAngka("Pilih menu: ", &pilihan)) {
            continue;
        }
        switch (pilihan) {
            case 1: tampilDaftar();       break;
            case 2: tambahProduk();       break;
            case 3: ubahProduk();         break;
            case 4: transaksiPenjualan(); break;
            case 5: tambahStok();         break;
            case 6: cariProduk();         break;
            case 7: urutkanProduk();      break;
            case 8: laporanPenjualan();   break;
            case 9: cekStokMenipis();     break;
            case 0: printf("\nTerima kasih. Program selesai.\n"); break;
            default: printf("[!] Pilihan menu tidak tersedia. Pilih 0-9.\n");
        }
    } while (pilihan != 0);

    return 0;
}
