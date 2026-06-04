# Text Editor Dinamis - Doa Ibu

Text Editor Dinamis - Doa Ibu adalah aplikasi text editor berbasis terminal (CLI) yang dibuat menggunakan bahasa C. Proyek ini dikembangkan untuk menerapkan konsep struktur data **Multi-Level Doubly Linked List**, di mana data teks disimpan secara dinamis mulai dari tingkat karakter, baris, hingga dokumen.

Berbeda dengan editor sebelumnya yang menyimpan teks dalam array, aplikasi ini menggunakan linked list sehingga proses penambahan, penghapusan, dan pengelolaan teks dapat dilakukan secara lebih fleksibel. Selain itu, editor juga mendukung penggunaan beberapa tab dalam satu kali proses.

---

## Fitur

Beberapa fitur yang tersedia pada aplikasi ini antara lain:

* Membuka hingga 5 tab dokumen sekaligus.
* Menyisipkan karakter pada posisi kursor.
* Menghapus karakter dan baris.
* Membuat baris baru (newline).
* Navigasi menggunakan tombol panah.
* Auto-wrap saat teks melewati batas tampilan.
* Scrolling untuk dokumen yang panjang.
* Fitur pencarian kata (Find).
* Highlight hasil pencarian.
* Replace dan Replace All.
* Menyimpan dokumen ke file.
* Menyimpan dokumen dengan nama baru (Save As).
* Membuka file yang sudah ada (Load File).
* Manajemen multi-tab.
* Deteksi perubahan dokumen melalui status `isModified`.

---

## Shortcut Keyboard

Untuk mempermudah penggunaan editor, tersedia beberapa shortcut keyboard yang dapat digunakan untuk mengakses berbagai fitur secara cepat.

| Tombol      | Fungsi                                          |
| ----------- | ----------------------------------------------- |
| Enter       | Membuat baris baru (New Line)                   |
| Backspace   | Menghapus karakter sebelum kursor               |
| Ctrl + D    | Menghapus baris aktif (Delete Line)             |
| Ctrl + F    | Mencari kata atau teks (Find)                   |
| Ctrl + L    | Mengganti seluruh kemunculan kata (Replace All) |
| Ctrl + S    | Menyimpan file (Save)                           |
| Ctrl + A    | Menyimpan file dengan nama baru (Save As)       |
| Ctrl + O    | Membuka file dari penyimpanan (Load File)       |
| Ctrl + W    | Membuka tab baru                                |
| Ctrl + Q    | Berpindah ke tab sebelumnya                     |
| Ctrl + R    | Berpindah ke tab berikutnya                     |
| Ctrl + E    | Menghapus tab aktif                             |
| Esc         | Keluar dari editor                              |
| Arrow Up    | Memindahkan kursor ke atas                      |
| Arrow Down  | Memindahkan kursor ke bawah                     |
| Arrow Left  | Memindahkan kursor ke kiri                      |
| Arrow Right | Memindahkan kursor ke kanan                     |

### Catatan Shortcut

* Editor mendukung maksimal **5 tab** yang dapat dibuka secara bersamaan.
* Perpindahan tab dilakukan menggunakan `Ctrl + Q` dan `Ctrl + R`.
* Fitur pencarian (`Ctrl + F`) akan melakukan highlight terhadap kata yang ditemukan.
* Fitur `Replace All` (`Ctrl + L`) mengganti seluruh kemunculan kata dalam dokumen aktif.
* Status `isModified` digunakan untuk mendeteksi perubahan dokumen.

---

## Struktur Data yang Digunakan

Aplikasi dibangun menggunakan struktur data **Multi-Level Doubly Linked List** yang terdiri dari tiga level utama.

### CharNode

Digunakan untuk menyimpan satu karakter.

```c
struct CharNode {
    char data;
    CharNode *next;
    CharNode *prev;
};
```

### LineNode

Digunakan untuk menyimpan satu baris teks yang terdiri dari kumpulan karakter.

```c
struct LineNode {
    CharNode *firstChar;
    CharNode *lastChar;

    LineNode *up;
    LineNode *down;

    int length;
    bool isNewLine;
};
```

### TabNode

Digunakan untuk menyimpan satu dokumen lengkap beserta posisi kursor dan viewport.

```c
struct TabNode {
    char fileName[MAX_PATH];

    LineNode *firstLine;
    LineNode *currLine;
    CharNode *currChar;

    LineNode *topLine;

    int targetX;
    int cursorX;
    int cursorY;

    int topIndex;

    TabNode *next;
    TabNode *prev;

    bool isModified;
};
```

### Editor

Merupakan struktur utama yang mengelola seluruh tab yang sedang dibuka.

```c
struct Editor {
    TabNode *activeTab;

    int n_tabs;
    int curr_tab;

    char findKeyword[100];
};
```

---

## Gambaran Struktur Data

