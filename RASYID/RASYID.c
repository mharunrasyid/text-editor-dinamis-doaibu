#include <stdio.h>
#include "../CONFIG.h"
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

    if(CC != NULL) {
        if(LL->length <= 1) {
            LL->firstChar = NULL;
            LL->lastChar = NULL;
            TT->currChar = NULL;

        } else if(CC == LL->firstChar) {
            LL->firstChar = CC->next;
            LL->firstChar->prev = NULL;
            TT->currChar = CC->prev;
        } else if(CC == LL->lastChar) {
            LL->lastChar = CC->prev;
            LL->lastChar->next = NULL;
            TT->currChar = LL->lastChar;
        } else {
            CC->prev->next = CC->next;
            CC->next->prev = CC->prev;
            TT->currChar = CC->prev;
        }

        DelokasiChar(CC);

        TT->cursorX--;
        TT->targetX = TT->cursorX;
        LL->length--;

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
        } else {
            int gap = SCREEN_WIDTH - LL->length;
            merge(LL, gap);
        }

    } else if(LL->up != NULL) {
        LineNode *upLine = LL->up;
        
        if(LL->length <= 0) {
            upLine->down = LL->down;
            if(LL->down != NULL) LL->down->up = upLine;

            TT->currLine= LL->up;
            TT->currChar= LL->up->lastChar;

            TT->cursorY--;
            TT->cursorX = LL->up->length + 1;
            TT->targetX = TT->cursorX;

            DelokasiLine(LL);
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
        } else {
            if(!LL->isNewLine) {       
                CharNode *currLastCC = upLine->lastChar; 
                upLine->lastChar = currLastCC->prev;
                currLastCC->prev->next = NULL;
                DelokasiChar(currLastCC);

                upLine->length--;
            } else LL->isNewLine = false;

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

// MERGE

void merge(LineNode *LL, int gap) {
    LineNode *nextLL = LL->down;

    if (nextLL != NULL && !LL->down->isNewLine) {
        LL->lastChar->next = nextLL->firstChar;
        nextLL->firstChar->prev = LL->lastChar;

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

        if(nextLL->length <= 0) {
            if(nextLL->down != NULL) {
                LL->down = nextLL->down;
                nextLL->down->up = LL;
            } else LL->down = NULL;
            
            DelokasiLine(nextLL);
        } else {
            int newGap = SCREEN_WIDTH - nextLL->length;
            merge(nextLL, newGap);
        }
    }
}