#include "./CONFIG.h"
#include "./RASYID/RASYID.h"
#include "./RAMA/rama.h"
#include "./CALLISTA/callista.h"

Editor E;

int main() {
    setStructEditor(&E);
    addTab(&E);

    clearScreen();
    renderHeader(&E);

    while(1) {
        int c = _getch();
        TabNode **TT = &E.activeTab;
        
        if (c == 0 || c == 224) {
            c = _getch();
            arrowKeyHandler(TT, c);
        } else {
            inputCharHandler(TT, c);
        }
    }

    return 0;
}