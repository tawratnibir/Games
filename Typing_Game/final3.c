#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void initialize_colors()
{
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_RED);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(8, COLOR_WHITE, COLOR_WHITE);
}

void display_results(int typed, int mistakes, time_t start_time, time_t end_time, int x)
{
    attron(COLOR_PAIR(4));
    float timeTaken = difftime(end_time, start_time);
    printw("\nTyped: %d\nMistakes: %d\n", typed, mistakes);
    int total = typed + mistakes;
    printw("Total: %d\n", total);
    printw("Time taken: %.2f s\n", timeTaken);
    float wpm = (60.0 * (((float)typed - mistakes) / (4.0 * x))) / timeTaken;
    if (wpm < 0)
        printw("WPM: 0\n");
    else
        printw("WPM: %.2f\n", wpm);
    float cps = ((((float)typed - mistakes)) / timeTaken);
    if (cps < 0)
        printw("CPS: 0\n");
    else
        printw("CPS: %.2f\n", cps);
    float accuracy = 100.0 - ((float)mistakes / typed) * 100;
    if (accuracy < 0)
        printw("Accuracy: 0.00%%\n");
    else
        printw("Accuracy: %.2f%%\n", accuracy);
    attroff(COLOR_PAIR(4));
    refresh();
}
void drop_random_characters()
{
    srand(time(NULL));
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x); // Get screen size

    int drops = 5; // Number of random falling characters

    for (int i = 0; i < drops; i++)
    {
        int length = (COLS - strlen("Press ENTER to start the game.")) / 2;
        int length2 = length + strlen("Press ENTER to start the game.");
        int start_x = rand() % length;                                                         // Random column
        int start_x2 = length2 + rand() % (COLS - length2 + 1);                                // Random column
        int delay = 5;                                                                         // Random speed (ms per move)
        char rand_char = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"[rand() % 52];  // Random character
        char rand_char2 = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"[rand() % 52]; // Random character

        // Drop the character from top to bottom
        for (int y = 1; y < max_y - 1; y++)
        {
            mvprintw(y, start_x, "%c", rand_char);
            mvprintw(y, start_x2, "%c", rand_char2);
            refresh();
            napms(delay);               // Sleep for 'delay' milliseconds
            mvprintw(y, start_x, " ");  // Erase previous character
            mvprintw(y, start_x2, " "); // Erase previous character
        }
    }
}

