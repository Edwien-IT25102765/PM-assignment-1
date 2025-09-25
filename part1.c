#include <stdio.h>
#include <stdlib.h>

#define MIN_N 3
#define MAX_N 10

/* Global variables */
char board[MAX_N][MAX_N];
int N;
int WIN_COUNT;
int lastRow = -1, lastCol = -1;
FILE *logFile;

/* --- FUNCTION DEFINITIONS --- */

// Display board
void displayBoard() {
    system(
#if defined(_WIN32) || defined(_WIN64)
        "cls"
#else
        "clear"
#endif
    );

    printf("\nTic Tac Toe (%d x %d) - %d in a row to win\n\n", N, N, WIN_COUNT);

    // Calculate spacing for column numbers
    printf("    ");
    for (int j = 0; j < N; j++) {
        if (j + 1 < 10)
            printf("  %d  ", j + 1); // single digit
        else
            printf(" %d  ", j + 1);  // double digit
    }
    printf("\n");

    // Top border
    printf("   ");
    for (int j = 0; j < N; j++) printf("------");
    printf("\n");

    for (int i = 0; i < N; i++) {
        // Row number + left border
        if (i + 1 < 10)
            printf(" %d |", i + 1);
        else
            printf("%d |", i + 1);

        for (int j = 0; j < N; j++) {
            if (i == lastRow && j == lastCol)
                printf(" *%c* |", board[i][j]);
            else
                printf("  %c  |", board[i][j]);
        }
        printf("\n");

        // Horizontal divider
        printf("   ");
        for (int j = 0; j < N; j++) printf("------");
        printf("\n");
    }

    printf("\n");
}

// Log board to file
void logBoard(char player) {
    fprintf(logFile, "Player %c made a move:\n", player);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) fprintf(logFile, "%c ", board[i][j]);
        fprintf(logFile, "\n");
    }
    fprintf(logFile, "\n");
}

// Check if draw
int isDraw() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == ' ') return 0;
    return 1;
}

// Validate move
int isValidMove(int row, int col) {
    return (row >= 0 && row < N && col >= 0 && col < N && board[row][col] == ' ');
}

// Player input
void playerMove(char player) {
    int row, col;
    while (1) {
        printf("Player %c, enter move (row col, 1-%d): ", player, N);
        if (scanf("%d %d", &row, &col) != 2) {
            while (getchar() != '\n'); // flush buffer
            printf("Invalid input. Try again.\n");
            continue;
        }
        row--; col--;
        if (isValidMove(row, col)) {
            board[row][col] = player;
            lastRow = row;
            lastCol = col;
            break;
        } else {
            printf("Invalid move. Try again.\n");
        }
    }
}

// Check winner
int checkWinner() {
    // Rows
    for (int i = 0; i < N; i++)
        for (int j = 0; j <= N - WIN_COUNT; j++) {
            char first = board[i][j];
            if (first == ' ') continue;
            int win = 1;
            for (int k = 1; k < WIN_COUNT; k++)
                if (board[i][j+k] != first) { win = 0; break; }
            if (win) return first;
        }

    // Columns
    for (int j = 0; j < N; j++)
        for (int i = 0; i <= N - WIN_COUNT; i++) {
            char first = board[i][j];
            if (first == ' ') continue;
            int win = 1;
            for (int k = 1; k < WIN_COUNT; k++)
                if (board[i+k][j] != first) { win = 0; break; }
            if (win) return first;
        }

    // Diagonals
    for (int i = 0; i <= N - WIN_COUNT; i++)
        for (int j = 0; j <= N - WIN_COUNT; j++) {
            char first = board[i][j];
            if (first == ' ') continue;
            int win = 1;
            for (int k = 1; k < WIN_COUNT; k++)
                if (board[i+k][j+k] != first) { win = 0; break; }
            if (win) return first;
        }

    // Anti-diagonals
    for (int i = 0; i <= N - WIN_COUNT; i++)
        for (int j = WIN_COUNT - 1; j < N; j++) {
            char first = board[i][j];
            if (first == ' ') continue;
            int win = 1;
            for (int k = 1; k < WIN_COUNT; k++)
                if (board[i+k][j-k] != first) { win = 0; break; }
            if (win) return first;
        }

    return 0;
}

// Setup game
void setupGame() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            board[i][j] = ' ';

    logFile = fopen("game_log.txt", "w");
    if (!logFile) {
        printf("Error: Cannot open log file.\n");
        exit(1);
    }

    if (N <= 4) WIN_COUNT = 3;
    else if (N <= 7) WIN_COUNT = 4;
    else WIN_COUNT = 5;
}

// Close game
void closeGame() {
    fclose(logFile);
}

// Main control
void playGame() {
    char current = 'X';
    int winner = 0;

    while (!winner && !isDraw()) {
        displayBoard();
        playerMove(current);
        logBoard(current);

        winner = checkWinner();
        if (!winner) current = (current == 'X') ? 'O' : 'X';
    }

    displayBoard();
    if (winner)
        printf(">>> Player %c wins! <<<\n", winner);
    else
        printf(">>> It's a draw! <<<\n");
}

// Main function
int main(void) {
    printf("=== Tic Tac Toe (Part 1: Two Players) ===\n");
    printf("Enter board size N (%d–%d): ", MIN_N, MAX_N);
    scanf("%d", &N);

    if (N < MIN_N || N > MAX_N) {
        printf("Invalid board size. Exiting.\n");
        return 1;
    }

    setupGame();
    playGame();
    closeGame();

    return 0;
}

