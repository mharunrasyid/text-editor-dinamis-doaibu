#include <stdio.h>
#include "CONFIG.h"
#include "rasyid.h"
#include "rama.h"

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
    
    
	TT->firstLine = NULL;
    TT->currLine = NULL;
    TT->currChar = NULL;
    TT->topLine = NULL;

    TT->cursorX = 1;
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


// TAB

TabNode *AlokasiTab() {
    TabNode *TT = (TabNode *) malloc (sizeof(TabNode));

    if (TT != NULL) {
		resetTab(TT);
		
		return (TT);
	} else return NULL;
}


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
        case 19: // TOMBOL CTRL + S (Save Menanyakan)
            saveFile(*TT);
            break;

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
            (*TT)->cursorX = (*TT)->cursorX;
        } else if(LL != NULL && LL->up != NULL) {
            (*TT)->currLine = LL->up;
            (*TT)->currChar = LL->up->lastChar; 
            (*TT)->cursorY--;
            (*TT)->cursorX = LL->up->length + 1;
            (*TT)->cursorX = (*TT)->cursorX;
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
                (*TT)->cursorX = (*TT)->cursorX;
            } else if(LL->down != NULL) {
                (*TT)->currLine = LL->down;
                (*TT)->currChar = NULL;
                (*TT)->cursorY++;
                (*TT)->cursorX = 1;
                (*TT)->cursorX = 1;
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







