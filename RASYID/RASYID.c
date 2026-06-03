#include <stdio.h>
#include "../CONFIG.h"
#include "../RAMA/rama.h"
#include "RASYID.h"

// DEALOKASI

void DelokasiChar(CharNode *CC) {
    if(CC != NULL) free(CC);
}

void DelokasiLine(LineNode *LL) {
    if(LL != NULL) free(LL);
}

void DelokasiTab(TabNode *TT) {
    if(TT != NULL) free(TT);
}

// DELETE

void delete(TabNode *TT) {
    if (TT == NULL || TT->currLine == NULL) return;
    
    LineNode *LL = TT->currLine;
    CharNode *CC = TT->currChar;

    // jika karakter tidak null (tidak di awal baris)
    if(CC != NULL) {

        // ketika baris minimal punya 1 karakter
        if(LL->length <= 1) {
            LL->firstChar = NULL;
            LL->lastChar = NULL;
            TT->currChar = NULL;

        // ketika karakter yang dihapus adalah karakter pertama
        } else if(CC == LL->firstChar) {
            LL->firstChar = CC->next;
            LL->firstChar->prev = NULL;
            TT->currChar = CC->prev;
        
        // ketika karakter yang dihapus adalah karakter terakhir
        } else if(CC == LL->lastChar) {
            LL->lastChar = CC->prev;
            LL->lastChar->next = NULL;
            TT->currChar = LL->lastChar;

        // ketika karakter yang dihapus adalah karakter tengah
        } else {
            CC->prev->next = CC->next;
            CC->next->prev = CC->prev;
            TT->currChar = CC->prev;
        }

        DelokasiChar(CC);

        TT->cursorX--;
        TT->targetX = TT->cursorX;
        LL->length--;

        // ketika karakter yang dihapus adalah karakter terakhir dan bakal pindah ke baris di atasnya
        if(LL->length <= 0 && LL->up != NULL) {

            if(LL->down != NULL) {
                LL->up->down = LL->down;
                LL->down->up = LL->up;
            } else LL->up->down = NULL;
            
            TT->currLine= LL->up;
            TT->currChar= LL->up->lastChar;

            TT->cursorY--;
            TT->cursorX = LL->up->length + 1;
            TT->targetX = TT->cursorX;

            DelokasiLine(LL);

        // merging karakter setelah dihapus
        } else {
            int gap = SCREEN_WIDTH - LL->length;
            merge(LL, gap);
        }

    // penghapusan karakter di awal baris dan baris di atasnya tidak null
    } else if(LL->up != NULL) {
        LineNode *upLine = LL->up;
        
        // jika baris saat ini adalah baris kosong
        if(LL->length <= 0) {
            upLine->down = LL->down;
            if(LL->down != NULL) LL->down->up = upLine;

            TT->currLine= LL->up;
            TT->currChar= LL->up->lastChar;

            TT->cursorY--;
            TT->cursorX = LL->up->length + 1;
            TT->targetX = TT->cursorX;

            DelokasiLine(LL);

        // jika baris di atasnya adalah baris kosong
        } else if(upLine->length <= 0) {
            if(upLine->up != NULL) {
                LL->up = upLine->up;
                upLine->up->down = LL;
            } else {
                LL->up = NULL;
                TT->firstLine = LL;
                TT->currLine = TT->firstLine;
            }

            TT->currLine= LL;
            TT->currChar= LL->firstChar->prev;

            TT->cursorY--;
            TT->cursorX = 1;
            TT->targetX = TT->cursorX;

            DelokasiLine(upLine);
        
        // jika baris saat ini tidak kosong dan baris di atasnya tidak kosong
        } else {

            // jika baris saat ini bukan baris independen
            if(!LL->isNewLine) {       
                CharNode *currLastCC = upLine->lastChar; 
                upLine->lastChar = currLastCC->prev;
                currLastCC->prev->next = NULL;
                DelokasiChar(currLastCC);

                upLine->length--;
            } else LL->isNewLine = false;

            // merging ke baris atas
            if(upLine->length < SCREEN_WIDTH) {
                TT->currLine = upLine;
                TT->currChar = upLine->lastChar;

                TT->cursorY--;
                TT->cursorX = upLine->length + 1;
                TT->targetX = TT->cursorX;

                int gap = SCREEN_WIDTH - upLine->length;
                merge(upLine, gap);
            }
        }
    }
}

