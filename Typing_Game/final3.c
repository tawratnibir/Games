#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void initialize_colors() {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
}

void display_results(int typed, int mistakes, time_t start_time, time_t end_time, int x) {
    attron(COLOR_PAIR(4));
    float timeTaken = difftime(end_time, start_time);
    printw("\nTyped: %d\nMistakes: %d\n", typed, mistakes);
    printw("Time taken: %.2f s\n", timeTaken);
    printw("WPM: %.2f\n", (60.0 * (((float)typed - mistakes) / (4.0 * x))) / timeTaken);
    printw("CPS: %.2f\n", (((((float)typed - mistakes)) / timeTaken)));
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(2));
    printw("Accuracy: %.2f%%\n", 100.0 - ((float)mistakes / typed) * 100);
    attroff(COLOR_PAIR(2));
    refresh();
}

void game_loop(char *c, int len, int x, int low, int high, int word) {
    int typed = 0, mistakes = 0, correct = 1;
    int i = 0, j = 0, k = 0, l = 0;
    char dd[1000];
    char m[100];
    memset(dd, 0, sizeof(dd));
    memset(m, 0, sizeof(m));
    time_t start_time, end_time;
    time(&start_time);

    while (c[i] != '\0') {
        time(&end_time);
        move(10, 0);
        clrtoeol();
        attron(COLOR_PAIR(5));
        printw("Time remaining: %.2f s\tWPM: %.2f\tTyped: %d\tMistakes: %d\tCPS: %.2f\n", 30.0 - difftime(end_time, start_time), (60.0 * (((float)typed - mistakes) / (4.0 * x))) / (difftime(end_time, start_time)), typed, mistakes, (((((float)typed - mistakes)) / difftime(end_time, start_time))));
        attroff(COLOR_PAIR(5));
        refresh();
        
        move(11, 1);
        if (difftime(end_time, start_time) >= 30.0) {
            attron(COLOR_PAIR(3));
            printw("\nTime is up!\n");
            attroff(COLOR_PAIR(3));
            break;
        }
        noecho();
        char ch = getch();
        refresh();
        if (ch == 27) {
            break;
        }
        if (ch == c[i] && correct == 1) {
            dd[i] = ch;
            attron(COLOR_PAIR(1));
            clrtoeol();
            printw("%s", dd);
            refresh();
            attroff(COLOR_PAIR(1));
            i++;
            typed++;
            correct = 1;
            j = 0;
            strcpy(m, "");
        } 
        else if ((ch == KEY_BACKSPACE || ch == 127) && correct == 0) {
            if (j > 0) {
                attron(COLOR_PAIR(1));
                clrtoeol();
                m[j - 1] = '\0';
                j--;
                printw("%s", dd);
                refresh();
                attroff(COLOR_PAIR(1));
                attron(COLOR_PAIR(2));
                printw("%s", m);
                refresh();
                attroff(COLOR_PAIR(2));
                if (j == 0) {
                    correct = 1;
                }
            }
        } 
        else if ((ch == KEY_BACKSPACE || ch == 127) && correct == 1) {
            if (i > 0) {
                attron(COLOR_PAIR(1));
                clrtoeol();
                dd[i-1] = '\0';
                printw("%s", dd);
                refresh();
                attroff(COLOR_PAIR(1));
                i--;
                mistakes++;
            }
        } 
        else {
            attron(COLOR_PAIR(1));
            clrtoeol();
            printw("%s", dd);
            attroff(COLOR_PAIR(1));
            refresh();
            attron(COLOR_PAIR(2));
            m[j] = ch;
            printw("%s", m);
            j++;
            mistakes++;
            correct = 0;
            attroff(COLOR_PAIR(2));
        }
        printw("\nType '%c'", c[i]);
        clrtoeol();
    }

    display_results(typed, mistakes, start_time, end_time, x);
}

