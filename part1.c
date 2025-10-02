#include <stdio.h>
#include <stdlib.h>

#define min 3
#define max 10

char board[max][max];
int n,i,j,k;
int winRow;
int lastRow = -1, lastCol = -1;
FILE *logFile;

// Display board
void displayBoard(){
	system("clear");//clears the terminal and only show the current game frame

    printf("\nTic Tac Toe (%dx%d) - %d in a row to win\n",n,n,winRow);

    // Calculate spacing for column numbers
    printf("   ");
    for (j=0;j<n;j++) {
        if (j+1<10)
            printf("   %d  ",j+1); // single digit column numbers
        else
            printf("  %d   ",j+1);  // double digit columns numbers(10)
    }
    printf("\n");

    // Top border of the board
    printf("   ");
    for (j=0;j<n;j++) printf("------");
    printf("-\n");

    for (i=0;i<n;i++){
        // Row number + left border of the board
        if (i+1<10)
            printf(" %d |",i+1);
        else
            printf("%d |",i+1);

        for (j=0;j<n;j++){
            if (i == lastRow && j == lastCol)
                printf(" *%c* |", board[i][j]);//show the last come highlighted like *X*
            else
                printf("  %c  |", board[i][j]);
        }
        printf("\n");

        // Horizontal divider of the board
        printf("   ");
        for (j=0;j<n;j++) printf("------");
        printf("-\n");
    }

    printf("\n");
}

// Logging gameplay to file
void logBoard(char player){
    fprintf(logFile, "Player %c made a move:\n", player);
    for (i=0;i<n;i++){
        for (j=0;j<n;j++) fprintf(logFile, "%c ", board[i][j]);
        fprintf(logFile, "\n");
    }
    fprintf(logFile, "\n");
}

// Check if its draw
int checkDraw(){
    for (i=0;i<n;i++)
        for (j=0;j<n;j++)
            if (board[i][j]==' ') return 0;
    return 1;
}

// Validate move of players
int checkValidMove(int row,int col){
    return (row>=0 && row<n && col>=0 && col<n && board[row][col]==' ');
}

// Player input
void playerMove(char player){
    int row,col;
    while (1) {
	printf("Player %c, where would you like to place your mark? (row col, 1-%d): ", player, n);
	if (scanf("%d %d",&row,&col)!=2){
            while (getchar()!='\n');
	    printf("Oops! That spot is taken or invalid. Please try again.\n");
            continue;
        }
        row--; col--;
        if (checkValidMove(row, col)) {
            board[row][col]=player;
            lastRow=row;
            lastCol=col;
            break;
        } else {
		printf("Oops! That spot is taken or invalid. Please try again.\n");
	}
    }
}
// Checking for winner
int checkWinner(){
    // Row checking
    for (i=0;i<n;i++)
        for (j=0;j<=n-winRow;j++){
            char first=board[i][j];
            if (first==' ')continue;
            int win=1;
            for (k=1;k<winRow;k++)
                if (board[i][j+k]!=first){win=0;break;}
            if (win) return first;
	}

    // Columns checking
    for (j=0;j<n;j++)
        for (i=0;i<=n-winRow;i++){
            char first=board[i][j];
            if (first==' ')continue;
            int win=1;
            for (k=1;k<winRow;k++)
                if (board[i+k][j]!=first){win=0;break;}
            if (win) return first;
        }

    // Diagonals checking
    for (i=0;i<=n-winRow;i++)
        for (j=0;j<=n-winRow;j++){
            char first=board[i][j];
            if (first==' ') continue;
            int win=1;
            for (k=1;k<winRow;k++)
                if (board[i+k][j+k]!=first){win=0;break;}
            if (win) return first;
        }

    // Anti-diagonals checking
    for (i=0;i<=n-winRow;i++)
        for (j=winRow-1;j<n;j++){
            char first=board[i][j];
            if (first==' ') continue;
            int win=1;
            for (k=1;k<winRow;k++)
                if (board[i+k][j-k]!=first){win=0;break;}
            if (win) return first;
        }

    return 0;
}

// Setup game 
void setupGame(){
    for (i=0;i<n;i++)
        for (j=0;j<n;j++)
            board[i][j]=' ';

    logFile = fopen("game_log.txt", "w");//writes the current game logs only
    if (!logFile) {
        printf("Error: Cannot open log file.\n");
        exit(1);
    }
    //this is to decide how many in a row are needed to win based on the board size
    if (n<=4) winRow=3;//for 3 & 4 grid 3 in a row
    else if (n<=7) winRow=4;//for 5 to 7 grid 4 in a row
    else winRow=5;//for 8 to 10 grid 5 in a row
}

// Close game
void closeGame(){
    fclose(logFile);
}

// Main control
void playGame(){
    char current='X';
    int winner=0;

    while (!winner && !checkDraw()){
        displayBoard();
        playerMove(current);
        logBoard(current);

        winner=checkWinner();
        if (!winner) current= (current=='X') ? 'O' : 'X';
    }

    displayBoard();
    printf("\n>>> Game Over! <<<\n");
    if (winner)
        printf("Player %c wins!\n",winner);
    else
        printf("It's a draw!\nWell played both!\n");
    printf("Thanks for playing!\nWant to play again(y/n)?\n");
    
}

// Main function
int main(void){
    printf("--- Tic Tac Toe ---\n");
    printf("Enter board size (%d–%d): ",min,max);
    scanf("%d",&n);

    if (n<min || n>max){
        printf("Invalid board size. Exiting game.\n");
        return 1;
    }

    setupGame();
    playGame();
    closeGame();

    return 0;
}