```text
Editor
│
├── TabNode
│   │
│   ├── LineNode
│   │   │
│   │   ├── CharNode
│   │   ├── CharNode
│   │   └── CharNode
│   │
│   └── LineNode
│
├── TabNode
│
└── TabNode
```

Hubungan antar node:

```text
TabNode <--> TabNode <--> TabNode

LineNode
   ▲
   │
   ▼
LineNode

CharNode <--> CharNode <--> CharNode
```

---

## Struktur Folder

```text
.
├── main.c
├── CONFIG.h
│
├── CALLISTA
│   ├── callista.c
│   └── callista.h
│
├── RAMA
│   ├── rama.c
│   └── rama.h
│
└── RASYID
    ├── RASYID.c
    └── RASYID.h
```

---

## Pembagian Modul

### main.c

File utama yang menjalankan program. Pada file ini dilakukan inisialisasi editor, pengelolaan loop utama aplikasi, serta penghubung antar modul.

### Modul RAMA

Modul ini berfokus pada pengelolaan struktur data dan proses penyuntingan teks.

Fungsi yang ditangani antara lain:

* Inisialisasi editor.
* Alokasi dan reset node.
* Insert karakter.
* Penambahan baris baru.
* Auto-wrap.
* Penyimpanan file.
* Save As.
* Validasi nama file.
* Fitur Find dan Highlight.

### Modul CALLISTA

Modul ini menangani tampilan editor dan interaksi pengguna.

Fungsi yang ditangani antara lain:

* Rendering isi dokumen.
* Menampilkan header editor.
* Menampilkan scrollbar.
* Mengatur posisi kursor.
* Menyembunyikan dan menampilkan kursor.
* Membersihkan area tampilan.
* Keyboard handler.
* Navigasi menggunakan tombol panah.
* Redraw text.
* Load file.

### Modul RASYID

Modul ini menangani operasi lanjutan yang berhubungan dengan perubahan isi dokumen.

Fungsinya meliputi:

* Delete karakter.
* Delete line.
* Delete tab.
* Penambahan tab.
* Perpindahan tab.
* Dealokasi memori.
* Merge line.
* Replace All.

---

## Cara Kerja Program

Program berjalan di dalam loop utama yang terus menerima input dari pengguna.

```c
while (1)
{
    ...
}
```

Secara umum proses yang terjadi adalah:

1. Menunggu input keyboard.
2. Membaca input menggunakan `_getch()`.
3. Menentukan jenis tombol yang ditekan.
4. Menjalankan fungsi yang sesuai dengan shortcut yang digunakan.
5. Memperbarui struktur data editor.
6. Merender ulang tampilan editor.
7. Menampilkan hasil perubahan secara real-time.

---

## Kompilasi

Menggunakan GCC atau MinGW:

```bash
gcc main.c ./RASYID/RASYID.c ./RAMA/rama.c ./CALLISTA/callista.c -o text_editor_doaibu.exe
```

---

## Menjalankan Program

Windows:

```bash
text_editor_doaibu.exe
```

atau

```bash
./text_editor_doaibu.exe
```

---

## Teknologi yang Digunakan

* Bahasa C
* Doubly Linked List
* Multi-Level Doubly Linked List
* Dynamic Memory Allocation
* Modular Programming
* Windows Console API

---

## Tim Pengembang

### Rama Dwi Nugraha

* Struktur data editor
* Insert karakter
* Newline dan Auto-Wrap
* Save dan Save As
* Find dan Highlight
* Dokumentasi proyek

### Callista

* Rendering editor
* Header dan scrollbar
* Posisi kursor
* Keyboard handler
* Navigasi editor
* Load File

### Rasyid

* Delete karakter
* Delete line
* Delete tab
* Penambahan dan perpindahan tab
* Dealokasi memori
* Merge line
* Replace All

---

## Catatan

Proyek ini dibuat untuk memenuhi tugas besar mata kuliah Proyek 2, yaitu mengembangkan aplikasi berbasis library menggunakan bahasa C. Aplikasi yang dikembangkan berupa text editor berbasis terminal yang memanfaatkan struktur data Multi-Level Doubly Linked List untuk mengelola data teks secara dinamis.

Selama proses pengembangan, berbagai konsep yang telah dipelajari seperti modular programming, penggunaan header file, pengelolaan memori dinamis, serta implementasi linked list diterapkan secara langsung ke dalam aplikasi. Setiap bagian program dipisahkan ke dalam beberapa modul agar kode lebih terstruktur, mudah dipahami, dan mudah dikembangkan lebih lanjut.

Melalui proyek ini, kami tidak hanya mempelajari cara membangun sebuah aplikasi yang dapat digunakan, tetapi juga memahami bagaimana struktur data, manajemen memori, dan pembagian modul program berperan dalam pengembangan perangkat lunak yang lebih kompleks.
