#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUKU 100
#define MAX_PINJAM 100
#define MAX_STRING_LENGTH 100

// Definisikan tipe data buku
typedef struct {
    int id;
    char judul[MAX_STRING_LENGTH];
    char pengarangBuku[MAX_STRING_LENGTH];
    int stock;
    char rating[MAX_STRING_LENGTH];
} buku;

// Definisikan tipe data peminjaman
typedef struct {
    int bukuId;
    char judulBuku[MAX_STRING_LENGTH];
    char namaPeminjam[MAX_STRING_LENGTH];
    int quantity;
} dataPeminjam;

int loginkeSistem(){
    char username[20];
    char password[20];
    char usernameBenar[] = "admin";
    char passwordBenar[] = "admin";

    printf("\n=== Login Ke Sistem Perpustakaan ===\n");
    printf("Username : ");
    scanf("%s", username);
    printf("Password : ");
    scanf("%s", password);

    if (strcmp(username, usernameBenar) == 0 && strcmp(password, passwordBenar) == 0) {
        printf("Login berhasil.\n");
        return 1; // Login berhasil
    } else {
        printf("Login gagal. Username atau password salah.\n");
        printf("\nAkses ditolak. Silakan coba lagi.\n");
        return 0; // Login gagal
    }
}

// Fungsi untuk membaca data buku dari file CSV
int membacaDataBuku(buku bukuPerpus[], const char *filename) {
    FILE *file = fopen("databuku.csv", "r");
    if (!file) {
        printf("\nTidak dapat membuka file buku.\n");
        return 0;
    }

    int count = 0;
    while (fscanf(file, "%d,%[^,],%[^,],%d,%[^\n]", &bukuPerpus[count].id, bukuPerpus[count].judul, bukuPerpus[count].pengarangBuku, &bukuPerpus[count].stock, bukuPerpus[count].rating) == 5) {
        count++;
        if (count >= MAX_BUKU) 
        break;
    }

    fclose(file);
    return count;
}

// Fungsi untuk membaca data peminjam buku dari file CSV
int membacaDataPeminjamBuku(dataPeminjam borrow[], const char *filename){
    FILE *file = fopen("datapeminjambuku.csv", "r");
    if (!file) {
        printf("\nTidak dapat membuka file peminjam.\n");
        return 0;
    }

    int count = 0;
    while (fscanf(file, "%d,%[^,],%[^,],%d,%[^\n]", &borrow[count].bukuId, borrow[count].judulBuku, borrow[count].namaPeminjam, &borrow[count].quantity) == 4) {
        count++;
        if (count >= MAX_PINJAM) 
        break;
    }

    fclose(file);
    return count;
}

// Fungsi untuk menyimpan data peminjaman ke file CSV
void menyimpanDataPeminjam(int bukuId, const char *judulBuku, const char *namaPeminjam, int quantity, const char *filename) {
    FILE *file = fopen("datapeminjambuku.csv", "a");
    if (!file) {
        printf("Tidak dapat membuka file untuk menyimpan data peminjaman.\n");
        return;
    }

    fprintf(file, "%d,%s,%s,%d\n", bukuId, judulBuku, namaPeminjam, quantity);
    fclose(file);
}

// Fungsi untuk menghapus data peminjaman dari file CSV ketika buku dikembalikan
void menghapusDataPeminjam(int bukuId, const char *namaPeminjam, int quantity, const char *filename) {
    FILE *file = fopen("datapeminjambuku.csv", "r");
    FILE *tempFile = fopen("data_sementara.csv", "w");
    if (!file || !tempFile) {
        printf("\nTidak dapat membuka file untuk menghapus data peminjaman.\n");
        return;
    }

    dataPeminjam rekap;
    int ditemukan = 0;

    // Baca setiap baris dan tulis ulang ke file sementara jika data tidak cocok
    while (fscanf(file, "%d,%[^,],%[^,],%d\n", &rekap.bukuId, rekap.judulBuku, rekap.namaPeminjam, &rekap.quantity) == 4) {
        if (rekap.bukuId == bukuId && strcmp(rekap.namaPeminjam, namaPeminjam) == 0) {
            if (rekap.quantity > quantity) {
                rekap.quantity -= quantity;  // Kurangi jumlah yang dikembalikan
                fprintf(tempFile, "%d,%s,%s,%d\n", rekap.bukuId, rekap.judulBuku, rekap.namaPeminjam, rekap.quantity);
            } 
            ditemukan = 1;
        } else {
            fprintf(tempFile, "%d,%s,%s,%d\n", rekap.bukuId, rekap.judulBuku, rekap.namaPeminjam, rekap.quantity);
        }
    }

    fclose(file);
    fclose(tempFile);

    // Ganti file asli dengan file sementara
    remove(filename);
    rename("data_sementara.csv", filename);

    if (ditemukan) {
        printf("\nData peminjaman buku berhasil diperbarui.\n");
    } else {
        printf("\nData peminjaman buku tidak ditemukan.\n");
    }
}

