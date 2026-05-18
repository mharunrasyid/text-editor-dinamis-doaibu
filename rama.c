#include "rama.h"

LineNode *AlokasiLine() {
    LineNode *LL = (LineNode *) malloc (sizeof(LineNode));

    if (LL != NULL) {
		resetLine(LL);
		
		return (LL);
	} else return NULL;
}

CharNode *AlokasiChar() {
    CharNode *CC = (CharNode *) malloc (sizeof(CharNode));

    if (CC != NULL) {
		resetChar(CC);
		
		return (CC);
	} else return NULL;
}



void insert(TabNode *TT, char c) {
    if (TT == NULL || TT->currLine == NULL) return;

    LineNode *LineKursor = TT->currLine;
    CharNode *CharBaru = AlokasiChar();
    if (CharBaru == NULL) return;
    CharBaru->data = c;

    // insert di awal baris kosong
    if (TT->currChar == NULL && LineKursor->firstChar == NULL) {
        CharBaru->next = NULL;
        CharBaru->prev = NULL;
        LineKursor->firstChar = CharBaru;
        LineKursor->lastChar  = CharBaru;
    }
    // insert di awal
    else if (TT->currChar == NULL && LineKursor->firstChar != NULL) {
        CharBaru->next = LineKursor->firstChar;
        CharBaru->prev = NULL;
        LineKursor->firstChar->prev = CharBaru;
        LineKursor->firstChar = CharBaru;
    }
    // insert di tengah
    else if (TT->currChar != NULL && TT->currChar->next != NULL) {
        CharBaru->prev = TT->currChar;
        CharBaru->next = TT->currChar->next;
        TT->currChar->next->prev = CharBaru;
        TT->currChar->next = CharBaru;
    }
    // insert di akhir
    else {
        CharBaru->prev = TT->currChar;
        CharBaru->next = NULL;
        TT->currChar->next = CharBaru;
        LineKursor->lastChar = CharBaru;
    }

    LineKursor->length++;
    TT->currChar = CharBaru;
    TT->cursorX++;
    TT->targetX = TT->cursorX;

    if (LineKursor->length > SCREEN_WIDTH) {
        handleOverflow(TT, LineKursor);
    }
}

void handleOverflow(TabNode *TT, LineNode *line) {
    if (line->length <= SCREEN_WIDTH) return;

    // Buat baris baru jika perlu
    LineNode *lineSelanjutnya = line->down;//next itu line dibawha kursor
    if (lineSelanjutnya == NULL || lineSelanjutnya->isNewLine == true) {
//    	addLine(TT, line); 
//        
//        // Ambil pointer baris yang baru saja dibuat oleh addLine
//        lineSelanjutnya = line->down; 
//        
//        // Pastikan isNewLine diatur ke false karena ini adalah auto-wrap
//      	lineSelanjutnya->isNewLine = false;
        
        LineNode *baru = AlokasiLine();//buat line namanya baru
        if (baru == NULL) return;
        baru->isNewLine = false; // Ini baris auto-wrap, bukan enter baru
        baru->up = line;// baru up teh ke kursor
        baru->down = lineSelanjutnya;// baru down teh ke sinext tadi
        line->down = baru;// si line kursor downnya ke baru
        if (lineSelanjutnya != NULL) lineSelanjutnya->up = baru;
        lineSelanjutnya = baru;//terus kita samakan si next itu jadi baru tadi
    }

    // Cabut lastChar dari baris ini
    CharNode *pindah = line->lastChar;
    pindah->prev->next = NULL;
//    if (pindah->prev != NULL) pindah->prev->next = NULL;
//    else line->firstChar = NULL;
    line->lastChar = pindah->prev;
    pindah->prev = NULL;
    line->length--;


    if (TT->currChar == pindah) {
        TT->currLine = lineSelanjutnya;
        TT->currChar = pindah; // Kursor tetap memegang karakter yang pindah
        TT->cursorY++;
        TT->cursorX = 2;       // Kursor sekarang berada di kolom 2 (setelah karakter pertama)
        TT->targetX = TT->cursorX;
    } 

    if (lineSelanjutnya->firstChar == NULL) {
        lineSelanjutnya->firstChar = pindah;
        lineSelanjutnya->lastChar  = pindah;
    } else {
        pindah->next = lineSelanjutnya->firstChar;
        lineSelanjutnya->firstChar->prev = pindah;
        lineSelanjutnya->firstChar = pindah;
    }
    lineSelanjutnya->length++;

    handleOverflow(TT,lineSelanjutnya );
}

			