void delLine(TabNode *TT) {
    if (TT == NULL || TT->currLine == NULL) return;

    LineNode *LL = TT->currLine;
    CharNode *tempCC = LL->firstChar;
    LL->firstChar = NULL;
    LL->lastChar = NULL;
    LL->length = 0;

    // hapus semua karakter di dalam baris 
    while(tempCC != NULL) {
        CharNode *nextCC = tempCC->next;
        DelokasiChar(tempCC);
        tempCC = nextCC;
    }

    if(LL->up != NULL) {
        // hapus baris yang di atas dan di bawah baris tersebut ada baris
        if(LL->down != NULL) {
            LL->down->up = LL->up;
            LL->up->down = LL->down;
            TT->currLine = LL->down;

        // hapus baris yang di atas baris tersebut ada baris dan di bawahnya tidak ada baris
        } else {
            TT->cursorY--;
            TT->currLine = LL->up;
            LL->up->down = NULL;            
        }
    } else {
        // hapus baris yang di bawah baris tersebut ada baris dan di atasnya tidak ada baris
        if(LL->down != NULL) {
            TT->firstLine = LL->down;
            TT->currLine = TT->firstLine;
            TT->topLine = TT->currLine;
            LL->down-> up = NULL;
        }
    }

    // jika baris ada baris independent
    if(LL->isNewLine && LL->down != NULL) {
        LL->down->isNewLine = true;
    }

    // jika tab memiliki lebih dari 1 baris 
    if(LL->up != NULL || LL->down != NULL) {
        DelokasiLine(LL);
    }

    TT->currChar = TT->currLine->firstChar;
    TT->cursorX = 1;
    TT->targetX = TT->cursorX;
}

// MERGE

void merge(LineNode *LL, int gap) {
    LineNode *nextLL = LL->down;

    // Baris di bawah yang selalu dimerge ke baris atasnya dan baris yang dimerge hanya baris di bawah dengan status baris adalah baris dependent
    if (nextLL != NULL && !nextLL->isNewLine) {
        LL->lastChar->next = nextLL->firstChar;
        nextLL->firstChar->prev = LL->lastChar;

        // proses pemindahan karakter ke baris di atasnya
        CharNode *tempChar = nextLL->firstChar;
        int cutLength = 1;
        
        while(cutLength < gap) {
            if(tempChar->next == NULL) break;
            tempChar = tempChar->next;
            cutLength++;
        }

        LL->lastChar = tempChar;
        nextLL->firstChar = tempChar->next;
        
        if (nextLL->firstChar != NULL) {
            nextLL->firstChar->prev = NULL;
        }

        LL->lastChar->next = NULL;
        LL->length = LL->length + cutLength;
        nextLL->length = nextLL->length - cutLength;

        // jika baris jadi kosong setelah di merge ke baris atas karakter karakternya
        if(nextLL->length <= 0) {
            if(nextLL->down != NULL) {
                LL->down = nextLL->down;
                nextLL->down->up = LL;
            } else LL->down = NULL;
            
            DelokasiLine(nextLL);

        // jika setelah baris di merge ke atas, ternyata baris jadi ada space dan dibawahnya ada baris dependent, maka lakukan merge lagi
        } else {
            int newGap = SCREEN_WIDTH - nextLL->length;
            merge(nextLL, newGap);
        }
    }
}

void deleteTab(Editor *E) {
    TabNode *TT = E->activeTab;
    if (TT == NULL) return;

    // mengatur tunjuk menunjuk pointer tab
    if(TT->prev != NULL) {
        if(TT->next != NULL) {
            TT->prev->next = TT->next;
            TT->next->prev = TT->prev;
            E->activeTab = TT->next;
        } else {
            TT->prev->next = NULL;
            E->activeTab = TT->prev;
            E->curr_tab--;
        }
    } else {
        if(TT->next != NULL) {
            TT->next->prev = NULL;
            E->activeTab = TT->next;
        }
    }

    LineNode *tempLine = TT->firstLine;
    TT->fileName[0] = '\0';

    TT->firstLine = NULL;
    TT->currLine = NULL;
    TT->currChar = NULL;
    TT->topLine = NULL;

    TT->targetX = 1;
    TT->cursorX = 1;
    TT->cursorY = 1;
    TT->topIndex = 1;

    // hapus semua karakter dan baris
    while(tempLine != NULL) {
        LineNode *nextTempLine = tempLine->down;
        if (nextTempLine != NULL) {
            nextTempLine->up = NULL; 
        }

        CharNode *tempChar = tempLine->firstChar;
        resetLine(tempLine);

        while (tempChar != NULL){
            CharNode *nextTempChar = tempChar->next;
            if (nextTempChar != NULL) {
                nextTempChar->prev = NULL;
            }


            resetChar(tempChar);
            DelokasiChar(tempChar);
            tempChar = nextTempChar;
        }
        
        DelokasiLine(tempLine);
        tempLine = nextTempLine;
    }

    if(TT->prev != NULL || TT->next != NULL) {
        DelokasiTab(TT);
        E->n_tabs--;
    } else {
        LineNode *LL = AlokasiLine();

        if(LL == NULL) return;

        TT->firstLine = LL;
        TT->currLine = LL;
        TT->topLine = LL;
        LL->isNewLine = true;

        E->activeTab = TT;
    }
}