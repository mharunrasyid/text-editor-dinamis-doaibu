#include <stdio.h>
#include <stdlib.h>
#include "../CONFIG.h"
#include "../RASYID/RASYID.h"
#include "../RAMA/rama.h"
#include "callista.h"


// CLEAR

void clearScreen() {
   system("cls");
}

void clearRows(int start, int end, int width) {
    int r;
    for(r = start; r <= end; r++) {
        printf("\033[%d;1H", r);
        printf("%*s", width, "");
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

// RENDER
void renderHeader() {
    printf("\033[1;1H\033[K"); 
    printf(" [DOA IBU'S EDITOR]  |  ");

    TabNode *tempTab = E.activeTab; 
    if (tempTab == NULL) {
        printf("No Tabs Open");
        return;
    }

    while (tempTab->prev != NULL) {
        tempTab = tempTab->prev; 
    }

    while (tempTab != NULL) {
        if (tempTab == E.activeTab) {
            printf("\033[1;32m[%s *]\033[0m ", tempTab->fileName); 
        } else {
            printf("[%s] ", tempTab->fileName); 
        }
        tempTab = tempTab->next; 
    }
    printf("\n\033[K----------------------------------------------------------------------\n");
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

// cek apakah CTRL ditekan
int isCtrlPressed() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000);
}

// cek apakah ALT ditekan
int isAltPressed() {
    return (GetAsyncKeyState(VK_MENU) & 0x8000);
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
        case 72 : {
            if ((*TT)->currLine != NULL && (*TT)->currLine->up != NULL) {
                (*TT)->currLine = (*TT)->currLine->up;
                (*TT)->cursorY--;

                if ((*TT)->targetX > (*TT)->currLine->length + 1) {
                    (*TT)->cursorX = (*TT)->currLine->length + 1;
                } else {
                    (*TT)->cursorX = (*TT)->targetX;
                }

                (*TT)->currChar = NULL;
                CharNode *temp = (*TT)->currLine->firstChar;
                int i;
                for (i = 1; i < (*TT)->cursorX && temp != NULL; i++) {
                    (*TT)->currChar = temp;
                    temp = temp->next;
                }
            }
            break;
        }

        // arrow down
        case 80 : {
            if ((*TT)->currLine != NULL && (*TT)->currLine->down != NULL) {
                (*TT)->currLine = (*TT)->currLine->down;
                (*TT)->cursorY++;
                if ((*TT)->targetX > (*TT)->currLine->length + 1) {
                    (*TT)->cursorX = (*TT)->currLine->length + 1;
                } else {
                    (*TT)->cursorX = (*TT)->targetX;
                }

                (*TT)->currChar = NULL;
                CharNode *temp = (*TT)->currLine->firstChar;
                int i;
                for (i = 1; i < (*TT)->cursorX && temp != NULL; i++) {
                    (*TT)->currChar = temp;
                    temp = temp->next;
                }
            }
            break;
        }

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