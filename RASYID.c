#include <stdio.h>
#include "CONFIG.h"
#include "RASYID.h"

// cek apakah CTRL ditekan
int isCtrlPressed() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000);
}

// cek apakah ALT ditekan
int isAltPressed() {
    return (GetAsyncKeyState(VK_MENU) & 0x8000);
}

// SET

void setStructEditor(Editor *E) {
    E->activeTab = NULL;
    E->n_tabs = 0;
    E->curr_tab = 0;
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

// CURSOR

void hideCursor() {
    printf("\033[?25l");
}

void showCursor() {
    printf("\033[?25h");
}

void moveCursor(int row, int col) {
    printf("\033[%d;%dH", row + 2, col);
}

// CLEAR

void clearScreen() {
   system("cls");
}

void clearRows(int start, int end, int width) {
    for(int r = start; r <= end; r++) {
        printf("\033[%d;1H", r);
        printf("%*s", width, "");
    }
}

// RENDER

void renderHeader(Editor *E) {
    printf("\033[1;1H");   // pindah ke baris 1
    printf("\033[K"); 

    printf("Text Editor Doa ibu  ");
    for(int i = 1; i <= E->n_tabs; i++) {
        if (i == E->curr_tab) printf("[Tab *%d] ", i);
        else printf("[Tab %d] ", i);
    }

    printf("\033[2;1H");
    printf("\n");

    fflush(stdout);
}

void renderScroll(TabNode *TT) {
    if (TT == NULL) return;

    LineNode *currentLine = TT->topLine;
    int screenRow = 1;

    while (currentLine != NULL && screenRow <= SCREEN_HEIGHT) {
        moveCursor(screenRow, 1); 
        
        printf("\033[K"); 

        CharNode *currentChar = currentLine->firstChar;
        while (currentChar != NULL) {
            printf("%c", currentChar->data);
            currentChar = currentChar->next;
        }

        currentLine = currentLine->down;
        screenRow++;
    }


    while (screenRow <= SCREEN_HEIGHT) {
        moveCursor(screenRow, 1);
        printf("\033[K");
        screenRow++;
    }
}

void redrawText(TabNode *TT) {
    if (TT == NULL) return;

    if (TT->cursorY > SCREEN_HEIGHT) {
        if (TT->topLine != NULL && TT->topLine->down != NULL) {
            TT->topLine = TT->topLine->down; 
            TT->topIndex++; 
            TT->cursorY = SCREEN_HEIGHT; 
        }
    } 
    
    else if (TT->cursorY < 1) {
        if (TT->topLine != NULL && TT->topLine->up != NULL) {
            TT->topLine = TT->topLine->up; 
            TT->topIndex--; 
            TT->cursorY = 1; 
        }
    }

    hideCursor();
    renderScroll(TT); 
    moveCursor(TT->cursorY, TT->cursorX); 
    
    showCursor();
}

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


// ini masih jelek
void insert(TabNode *TT, int c) {
    if (TT == NULL || TT->currLine == NULL) return;

    LineNode *LL = TT->currLine;
    CharNode *CC = AlokasiChar();
    if (CC == NULL) return;
    CC->data = c;

    if (LL->length < SCREEN_WIDTH) { 
        
        if (TT->currChar != NULL) { 

            CC->prev = TT->currChar;
            CC->next = TT->currChar->next;

            if (TT->currChar->next != NULL) TT->currChar->next->prev = CC;
            else LL->lastChar = CC;

            TT->currChar->next = CC;

        } else {
            CC->next = LL->firstChar;
            CC->prev = NULL;

            if (LL->firstChar != NULL) {
                LL->firstChar->prev = CC;
            } else {
                LL->lastChar = CC;
            }
            LL->firstChar = CC;
        }

        LL->length++;
        TT->cursorX++;
        TT->targetX = TT->cursorX;

    } else {
        addLine(TT, LL);

        LineNode *newLL = LL->down; 


        newLL->firstChar = CC;
        newLL->lastChar = CC;
        newLL->length = 1;
        newLL->isNewLine = false;

        TT->currLine = newLL; 
        TT->cursorX = 2;
        TT->cursorY++;
        TT->targetX = TT->cursorX;
    }
    
    TT->currChar = CC;
}

// masih kacaw mania yak
void newline(TabNode *TT) {
    addLine(TT, TT->currLine);
    LineNode *newLL = TT->currLine->down;
    
    newLL->isNewLine = true;

    TT->currLine = newLL; 
    TT->currChar = newLL->firstChar;
    TT->cursorX = 1;
    TT->cursorY++;
    TT->targetX = TT->cursorX;
}

void addLine(TabNode *TT, LineNode *target) {
    LineNode *LL = AlokasiLine();
    if (LL == NULL) return;

    if (target != NULL) {
        // add line di bawah target
        LL->up = target;
        LL->down = target->down;

        if (target->down != NULL) {
            target->down->up = LL;
        }

        target->down = LL;

    } else {
        // add line di atas target
        LL->up = NULL;
        LL->down = TT->topLine;

        if (TT->topLine != NULL) {
            TT->topLine->up = LL;
        }
        
        TT->topLine = LL;
    }
}

// TAB
void addTab(Editor *E) {
	if(E->n_tabs >= MAX_TABS) return; 

    TabNode *TT = AlokasiTab();
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

// INPUT HANDLER

void inputCharHandler(TabNode **TT, int c) {
    switch (c) { 
        case 13: // TOMBOL ENTER
            newline(*TT);
            redrawText(*TT);
            break;

        case 8:
            delete(*TT);
            redrawText(*TT);

        default:

            // add character
            if (c >= 32 && c <= 126) {
                if (!isAltPressed()) {
                    insert(*TT, c);
                    redrawText(*TT);
                }
            }

            break;

    }
}



void arrowKeyHandler(TabNode **TT,int c) {
    switch (c) {  
        // arrow up
        // case 72 : {
        //     // ini mmasih perlu dikembangin lagi
        //     if(TT->cursor_y> 0) {
        //         if (TT->text[TT->cursor_y-1][TT->cursor_x] != '\0') TT->cursor_y--;
        //         else {
        //             setPosToNTRow(TT, TT->cursor_y-1, 'd');
        //         }
        //     }

        //     break;
        // }

        // arrow down
        // case 80 : {
        //     if(TT->cursor_y < MAX_ROWS - 1) {
        //         if (TT->text[TT->cursor_y+1][TT->cursor_x] != '\0') TT->cursor_y++;
        //         else {
        //             setPosToNTRow(TT, TT->cursor_y+1, 'u');
        //         }
        //     }
            
        //     break;
        // }

        // arrow left 
        case 75 : {
        LineNode *LL = (*TT)->currLine;

        if((*TT)->cursorX > 1) { 
            if ((*TT)->currChar != NULL) {
                (*TT)->currChar = (*TT)->currChar->prev;
            } 

            (*TT)->cursorX--;
            (*TT)->targetX = (*TT)->cursorX;
        } else if(LL != NULL && LL->up != NULL) {
            (*TT)->currLine = LL->up;
            (*TT)->currChar = LL->up->lastChar; 
            (*TT)->cursorY--;
            (*TT)->cursorX = LL->up->length + 1;
            (*TT)->targetX = (*TT)->cursorX;
        }
        
        break;
    }

        // arrow right
        case 77 : { // ARROW RIGHT
            LineNode *LL = (*TT)->currLine;

            if((*TT)->cursorX <= LL->length) {
                if ((*TT)->currChar == NULL) {
                    (*TT)->currChar = LL->firstChar;
                } else {
                    (*TT)->currChar = (*TT)->currChar->next;
                }
                (*TT)->cursorX++;
                (*TT)->targetX = (*TT)->cursorX;
            } else if(LL->down != NULL) {
                (*TT)->currLine = LL->down;
                (*TT)->currChar = NULL;
                (*TT)->cursorY++;
                (*TT)->cursorX = 1;
                (*TT)->targetX = 1;
            }
            
            break;
        }
            
        
        default:
            break;
    }

    if((*TT)->cursorY < 1 || (*TT)->cursorY > SCREEN_HEIGHT) {
        redrawText(*TT);
    } else {
        moveCursor((*TT)->cursorY, (*TT)->cursorX);
    }
}

void DelokasiChar(CharNode *CC) {
    if(CC != NULL) free(CC);
}

void DelokasiLine(LineNode *LL) {
    if(LL != NULL) free(LL);
}

void DelokasiTab(TabNode *TT) {
    if(TT != NULL) free(TT);
}


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

void merge(LineNode *LL, int gap) {
    LineNode *nextLL = LL->down;

    if (nextLL != NULL && !LL->down->isNewLine) {
        LL->lastChar->next = nextLL->firstChar;
        nextLL->firstChar->prev = LL->lastChar;

        CharNode *tempChar = nextLL->firstChar;
        int cutLength = 1;
        
        // gap = SCREEN_WIDTH - length  
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

        // ini masih aneh
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







