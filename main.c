#include <stdio.h>
#include "ui.h"

int main(void) {
    ui_initConsole();   /* fullscreen + warna cyan */
    ui_run();           /* alur utama */

    clearScreen();
    setCursorVisible(1);
    printf("Program selesai.\n");
    return 0;
}
