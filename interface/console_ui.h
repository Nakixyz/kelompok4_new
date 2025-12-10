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

/* ---------- Loading Screen Umum (fullscreen + progress bar) ---------- */
static void ui_showLoading(const char *title, const char *subtitle) {
    ui_updateSize();
    clearScreen();
    setCursorVisible(0);

    /* frame luar */
    drawBox(0, 0, g_cols, g_rows, "");

    /* ukuran box loading di tengah */
    int boxW = 60;
    int boxH = 8;
    if (boxW > g_cols - 4) boxW = g_cols - 4;
    if (boxH > g_rows - 4) boxH = g_rows - 4;

    int left = (g_cols - boxW) / 2;
    int top  = (g_rows - boxH) / 2;

    drawBox(left, top, boxW, boxH, "");

    /* judul */
    if (title && title[0] != '\0') {
        int tlen = (int)strlen(title);
        int tx = left + (boxW - tlen) / 2;
        if (tx < left + 2) tx = left + 2;
        gotoXY((short)tx, (short)(top + 1));
        printf("%s", title);
    }

    /* sub judul */
    if (subtitle && subtitle[0] != '\0') {
        int slen = (int)strlen(subtitle);
        int sx = left + (boxW - slen) / 2;
        if (sx < left + 2) sx = left + 2;
        gotoXY((short)sx, (short)(top + 2));
        printf("%s", subtitle);
    }

    /* progress bar */
    int barWidth = boxW - 10;
    if (barWidth < 10) barWidth = 10;
    int barLeft = left + (boxW - barWidth) / 2;
    int barTop  = top + 4;

    int totalSteps = barWidth - 2;   /* bagian dalam [=====] */

    for (int step = 0; step <= totalSteps; ++step) {
        /* gambar outline bar */
        gotoXY((short)barLeft, (short)barTop);
        putchar('[');
        for (int i = 0; i < barWidth - 2; ++i) {
            if (i < step) putchar('#');
            else          putchar(' ');
        }
        putchar(']');

        /* persentase di bawah bar */
        int percent = (step * 100) / (totalSteps == 0 ? 1 : totalSteps);
        char pctStr[8];
        snprintf(pctStr, sizeof(pctStr), "%3d%%", percent);

        int pctLen = (int)strlen(pctStr);
        int px = barLeft + (barWidth - pctLen) / 2;
        gotoXY((short)px, (short)(barTop + 1));
        printf("%s", pctStr);

        Sleep(40); /* kecepatan animasi */
    }
}


#endif /* CONSOLE_UI_H */
