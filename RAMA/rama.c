#include "rama.h"
#include "../CALLISTA/callista.h"
#include "../RASYID/RASYID.h"

void setStructEditor() {
    E.activeTab = NULL;
    E.n_tabs = 0;
    E.curr_tab = 0;
}

//alokasi
TabNode *AlokasiTab() {
    TabNode *TT = (TabNode *) malloc (sizeof(TabNode));

    if (TT != NULL) {
		resetTab(TT);
		
		return (TT);
	} else return NULL;
}

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

// RESET
void resetTab(TabNode *TT) {
    TT->fileName[0] = '\0';

    TT->currLine = NULL;
    TT->currChar = NULL;
    TT->topLine = NULL;

    TT->targetX = 1;
    TT->cursorX = 1;
    TT->cursorY = 1;
    TT->topIndex = 1;

    TT->isModified = false;

    TT->next = NULL;
    TT->prev = NULL;
}

void resetLine(LineNode *LL) {
    LL->firstChar = NULL;
    LL->lastChar = NULL;
    LL->up = NULL;
    LL->down = NULL;

    LL->length = 0;
    LL->isNewLine = false;
}

void resetChar(CharNode *CC) {
    CC->data = '\0';
    CC->next = NULL;
    CC->prev = NULL;
}

//addtab
void addTab(Editor *E, TabNode *TT) {
	if(E->n_tabs >= MAX_TABS) return; 

    LineNode *LL = AlokasiLine();

    if(TT == NULL || LL == NULL) {
        if(TT) free(TT);
        if(LL) free(LL);
        return;
    }

    TT->firstLine = LL;
    TT->currLine = LL;
    TT->topLine = LL;
    LL->isNewLine = true;

    if(E->activeTab != NULL) {
        E->activeTab->next = TT;
        TT->prev = E->activeTab;
    }

    E->activeTab = TT;
    E->n_tabs++;
    E->curr_tab = E->n_tabs;
}


//insert
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

    LineNode *lineSelanjutnya = line->down;
    if (lineSelanjutnya == NULL || lineSelanjutnya->isNewLine == true) {
        LineNode *baru = AlokasiLine();
        if (baru == NULL) return;
        baru->isNewLine = false; 
        baru->up = line;
        baru->down = lineSelanjutnya;
        line->down = baru;
        if (lineSelanjutnya != NULL) lineSelanjutnya->up = baru;
        lineSelanjutnya = baru;
    }

    CharNode *pindah = line->lastChar;
    pindah->prev->next = NULL;
    line->lastChar = pindah->prev;
    pindah->prev = NULL;
    line->length--;


    if (TT->currChar == pindah) {
        TT->currLine = lineSelanjutnya;
        TT->currChar = pindah; 
        TT->cursorY++;
        TT->cursorX = 2;       
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

			
//NEWLINE
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
        
        Linetarget->down = LineBaru;

    } else {
        LineBaru->up = NULL;
        LineBaru->down = TT->firstLine;

        if (TT->firstLine != NULL) {
            TT->firstLine->up = LineBaru;
        }
        
        TT->firstLine = LineBaru;
        if (TT->topLine == NULL) {
            TT->topLine = LineBaru;
        }
    }
}

//savefile
void saveFile(TabNode *TT) {
    if (TT == NULL || TT->firstLine == NULL) {
        return;
    }

    if (TT->fileName[0] == '\0') {
        clearScreen();
        renderHeader(&E);
        moveCursor(1, 1);
        printf("Masukkan nama file untuk menyimpan (contoh: rama.txt): wajib mengunakan ekstensi .txt");
        scanf(" %260s", TT->fileName); 
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
    TT->isModified = false;
    moveCursor(TT->cursorY, TT->cursorX);
}


void saveAs(TabNode *TT) {
    if (TT == NULL || TT->firstLine == NULL) {
        return;
    }

    char namaFileBaru[260];

    clearScreen();
    renderHeader();
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
    TT->isModified = false;
    strcpy(TT->fileName, namaFileBaru);
    moveCursor(TT->cursorY, TT->cursorX);
}