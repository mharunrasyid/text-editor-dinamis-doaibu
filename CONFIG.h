#ifndef CONFIG_h
#define CONFIG_h

// windows lib
#include <windows.h>
#include <conio.h>

// c lib
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_TABS 5
#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 20
#define MAX_PATH 260

typedef struct CharNode CharNode;
typedef struct LineNode LineNode;
typedef struct TabNode TabNode;
typedef struct Editor Editor;

// Struct Char

struct CharNode { 
    char data; // Karakter yang disimpan 
    CharNode *next; // Pointer ke karakter kanan 
    CharNode *prev; // Pointer ke karakter kiri 
};

// Struct Baris

struct LineNode { 
    CharNode *firstChar; // Head dari list karakter di baris ini 
    CharNode *lastChar; // Tail (opsional, untuk optimasi navigasi dari belakang) 

    LineNode *up; // Pointer ke baris atas 
    LineNode *down; // Pointer ke baris bawah 

    int length; // Jumlah karakter (untuk cek auto-wrap) 
    bool isNewLine; // TRUE jika Enter, FALSE jika Auto-wrap 
};

// Struct Tab

struct TabNode { 
    char fileName[MAX_PATH]; // Nama file untuk ditampilkan di tab bar 
    LineNode *firstLine; // Head dari seluruh baris dalam dokumen 

    // POSISI KURSOR (Penting disimpan per tab agar tidak hilang saat pindah tab) 
    LineNode *currLine; // Baris tempat kursor berada 
    CharNode *currChar; // Node karakter spesifik tempat kursor berada 
    LineNode *topLine; // Baris paling atas yang nampak di layar (Viewport)

    int targetX; // posisi x yang membantu proses arrow up/down

    int cursorX; // kursor x tampilan
    int cursorY; // kursor y tampilan
    int topIndex; // Nomor baris absolut untuk Line Numbering

    TabNode *next; // Pointer ke tab sebelah kanan 
    TabNode *prev; // Pointer ke tab sebelah kiri 
};

// Struct Editor

struct Editor { 
    TabNode *activeTab; // Tab yang sedang tampil di layar 
};

// Var Global
extern Editor E;

#endif