void newline(TabNode *TT) {
    if (TT == NULL || TT->currLine == NULL) return;

    LineNode *LineKursor = TT->currLine;
    addLine(TT, LineKursor);
    LineNode *newLL = LineKursor->down;
    newLL->isNewLine = true;

    
   if (LineKursor->firstChar != NULL) {
        CharNode *charPindah = NULL;
        if (TT->currChar == NULL) {
            charPindah = LineKursor->firstChar;
            LineKursor->firstChar = NULL;
            LineKursor->lastChar = NULL;
            
        } else if (TT->currChar->next != NULL) {
            charPindah = TT->currChar->next;
            TT->currChar->next = NULL;
            LineKursor->lastChar = TT->currChar;
        }

        if (charPindah != NULL) {
            charPindah->prev = NULL; 
            newLL->firstChar = charPindah; 

            CharNode *temp = charPindah;
            int countPindah = 0;
            while (temp != NULL) {
                countPindah++;
                newLL->lastChar = temp; 
                temp = temp->next;
            }

            newLL->length = countPindah;
            LineKursor->length = LineKursor->length - countPindah;
        }
    }
    
    TT->currLine = newLL;   
    TT->currChar = NULL;    
    TT->cursorX = 1;       
    TT->cursorY++;        
    TT->targetX = TT->cursorX;
}

void addLine(TabNode *TT, LineNode *Linetarget) {
    if (TT == NULL) return;

    LineNode *LineBaru = AlokasiLine();
    if (LineBaru == NULL) return;

    if (Linetarget != NULL) {
        LineBaru->up = Linetarget; 
        LineBaru->down = Linetarget->down;

        if (Linetarget->down != NULL) {
            Linetarget->down->up = LineBaru;
        }
        
        // Sambungkan target ke baris baru
        Linetarget->down = LineBaru;//jadi target downnya itu nunjuk kell

    } else {
        // Skenario Cadangan: Jika dokumen masih kosong atau sisip di paling atas (Head)
        LineBaru->up = NULL;
        LineBaru->down = TT->firstLine;

        if (TT->firstLine != NULL) {
            TT->firstLine->up = LineBaru;
        }
        
        TT->firstLine = LineBaru;
        
        // Pastikan topLine (Viewport) ikut terarah jika ini baris pertama
        if (TT->topLine == NULL) {
            TT->topLine = LineBaru;
        }
    }
}

void saveFile(TabNode *TT) {
    if (TT == NULL || TT->firstLine == NULL) {
        return;
    }

    if (TT->fileName[0] == '\0') {
        clearScreen();
        renderHeader(&E);
        moveCursor(1, 1);
        printf("Masukkan nama file untuk menyimpan (contoh: rama.txt): wajib mengunakan ekstensi .txt");
        scanf(" %259s", TT->fileName); 
    }

    FILE *file = fopen(TT->fileName, "w");
    if (file == NULL) {
        redrawText(TT);
        return;
    }

    LineNode *lineSekarang = TT->firstLine;
    while (lineSekarang != NULL) {
        CharNode *charSekarang = lineSekarang->firstChar;
        while (charSekarang != NULL) {
            fputc(charSekarang->data, file);
            charSekarang = charSekarang->next;
        }
        if (lineSekarang->isNewLine && lineSekarang->down != NULL) {
            fputc('\n', file);
        }
        lineSekarang = lineSekarang->down;
    }

    fclose(file);
    moveCursor(TT->cursorY, TT->cursorX);
}


void saveAs(TabNode *TT) {
    if (TT == NULL || TT->firstLine == NULL) {
        return;
    }

    char namaFileBaru[260];

    clearScreen();
    renderHeader(&E);
    moveCursor(1, 1);
    printf("Masukkan nama file baru (contoh: rama.txt): wajib menggunakan ekstensi.txt");
    scanf(" %259s", namaFileBaru);

    FILE *file = fopen(namaFileBaru, "w");
    if (file == NULL) {
        redrawText(TT);
        return;
    }

    LineNode *lineSekarang = TT->firstLine;
    while (lineSekarang != NULL) {
        CharNode *charSekarang = lineSekarang->firstChar;
        while (charSekarang != NULL) {
            fputc(charSekarang->data, file);
            charSekarang = charSekarang->next;
        }
        if (lineSekarang->isNewLine && lineSekarang->down != NULL) {
            fputc('\n', file);
        }
        lineSekarang = lineSekarang->down;
    }

    fclose(file);
    strcpy(TT->fileName, namaFileBaru);
    moveCursor(TT->cursorY, TT->cursorX);
}