// Fungsi untuk menampilkan daftar buku dalam bentuk tabel
void tampilkanBuku(buku bukuPerpus[], int count) {
    printf("\nDaftar Buku:\n");
    printf("ID  | Judul Buku                                       | Penulis              | Stok |\n");
    printf("--------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-3d | %-48s | %-20s | %-4d |\n",
               bukuPerpus[i].id, bukuPerpus[i].judul, bukuPerpus[i].pengarangBuku, bukuPerpus[i].stock);
    }
}

// Fungsi untuk meminjam beberapa buku sekaligus
void pinjamBuku(buku bukuPerpus[], int count) {
    int nomerBuku, id, quantity;
    char namaPeminjam[MAX_STRING_LENGTH];

    printf("Masukkan nama peminjam: ");
    scanf(" %[^\n]", namaPeminjam);

    printf("Masukkan jumlah buku yang ingin dipinjam: ");
    scanf("%d", &nomerBuku);

    for (int i = 0; i < nomerBuku; i++) {
        printf("\nMasukkan ID buku ke-%d yang ingin dipinjam: ", i + 1);
        scanf("%d", &id);
        printf("Masukkan jumlah yang ingin dipinjam untuk buku ID %d: ", id);
        scanf("%d", &quantity);

        int ditemukan = 0;
        for (int j = 0; j < count; j++) {
            if (bukuPerpus[j].id == id) {
                ditemukan = 1;
                if (bukuPerpus[j].stock >= quantity) {
                    bukuPerpus[j].stock -= quantity;
                    printf("%d buku '%s' berhasil dipinjam oleh %s.\n", quantity, bukuPerpus[j].judul, namaPeminjam);

                    // Simpan data peminjaman ke file CSV
                    menyimpanDataPeminjam(id, bukuPerpus[j].judul, namaPeminjam, quantity, "datapeminjambuku.csv");
                } else {
                    printf("Stok buku '%s' tidak mencukupi. Tersedia: %d\n", bukuPerpus[j].judul, bukuPerpus[j].stock);
                }
                break;
            }
        }
        if (!ditemukan) {
            printf("Buku dengan ID %d tidak ditemukan.\n", id);
        }
    }
}

// Fungsi untuk mengembalikan beberapa buku sekaligus
void kembalikanBuku(buku bukuPerpus[], int count) {
    int nomerBuku, id, quantity;
    char namaPeminjam[MAX_STRING_LENGTH];
    char rating[MAX_STRING_LENGTH]; // Tambahkan variabel rating
    float nilaiRating; // Tambahkan variabel untuk menyimpan rating sebagai angka

    printf("Masukkan nama peminjam: ");
    scanf(" %[^\n]", namaPeminjam);

    printf("Masukkan jumlah buku yang ingin dikembalikan: ");
    scanf("%d", &nomerBuku);

    for (int i = 0; i < nomerBuku; i++) {
        printf("\nMasukkan ID buku ke-%d yang ingin dikembalikan: ", i + 1);
        scanf("%d", &id);
        printf("Masukkan jumlah yang ingin dikembalikan untuk buku ID %d: ", id);
        scanf("%d", &quantity);

        int ditemukan = 0;
        for (int j = 0; j < count; j++) {
            if (bukuPerpus[j].id == id) {
                ditemukan = 1;
                bukuPerpus[j].stock += quantity;
                printf("%d buku '%s' berhasil dikembalikan.\n", quantity, bukuPerpus[j].judul);

                // Validasi input rating
                do {
                    printf("Masukkan rating untuk buku '%s' (1 hingga 5): ", bukuPerpus[j].judul);
                    scanf(" %[^\n]", rating);
                    nilaiRating = atof(rating); // Ubah rating menjadi float
                    if (nilaiRating < 1 || nilaiRating > 5) {
                        printf("\nRating tidak valid! Harus antara 1 dan 5.\n");
                    }
                } while (nilaiRating < 1 || nilaiRating > 5);

                strcpy(bukuPerpus[j].rating, rating); // Simpan rating di struct buku

                // Update data buku ke file
                FILE *file = fopen("databuku.csv", "w");
                if (file) {
                    for (int k = 0; k < count; k++) {
                        fprintf(file, "%d,%s,%s,%d,%s\n", bukuPerpus[k].id, bukuPerpus[k].judul, bukuPerpus[k].pengarangBuku, bukuPerpus[k].stock, bukuPerpus[k].rating);
                    }
                    fclose(file);
                } else {
                    printf("Tidak dapat membuka file untuk menyimpan rating.\n");
                }

                // Hapus data peminjaman dari file CSV jika ditemukan
                menghapusDataPeminjam(id, namaPeminjam, quantity, "datapeminjambuku.csv");
                break;
            }
        }
        if (!ditemukan) {
            printf("Buku dengan ID %d tidak ditemukan.\n", id);
        }
    }
}

