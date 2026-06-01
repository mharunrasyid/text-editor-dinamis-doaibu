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
            (*TT)->isModified = true;
            redrawText(*TT);
            break;
        case 8: //CTRL + H (Backspace)
            delete(*TT);
            (*TT)->isModified = true;
            redrawText(*TT);    
			break;    
        case 18: // CTRL + R (Replace)
            replaceHandler(TT);
            break;    
        case 19: // TOMBOL CTRL + S (Save Menanyakan)
            saveFile(*TT);
            break;
        

        default:
            if (c >= 32 && c <= 126) {
                if (!isAltPressed()) {
                    insert(*TT, c);
                    (*TT)->isModified = true;
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

// Read File
void readFile(TabNode *TT, const char *fileName) {
    if (TT == NULL) return;

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("[Error] Gagal membuka file: %s\n", fileName);
        return;
    }

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            newline(TT);
        } else if (c >= 32 && c <= 126) {
            insert(TT, (char)c);
        }
    }

    TT->currLine = TT->firstLine;
    TT->currChar = NULL;
    TT->cursorX  = 1;
    TT->cursorY  = 1;
    TT->targetX  = 1;
    TT->topLine  = TT->firstLine;
    TT->topIndex = 1;

    TT->isModified = false;

    fclose(file);
}

// Match Word for Replace
static CharNode *matchWord(CharNode *cc, const char *word) {
    CharNode *temp = cc;
    int i;
    for (i = 0; i < strlen(word); i++) {
        if (temp == NULL || tolower(temp->data) != tolower(word[i])) {
            return NULL;
        }
        temp = temp->next;
    }
    return temp; 
}

// Replace 
void replaceOne(TabNode *TT, const char *oldWord, const char *newWord) {
    if (TT == NULL || oldWord == NULL || newWord == NULL) return;

    int oldLen = strlen(oldWord);
    LineNode *line = TT->firstLine;

    while (line != NULL) {
        CharNode *cc = line->firstChar;

        while (cc != NULL) {
            CharNode *next = cc->next;

            if (matchWord(cc, oldWord) != NULL) {

                TT->currLine = line;
                TT->currChar = cc->prev;
                TT->cursorX  = TT->currChar == NULL ? 1 : TT->cursorX;

                int i;
                for (i = 0; i < oldLen; i++) {
                    TT->currChar = cc;
                    CharNode *toDelete = cc;
                    cc = cc->next;
                    delete(TT);
                }

                for (i = 0; i < strlen(newWord); i++) {
                    insert(TT, newWord[i]);
                }

                TT->isModified = true;
                return; 
            }
            cc = next;
        }
        line = line->down;
    }
}

void replaceAll(TabNode *TT, const char *oldWord, const char *newWord) {
    if (TT == NULL || oldWord == NULL || newWord == NULL) return;

    int oldLen = strlen(oldWord);
    LineNode *line = TT->firstLine;

    while (line != NULL) {
        CharNode *cc = line->firstChar;

        while (cc != NULL) {
            if (matchWord(cc, oldWord) != NULL) {
                TT->currLine = line;
                TT->currChar = cc->prev;

                int i;
                for (i = 0; i < oldLen; i++) {
                    TT->currChar = cc;
                    CharNode *toDelete = cc;
                    cc = cc->next;
                    delete(TT);
                }

                for (i = 0; i < strlen(newWord); i++) {
                    insert(TT, newWord[i]);
                }
            } else {
                cc = cc->next;
            }
        }
        line = line->down;
    }

    TT->isModified = true;
}

void replaceHandler(TabNode **TT) {
    char oldWord[100];
    char newWord[100];
    char pilihan[4];

    clearScreen();
    printf("=== FIND & REPLACE ===\n");
    printf("Kata yang dicari  : ");
    fgets(oldWord, sizeof(oldWord), stdin);
    oldWord[strcspn(oldWord, "\n")] = 0;

    printf("Kata pengganti    : ");
    fgets(newWord, sizeof(newWord), stdin);
    newWord[strcspn(newWord, "\n")] = 0;

    printf("Replace (o)ne / (a)ll? : ");
    fgets(pilihan, sizeof(pilihan), stdin);

    if (pilihan[0] == 'o' || pilihan[0] == 'O') {
        replaceOne(*TT, oldWord, newWord);
    } else if (pilihan[0] == 'a' || pilihan[0] == 'A') {
        replaceAll(*TT, oldWord, newWord);
    } else {
        printf("[Info] Dibatalkan.\n");
        return;
    }

    renderHeader();
    redrawText(*TT);
}

// Load File
void loadFile() {
    char fileName[MAX_PATH];
    clearScreen();
    printf("Enter the file name to load: ");
    fgets(fileName, sizeof(fileName), stdin);
    fileName[strcspn(fileName, "\n")] = 0;

    if (strlen(fileName) == 0) {
        printf("[Error] Filename is still empty.\n");
        return;
    }

    TabNode *temp = E.activeTab;
    if (temp != NULL) {
        while (temp->prev != NULL) temp = temp->prev;
        while (temp != NULL) {
            if (strcmp(temp->fileName, fileName) == 0) {
                E.activeTab = temp;
                renderHeader();
                redrawText(E.activeTab);
                printf("\n[Info] File '%s' has been opened. Switching tab. \n", fileName);
                return;
            }
            temp = temp->next;
        }
    }
    if (E.n_tabs >= MAX_TABS) {
        printf("[Error] Maximum tab (%d) reached. Close a tab first.\n", MAX_TABS);
        return;
    }
    FILE *check = fopen(fileName, "r");
    if (check == NULL) {
        printf("[Error] File '%s' not found.\n", fileName);
        return;
    }
    
    fclose(check);

    TabNode *newTab = createTab(fileName);
    if (newTab != NULL) {
        newTab->isModified = false;  
        addTab(newTab);
        readFile(newTab, fileName);
        E.activeTab = newTab;
        renderHeader();
        redrawText(E.activeTab);
    } else {
        printf("[Error] Failed to open file: %s\n", fileName);
    }
}

void quitEditor() {
    char input[8];

    TabNode *temp = E.activeTab;
    if (temp != NULL) {
        while (temp->prev != NULL) temp = temp->prev;
    }

    bool anyUnsaved = false;
    TabNode *check = temp;
    while (check != NULL) {
        if (check->isModified) {
            anyUnsaved = true;
            break;
        }
        check = check->next;
    }

    if (anyUnsaved) {
        clearScreen();
        printf("[Warning] Unsaved changes!\n");
        printf("Proceed to exit? (y/n): ");
        fgets(input, sizeof(input), stdin);
        if (input[0] != 'y' && input[0] != 'Y') {
            return;
        }
    }
    clearScreen();
    printf("Exiting Doa Ibu's Editor. See you later!\n");
    exit(0);
}