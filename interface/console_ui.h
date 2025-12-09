#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

/* ukuran window (diperbarui dinamis) */
static SHORT g_cols = 120;
static SHORT g_rows = 30;

/* ---------- Utility console dasar ---------- */

static void clearScreen() {
    system("cls");
}

static void gotoXY(short x, short y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(h, pos);
}

static void setCursorVisible(int visible) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(h, &info);
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(h, &info);
}

/* update g_cols & g_rows dengan ukuran window aktif */
static void ui_updateSize() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        g_cols = (SHORT)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        g_rows = (SHORT)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    }
}

/* box sederhana dengan judul opsional */
static void drawBox(int left, int top, int width, int height, const char *title) {
    int i;

    if (width < 2 || height < 2) return;

    gotoXY((short)left, (short)top);
    putchar('+');
    for (i = 0; i < width - 2; ++i) putchar('-');
    putchar('+');

    for (i = 1; i < height - 1; ++i) {
        gotoXY((short)left, (short)(top + i));
        putchar('|');
        gotoXY((short)(left + width - 1), (short)(top + i));
        putchar('|');
    }

    gotoXY((short)left, (short)(top + height - 1));
    putchar('+');
    for (i = 0; i < width - 2; ++i) putchar('-');
    putchar('+');

    if (title && title[0] != '\0') {
        gotoXY((short)(left + 2), (short)top);
        printf(" %s ", title);
    }
}

/* kotak input 1 baris (label di luar) */
static void drawInputBox(int left, int top, int width) {
    int i;
    if (width < 2) return;

    gotoXY((short)left, (short)top);
    putchar('+');
    for (i = 0; i < width - 2; ++i) putchar('-');
    putchar('+');

    gotoXY((short)left, (short)(top + 1));
    putchar('|');
    for (i = 0; i < width - 2; ++i) putchar(' ');
    putchar('|');

    gotoXY((short)left, (short)(top + 2));
    putchar('+');
    for (i = 0; i < width - 2; ++i) putchar('-');
    putchar('+');
}

/* ---------- Input helper umum ---------- */

/* input text biasa (username, nama, dsb), ESC mengembalikan 0, Enter = 1 */
static int inputText(char *buffer, int maxLen) {
    int len = 0;
    buffer[0] = '\0';

    while (1) {
        int ch = _getch();

        if (ch == 0 || ch == 224) {
            _getch();  /* ignore tombol arah/fungsi */
            continue;
        }

        if (ch == 27) { /* ESC */
            buffer[0] = '\0';
            return 0;
        } else if (ch == 13) { /* Enter */
            buffer[len] = '\0';
            return 1;
        } else if (ch == 8) { /* Backspace */
            if (len > 0) {
                len--;
                buffer[len] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        } else if (ch >= 32 && ch <= 126) { /* karakter biasa */
            if (len < maxLen - 1) {
                buffer[len++] = (char) ch;
                buffer[len] = '\0';
                putchar(ch);
                fflush(stdout);
            }
        }
    }
}

/* input password dengan *** + TAB untuk see/unsee, ESC=0, Enter=1 */
static int inputPassword(char *buffer, int maxLen) {
    int len = 0;
    int show = 0;
    buffer[0] = '\0';

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(h, &csbi);
    short startX = csbi.dwCursorPosition.X;
    short startY = csbi.dwCursorPosition.Y;

    while (1) {
        int ch = _getch();

        if (ch == 0 || ch == 224) {
            _getch();   /* ignore tombol khusus (arrow, function) */
            continue;
        }

        if (ch == 27) { /* ESC */
            buffer[0] = '\0';
            return 0;
        } else if (ch == 13) { /* Enter */
            buffer[len] = '\0';
            return 1;
        } else if (ch == 9) { /* TAB -> toggle show/unsee */
            int i;
            show = !show;
            gotoXY(startX, startY);
            for (i = 0; i < len; ++i) putchar(' ');
            gotoXY(startX, startY);

            if (show) {
                for (i = 0; i < len; ++i) putchar(buffer[i]);
            } else {
                for (i = 0; i < len; ++i) putchar('*');
            }
            fflush(stdout);
        } else if (ch == 8) { /* Backspace */
            if (len > 0) {
                int i;
                len--;
                buffer[len] = '\0';
                gotoXY(startX, startY);
                if (show) {
                    for (i = 0; i < len; ++i) putchar(buffer[i]);
                } else {
                    for (i = 0; i < len; ++i) putchar('*');
                }
                putchar(' ');
                gotoXY(startX + len, startY);
                fflush(stdout);
            }
        } else if (ch >= 32 && ch <= 126) {
            if (len < maxLen - 1) {
                int i;
                buffer[len++] = (char) ch;
                buffer[len] = '\0';
                if (show) {
                    gotoXY(startX, startY);
                    for (i = 0; i < len; ++i) putchar(buffer[i]);
                } else {
                    putchar('*');
                }
                fflush(stdout);
            }
        }
    }
}

/* ---------- Loading kecil ---------- */

static void ui_showLoading(const char *roleLabel) {
    ui_updateSize();
    clearScreen();

    int outerLeft = 0, outerTop = 0;
    int outerWidth = g_cols;
    int outerHeight = g_rows;

    drawBox(outerLeft, outerTop, outerWidth, outerHeight, "");

    int boxW = 34;
    int boxH = 5;
    int left = outerLeft + (outerWidth - boxW) / 2;
    int top  = outerTop + (outerHeight - boxH) / 2;

    drawBox(left, top, boxW, boxH, "");

    for (int i = 0; i <= 6; ++i) {
        gotoXY((short)(left + 2), (short)(top + 2));
        printf("Loading %s", roleLabel);
        int dots = i % 4;
        for (int d = 0; d < dots; ++d) putchar('.');
        fflush(stdout);
        Sleep(250);
    }
}

#endif /* CONSOLE_UI_H */