// Fungsi untuk menampilkan rating buku
void tampilkanRatingBuku(buku bukuPerpus[], int count) {
    printf("\nRating Buku:\n");
    printf("ID  | Judul Buku                                       | Penulis              | Rating |\n");
    printf("----------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-3d | %-48s | %-20s | %-6s |\n",
               bukuPerpus[i].id, bukuPerpus[i].judul, bukuPerpus[i].pengarangBuku, bukuPerpus[i].rating);
    }
}

// fungsi untuk menampikan data peminjam buku
void tampilkanDataPeminjam(dataPeminjam borrow[], int count) {
    FILE *file = fopen("datapeminjambuku.csv", "r");
    if (!file) {
        printf("Tidak dapat membuka file daftar peminjam.\n");
        return;
    }

    dataPeminjam rekap;
    printf("\nDaftar Peminjam Buku :\n");
    printf("ID  | Judul Buku                                       | Nama Peminjam             | Jumlah Dipinjam      |\n");
    printf("-----------------------------------------------------------------------------------------------------------\n");
    while (fscanf(file, "%d,%[^,],%[^,],%d\n", &rekap.bukuId, rekap.judulBuku, rekap.namaPeminjam, &rekap.quantity) == 4) {
        printf("%-3d | %-48s | %-25s | %-20d |\n", rekap.bukuId, rekap.judulBuku, rekap.namaPeminjam, rekap.quantity);
    }

    fclose(file);
}

int main() {
    // login ke sistem perpustakaan
    if (!loginkeSistem()) {
        return 0;
    }

    buku bukuPerpus[MAX_BUKU];
    int menghitungBuku = membacaDataBuku(bukuPerpus, "databuku.csv");
    if (menghitungBuku == 0) {
        printf("Tidak ada data buku.\n");
        return 1;
    }

    dataPeminjam pinjam[MAX_PINJAM];
    int jumlahMeminjam = membacaDataPeminjamBuku(pinjam, "datapeminjambuku.csv");
    if (jumlahMeminjam == 0) {
        printf("Tidak ada data peminjam buku.\n");
        return 1;
    }
    
    int pilihan;
    do {
        printf("\n=== Selamat Datang di Perpustakaan Jendela Kampus ===\n");
        printf("\nPilih Menu :\n");
        printf("1. Tampilkan Daftar Buku\n");
        printf("2. Pinjam Buku\n");
        printf("3. Kembalikan Buku\n");
        printf("4. Tampilkan Rating Buku\n");
        printf("5. Tampilkan Peminjam Buku\n");
        printf("6. Keluar\n");
        printf("Pilihan : ");
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                tampilkanBuku(bukuPerpus, menghitungBuku);
                break;
            case 2:
                pinjamBuku(bukuPerpus, menghitungBuku);
                break;
            case 3:
                kembalikanBuku(bukuPerpus, menghitungBuku);
                break;
            case 4:
                tampilkanRatingBuku(bukuPerpus, menghitungBuku);
                break;
            case 5:
                tampilkanDataPeminjam(pinjam, jumlahMeminjam);
                break;
            case 6:
                printf("\nTerima kasih! telah menggunakan sistem perpustakaan.\n");
                break;
            default:
                printf("Pilihan tidak valid.\n");
                break;
        }
    } while (pilihan != 6); // Dapat memilih sampai 6 menu

    return 0;
}