void game_loop(char *c, int len, int x, int low, int high, int word)
{
    int typed = 0, mistakes = 0, correct = 1;
    int i = 0, j = 0, k = 0, l = 0;
    char dd[1000];
    char m[100];
    memset(dd, 0, sizeof(dd));
    memset(m, 0, sizeof(m));
    time_t start_time, end_time;
    time(&start_time);

    while (c[i] != '\0')
    {
        time(&end_time);
        move(10, 0);
        clrtoeol();
        attron(COLOR_PAIR(5));
        float wpm = (60.0 * (((float)typed - mistakes) / (4.0 * x))) / difftime(end_time, start_time);
        float cps = ((((float)typed - mistakes)) / difftime(end_time, start_time));
        if (wpm < 0)
            wpm = 0;
        if (cps < 0)
            cps = 0;
        int total;
        total = typed + mistakes;
        timeout(100);
        printw("%.2f s remaining\tWPM: %.2f\tTyped: %d\tMistakes: %d\tTotal: %d\tCPS: %.2f\n", 30.0 - difftime(end_time, start_time), wpm, typed, mistakes, total, cps);
        attroff(COLOR_PAIR(5));
        refresh();

        move(12, 1);
        if (difftime(end_time, start_time) >= 30.0)
        {
            attron(COLOR_PAIR(3));
            printw("\nTime is up!\n");
            attroff(COLOR_PAIR(3));
            timeout(-1);
            break;
        }
        noecho();
        char ch = getch();
        refresh();
        if (ch == ERR)
        {
            continue;
        }
        if (ch == 27)
        {
            timeout(-1);
            break;
        }
        if (ch == c[i] && correct == 1)
        {
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
        else if ((ch == KEY_BACKSPACE || ch == 127) && correct == 0)
        {
            if (j > 0)
            {
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
                if (j == 0)
                {
                    correct = 1;
                }
            }
        }
        else if ((ch == KEY_BACKSPACE || ch == 127) && correct == 1)
        {
            if (i > 0)
            {
                attron(COLOR_PAIR(1));
                clrtoeol();
                dd[i - 1] = '\0';
                printw("%s", dd);
                refresh();
                attroff(COLOR_PAIR(1));
                i--;
                mistakes++;
            }
        }
        else
        {
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
        attron(COLOR_PAIR(7));
        mvprintw(15, 10, " /\\_/\\");
        mvprintw(16, 10, "( o.o )");
        mvprintw(17, 10, " > ^ <");
        attroff(COLOR_PAIR(7));
        clrtoeol();
    }

    display_results(typed, mistakes, start_time, end_time, x);
}

void generate_text(char *c, char a[800][20], int len, int low, int high, int word)
{
    int k = 0, l = 0;
    srand(time(NULL));
    while (l < word)
    {
        int i = rand() % len;
        if (strlen(a[i]) - 1 >= low && strlen(a[i]) - 1 <= high)
        {
            for (int j = 0; j < strlen(a[i]) - 1; j++)
            {
                if (a[i][j] == ' ')
                {
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

int display_menu()
{
    int choice = 1;
    int ch;
    keypad(stdscr, TRUE); // Enable special keys for menu
    while (1)
    {
        clear();
        attron(COLOR_PAIR(7) | A_BLINK);
        mvprintw(7, (COLS - strlen("Typing Game")) / 2, "TYPING GAME");
        attroff(COLOR_PAIR(7) | A_BLINK);
        mvprintw(10, (COLS - strlen("Select Difficulty")) / 2, "Select Difficulty");
        if (choice == 1)
        {
            attron(COLOR_PAIR(6) | A_BOLD);
            mvprintw(12, (COLS - strlen("Easy")) / 2, "Easy");
            attroff(COLOR_PAIR(6) | A_BOLD);
        }
        else
        {
            mvprintw(12, (COLS - strlen("Easy")) / 2, "Easy");
        }
        if (choice == 2)
        {
            attron(COLOR_PAIR(6) | A_BOLD);
            mvprintw(13, (COLS - strlen("Medium")) / 2, "Medium");
            attroff(COLOR_PAIR(6) | A_BOLD);
        }
        else
        {
            mvprintw(13, (COLS - strlen("Medium")) / 2, "Medium");
        }
        if (choice == 3)
        {
            attron(COLOR_PAIR(6) | A_BOLD);
            mvprintw(14, (COLS - strlen("Hard")) / 2, "Hard");
            attroff(COLOR_PAIR(6) | A_BOLD);
        }
        else
        {
            mvprintw(14, (COLS - strlen("Hard")) / 2, "Hard");
        }
        mvprintw(16, (COLS - strlen("Use UP/DOWN arrows to navigate and ENTER to select.")) / 2, "Use UP/DOWN arrows to navigate and ENTER to select.");
        refresh();
        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            if (choice > 1)
                choice--;
            break;
        case KEY_DOWN:
            if (choice < 3)
                choice++;
            break;
        case 10:                   // Enter key
            keypad(stdscr, FALSE); // Disable special keys after menu
            return choice;
        }
    }
}

void loading_screens()
{
    mvprintw(LINES - 5, (COLS - strlen("LOADING")) / 2, "LOADING");
    attron(COLOR_PAIR(8));
    mvprintw(LINES - 4, (COLS - strlen("          ")) / 2, "          ");
    attroff(COLOR_PAIR(8));
    for (int i = 0; i < 10; i++)
    {
        attron(COLOR_PAIR(1));
        mvprintw(LINES - 3, (COLS - strlen("          ")) / 2 + i, "@");
        attron(COLOR_PAIR(1));
        refresh();
        usleep(100000);
    }
}
void display_start_screen()
{
    clear();
    refresh();
    timeout(100);
    while (getch() != 10)
    {
        attron(COLOR_PAIR(2) | A_BLINK);
        mvprintw(LINES / 2, (COLS - strlen("Press ENTER to start the game.")) / 2, "Press ENTER to start the game.");
        attroff(COLOR_PAIR(2) | A_BLINK);
        drop_random_characters();
    } // Wait for Enter key
    // loading_screens();
    timeout(-1);
}

int main(int argc, char *argv[])
{
    char buffer[100];
    int i = 0;
    char a[800][20];
    int len = 0;
    char c[1000];
    while (scanf(" %[^\n]%*c", buffer) != EOF)
    {
        strcpy(a[i], buffer);
        i++;
        len++;
    }
    char rep = 'r';
    while (rep == 'r')
    {
        initscr();
        curs_set(FALSE);
        cbreak();
        noecho(); // Disable echoing of typed characters
        refresh();
        freopen("/dev/tty", "r", stdin);

        initialize_colors();

        int x;
        if (argc < 2 || (argv[1][0] != '1' && argv[1][0] != '2' && argv[1][0] != '3'))
        {
            x = display_menu();
            clear();
        }
        else
        {
            x = atoi(argv[1]);
            clear();
        }

        int low = 0, high = 0, word = 0;
        if (x == 1)
        {
            low = 1;
            high = 6;
            word = 100;
        }
        else if (x == 2)
        {
            low = 5;
            high = 9;
            word = 75;
        }
        else
        {
            low = 8;
            high = 40;
            word = 50;
        }

        display_start_screen(); // Show the start screen
        clear();                // Clear the screen before generating text
        loading_screens();
        sleep(1);
        clear();

        generate_text(c, a, len, low, high, word);
        attron(COLOR_PAIR(3)); // Change to yellow
        printw("%s\n\n", c);
        attroff(COLOR_PAIR(3)); // Change to yellow
        attron(COLOR_PAIR(4));
        if (x == 1)
        {
            printw("Easy\n");
        }
        else if (x == 2)
        {
            printw("Medium\n");
        }
        else
        {
            printw("Hard\n");
        }
        attroff(COLOR_PAIR(4));

        game_loop(c, len, x, low, high, word);

        printw("Enter ESC or 'q' to quit and 'r' to play again.\n");
        rep = getch();
        while (1)
        {
            if (rep == 'r' || rep == 'q' || rep == 27)
            {
                break;
            }
            else
            {
                printw("Invalid input!\n");
                rep = getch();
            }
        }
        clear();
    }
    endwin();
    return 0;
}