void generate_text(char *c, char a[800][20], int len, int low, int high, int word) {
    int k = 0, l = 0;
    srand(time(NULL));
    while (l < word) {
        int i = rand() % len;
        if (strlen(a[i]) - 1 >= low && strlen(a[i]) - 1 <= high) {
            for (int j = 0; j < strlen(a[i]) - 1; j++) {
                if (a[i][j] == ' ') {
                    break;
                }
                c[k] = a[i][j];
                k++;
            }
            c[k] = ' ';
            k++;
            l++;
        }
    }
    c[k] = '\0';
}

int display_menu() {
    int choice = 1;
    int ch;
    keypad(stdscr, TRUE); // Enable special keys for menu
    while (1) {
        clear();
        mvprintw(10, 10, "Select Difficulty:");
        if (choice == 1) {
            attron(COLOR_PAIR(3));
            mvprintw(12, 12, "1. Easy");
            attroff(COLOR_PAIR(3));
        } else {
            mvprintw(12, 12, "1. Easy");
        }
        if (choice == 2) {
            attron(COLOR_PAIR(3));
            mvprintw(13, 12, "2. Medium");
            attroff(COLOR_PAIR(3));
        } else {
            mvprintw(13, 12, "2. Medium");
        }
        if (choice == 3) {
            attron(COLOR_PAIR(3));
            mvprintw(14, 12, "3. Hard");
            attroff(COLOR_PAIR(3));
        } else {
            mvprintw(14, 12, "3. Hard");
        }
        mvprintw(16, 10, "Use UP/DOWN arrows to navigate and ENTER to select.");
        refresh();
        ch = getch();
        switch (ch) {
            case KEY_UP:
                if (choice > 1) choice--;
                break;
            case KEY_DOWN:
                if (choice < 3) choice++;
                break;
            case 10: // Enter key
                keypad(stdscr, FALSE); // Disable special keys after menu
                return choice;
        }
    }
}

void display_start_screen() {
    clear();
    mvprintw(LINES / 2, (COLS - strlen("Press ENTER to start the game.")) / 2, "Press ENTER to start the game.");
    refresh();
    while (getch() != 10); // Wait for Enter key
}

int main(int argc, char *argv[]) {
    char buffer[100];
    int i = 0;
    char a[800][20];
    int len = 0;
    char c[1000];
    while (scanf(" %[^\n]%*c", buffer) != EOF) {
        strcpy(a[i], buffer);
        i++;
        len++;
    }
    char rep = 'r';
    while (rep == 'r') {
        initscr();
        cbreak();
        noecho(); // Disable echoing of typed characters
        refresh();
        freopen("/dev/tty", "r", stdin);

        initialize_colors();

        int x;
        if (argc < 2 || (argv[1][0] != '1' && argv[1][0] != '2' && argv[1][0] != '3')) {
            x = display_menu();
            clear();
        } else {
            x = atoi(argv[1]);
            clear();
        }

        int low = 0, high = 0, word = 0;
        if (x == 1) {
            low = 1;
            high = 6;
            word = 100;
        } else if (x == 2) {
            low = 6;
            high = 9;
            word = 75;
        } else {
            low = 8;
            high = 40;
            word = 50;
        }

        display_start_screen(); // Show the start screen
        clear(); // Clear the screen before generating text

        generate_text(c, a, len, low, high, word);
        attron(COLOR_PAIR(3)); // Change to yellow
        printw("%s\n\n", c);
        attroff(COLOR_PAIR(3)); // Change to yellow
        attron(COLOR_PAIR(4));
        if (x == 1) {
            printw("Easy\n");
        } else if (x == 2) {
            printw("Medium\n");
        } else {
            printw("Hard\n");
        }
        attroff(COLOR_PAIR(4));

        game_loop(c, len, x, low, high, word);

        printw("Enter ESC or 'q' to quit and 'r' to play again.\n");
        rep = getch();
        while (1) {
            if (rep == 'r' || rep == 'q' || rep == 27) {
                break;
            } else {
                printw("Invalid input!\n");
                rep = getch();
            }
        }
        clear();
    }
    endwin();
    return 0;
}