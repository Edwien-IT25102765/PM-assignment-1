#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY ' '

// Function prototypes
char **createBoard(int n);
void freeBoard(char **board, int n);
void displayBoard(char **board, int n);
int isValidMove(char **board, int n, int row, int col);
int checkWin(char **board, int n, char symbol);
int checkDraw(char **board, int n);
void logMove(FILE *log, int player, int row, int col);

int main() {
    int n;
    printf("Enter board size (3-10): ");
    scanf("%d", &n);

    if (n < 3 || n > 10) {
        printf("Invalid size. Exiting.\n");
        return 1;
    }

    // Open log file
    FILE *log = fopen("tic_tac_toe_log.txt", "w");
    if (!log) {
        printf("Error opening log file.\n");
        return 1;
    }

    char **board = createBoard(n);
    int player = 1;
    char symbols[2] = {'X', 'O'};
    int row, col;

    while (1) {
        displayBoard(board, n);
        printf("Player %d (%c), enter your move (row col): ", player, symbols[player-1]);
        scanf("%d %d", &row, &col);

        if (!isValidMove(board, n, row, col)) {
            printf("Invalid move. Try again.\n");
            continue;
        }

        board[row][col] = symbols[player-1];
        logMove(log, player, row, col);

        if (checkWin(board, n, symbols[player-1])) {
            displayBoard(board, n);
            printf("Player %d wins!\n", player);
            break;
        }

        if (checkDraw(board, n)) {
            displayBoard(board, n);
            printf("It's a draw!\n");
            break;
        }

        player = (player == 1) ? 2 : 1; // switch turns
    }

    freeBoard(board, n);
    fclose(log);
    return 0;
}

// Function to create board with dynamic memory
char **createBoard(int n) {
    char **board = (char **)malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) {
        board[i] = (char *)malloc(n * sizeof(char));
        for (int j = 0; j < n; j++) {
            board[i][j] = EMPTY;
        }
    }
    return board;
}

// Free memory
void freeBoard(char **board, int n) {
    for (int i = 0; i < n; i++) free(board[i]);
    free(board);
}

// Display board
void displayBoard(char **board, int n) {
    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf(" %c ", board[i][j]);
            if (j < n - 1) printf("|");
        }
        printf("\n");
        if (i < n - 1) {
            for (int j = 0; j < n; j++) {
                printf("---");
                if (j < n - 1) printf("+");
            }
            printf("\n");
        }
    }
    printf("\n");
}

// Validate move
int isValidMove(char **board, int n, int row, int col) {
    if (row < 0 || row >= n || col < 0 || col >= n) return 0;
    if (board[row][col] != EMPTY) return 0;
    return 1;
}

// Check win condition
int checkWin(char **board, int n, char symbol) {
    // Check rows
    for (int i = 0; i < n; i++) {
        int win = 1;
        for (int j = 0; j < n; j++) {
            if (board[i][j] != symbol) {
                win = 0;
                break;
            }
        }
        if (win) return 1;
    }

    // Check cols
    for (int j = 0; j < n; j++) {
        int win = 1;
        for (int i = 0; i < n; i++) {
            if (board[i][j] != symbol) {
                win = 0;
                break;
            }
        }
        if (win) return 1;
    }

    // Check main diagonal
    int win = 1;
    for (int i = 0; i < n; i++) {
        if (board[i][i] != symbol) {
            win = 0;
            break;
        }
    }
    if (win) return 1;

    // Check anti-diagonal
    win = 1;
    for (int i = 0; i < n; i++) {
        if (board[i][n - i - 1] != symbol) {
            win = 0;
            break;
        }
    }
    if (win) return 1;

    return 0;
}

// Check for draw
int checkDraw(char **board, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (board[i][j] == EMPTY) return 0;
    return 1;
}

// Log moves to file
void logMove(FILE *log, int player, int row, int col) {
    fprintf(log, "Player %d moved to (%d, %d)\n", player, row, col);
}


