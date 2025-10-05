#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define min 3
#define max 10

char board[max][max];
int n,i,j,k;
int winRow;
int lastRow=-1,lastCol=-1;
int winCoords[max][2];
int winCount=0;
FILE *logFile;

char playersAll[3] = {'X','O','Z'};
int numPlayers = 2;
int isHuman[3] = {1,1,0};
int forcedMode = 0;
int totalComputers = 0;
int modeType = 0; // 1 = Player vs Player, 2 = Human vs Computer, 3 = 3-player

// This function repeatedly asks the user for an integer input between minv and maxv,allows 0 if allow0 is true
int read_int_allow0(const char *prompt,int minv,int maxv,int allow0){
    char buf[128];
    int val;
    while(1){
        printf("%s",prompt);
        if(!fgets(buf,sizeof(buf),stdin)){ clearerr(stdin); continue; }
        if(sscanf(buf,"%d",&val)!=1){ printf("Invalid input. Try again.\n"); continue; }
        if(allow0 && val==0) return 0;
        if(val < minv || val > maxv){ printf("Please enter a number between %d and %d.\n",minv,maxv); continue; }
        return val;
    }
}

// This function repeatedly asks the user to enter a character 'y' or 'n' and returns the first valid input for yes or no question
char read_char_yesno(const char *prompt){
    char buf[128];
    while(1){
        printf("%s",prompt);
        if(!fgets(buf,sizeof(buf),stdin)){ clearerr(stdin); continue; }
	for(i=0; buf[i]!='\0'; i++){
            if(buf[i]=='y'||buf[i]=='Y'||buf[i]=='n'||buf[i]=='N') return buf[i];
        }
        printf("Please enter 'y' or 'n'.\n");
    }
}

// Drawing the grid lines for the game
void displayBoard(){
    system("clear");//clears the terminal and shows the current game frame
    printf("\nTic Tac Toe (%dx%d) - %d in a row to win 🎯\n",n,n,winRow);
    printf("   ");
    for(j=0;j<n;j++) printf("   %d  ",j+1);//top border numbers
    printf("\n   ");
    for(j=0;j<n;j++) printf("------");//top horizontal border
    printf("-\n");
    for(i=0;i<n;i++){
        if(i+1<10) printf(" %d |",i+1);//side border numbers
        else printf("%d |",i+1);
        for(j=0;j<n;j++){
            int isWin=0;
            for(k=0;k<winCount;k++) if(winCoords[k][0]==i && winCoords[k][1]==j) isWin=1;
            if(isWin) printf(" *%c* |",board[i][j]);//shows the winning row in *%c*
            else if(i==lastRow && j==lastCol) printf(" (%c) |",board[i][j]);//shows the last move on the board as (%c)
            else printf("  %c  |",board[i][j]);
        }
        printf("\n   ");
        for(j=0;j<n;j++) printf("------");//horizontal borders
        printf("-\n");
    }
    if(lastRow!=-1) printf("Last move by %c marked with ().\n",board[lastRow][lastCol]);
    if(winCount>0) printf("Winning line marked with *.\n");
    printf("\n");
}

// Writes the game to log file
void logBoard(){
    if(!logFile) return;
    fprintf(logFile,"   ");
    for(j=0;j<n;j++) fprintf(logFile,"   %d  ",j+1);
    fprintf(logFile,"\n   ");
    for(j=0;j<n;j++) fprintf(logFile,"------");
    fprintf(logFile,"-\n");
    for(i=0;i<n;i++){
        if(i+1<10) fprintf(logFile," %d |",i+1);
        else fprintf(logFile,"%d |",i+1);
        for(j=0;j<n;j++) fprintf(logFile,"  %c  |",board[i][j]);
        fprintf(logFile,"\n   ");
        for(j=0;j<n;j++) fprintf(logFile,"------");
        fprintf(logFile,"-\n");
    }
    fprintf(logFile,"\n");
    fflush(logFile);
}

// Checks if the board is full,if it is, it is a draw
int checkDraw(){
    for(i=0;i<n;i++) for(j=0;j<n;j++) if(board[i][j]==' ') return 0;
    return 1;
}

// Validate move,if the player puts the same move 
int checkValidMove(int row,int col){
    return (row>=0 && row<n && col>=0 && col<n && board[row][col]==' ');
}

// human players move
void playerMove(char player){
    int row,col;
    char buf[128];
    while(1){
        printf("🧍 Player %c, enter row and column (1-%d): ",player,n);
        if(!fgets(buf,sizeof(buf),stdin)){ clearerr(stdin); continue; }
        if(sscanf(buf,"%d %d",&row,&col)!=2){ printf("Invalid input. Try again.\n"); continue; }
        row--; col--;
        if(checkValidMove(row,col)){ board[row][col]=player; lastRow=row; lastCol=col; break; }
        else printf("That spot is taken or out of range. Try again.\n");
    }
}

// Computer moves with delay to show its thinking
void computerMove(char player){
    int think = (totalComputers>=2) ? 2 : 1;
    printf("💻 Computer (%c) is thinking...\n",player);
    sleep(think);
    int row,col;
    do{ row = rand() % n; col = rand() % n; } while(!checkValidMove(row,col));
    board[row][col] = player;
    lastRow = row; lastCol = col;
    printf("💻 Computer (%c) placed at (%d,%d)\n",player,row+1,col+1);
}

// Check for winner and mark winning line
int checkWinner(){
    winCount = 0;
    // Rows
    for(i=0;i<n;i++) for(j=0;j<=n-winRow;j++){
        char first = board[i][j];
        if(first==' ') continue;
        int win=1;
        for(k=1;k<winRow;k++) if(board[i][j+k]!=first){ win=0; break; }
        if(win){ for(k=0;k<winRow;k++){ winCoords[k][0]=i; winCoords[k][1]=j+k; } winCount=winRow; return first; }
    }
    // Columns
    for(j=0;j<n;j++) for(i=0;i<=n-winRow;i++){
        char first = board[i][j];
        if(first==' ') continue;
        int win=1;
        for(k=1;k<winRow;k++) if(board[i+k][j]!=first){ win=0; break; }
        if(win){ for(k=0;k<winRow;k++){ winCoords[k][0]=i+k; winCoords[k][1]=j; } winCount=winRow; return first; }
    }
    // Diagonal
    for(i=0;i<=n-winRow;i++) for(j=0;j<=n-winRow;j++){
        char first = board[i][j];
        if(first==' ') continue;
        int win=1;
        for(k=1;k<winRow;k++) if(board[i+k][j+k]!=first){ win=0; break; }
        if(win){ for(k=0;k<winRow;k++){ winCoords[k][0]=i+k; winCoords[k][1]=j+k; } winCount=winRow; return first; }
    }
    // Anti-diagonal
    for(i=0;i<=n-winRow;i++) for(j=winRow-1;j<n;j++){
        char first = board[i][j];
        if(first==' ') continue;
        int win=1;
        for(k=1;k<winRow;k++) if(board[i+k][j-k]!=first){ win=0; break; }
        if(win){ for(k=0;k<winRow;k++){ winCoords[k][0]=i+k; winCoords[k][1]=j-k; } winCount=winRow; return first; }
    }
    return 0;
}

// Reset board and status for next game
void setupGame(){
    for(i=0;i<max;i++) for(j=0;j<max;j++) board[i][j] = ' ';
    lastRow = -1; lastCol = -1;
    winCount = 0;
    for(i=0;i<max;i++){ winCoords[i][0] = -1; winCoords[i][1] = -1; }
    if(n<=4) winRow=3; else if(n<=7) winRow=4; else winRow=5;
}

// Configure players according to mode
void configurePlayers(){
    int choice;
    for(i=0;i<3;i++) isHuman[i]=0;
    totalComputers=0;

    if(forcedMode==2){
	    numPlayers=2;
	    if(modeType==1){
		    isHuman[0]=1; isHuman[1]=1;
		    totalComputers=0;
	    }
	    else if(modeType==2){
		    isHuman[0]=1; isHuman[1]=0;
		    totalComputers=1;
	    }
	    return;
    }

    // 3-player mode
    while(1){
        printf("How many human players? (1,2,3) (0 to return): ");
        if(scanf("%d",&choice)!=1){ while(getchar()!='\n'); n=0; return; }
        while(getchar()!='\n');
        if(choice==0){ n=0; return; }
        if(choice==1){ isHuman[0]=1; isHuman[1]=0; isHuman[2]=0; printf("🧍 X is human. 💻 O & Z are computers.\n"); break; }
        else if(choice==2){ isHuman[0]=1; isHuman[1]=1; isHuman[2]=0; printf("🧍 X & O are human. 💻 Z is computer.\n"); break; }
        else if(choice==3){ isHuman[0]=1; isHuman[1]=1; isHuman[2]=1; printf("🧍 X, O & Z are human.\n"); break; }
        else printf("Invalid input. Try again.\n");
    }
    totalComputers=0;
    for(i=0;i<numPlayers;i++) if(!isHuman[i]) totalComputers++;
}

// Open log file of the game
void openLog(){
    logFile = fopen("tic_tac_toe.txt","w");
    if(!logFile){ printf("Error opening log file.\n"); return; }
    fprintf(logFile,"=== 🎮 Tic Tac Toe Match Start ===\n");
    fprintf(logFile,"Board Size: %dx%d\nWin Condition: %d in a row\n",n,n,winRow);
    fprintf(logFile,"Players:\n");
    for(i=0;i<numPlayers;i++) fprintf(logFile,"%s Player %c - %s\n", isHuman[i]?"🧍":"💻", playersAll[i], isHuman[i]?"Human":"Computer");
    fprintf(logFile,"\n--- 🕹️  Move Log ---\n");
    fflush(logFile);
}

// Close log file with result of ended game
void closeLog(char winner){
    if(!logFile) return;
    if(winner) fprintf(logFile,"🏆 Player %c wins with %d in a row!\n",winner,winRow);
    else fprintf(logFile,"🤝 It's a draw!\n");
    fprintf(logFile,"=== End of Match ===\n");
    fclose(logFile);
}

// Main match loop with full reset for replay
void playGame(){
    while(1){
        setupGame();       
        configurePlayers(); 
        if(n==0) return;

        openLog();         
        int winner=0, idx=0;
        char current = playersAll[idx];

        while(!winner && !checkDraw()){
            displayBoard();
            printf("Turn: %s Player %c\n", isHuman[idx]?"🧍":"💻", current);
            if(isHuman[idx]) playerMove(current);
            else computerMove(current);

            if(logFile) fprintf(logFile,"Player %c placed at (%d,%d)\n", current, lastRow+1, lastCol+1);
            logBoard();

            winner = checkWinner();
            if(!winner){ idx = (idx+1) % numPlayers; current = playersAll[idx]; }
        }

        displayBoard();
        printf("\n🎮 Game Over! 🎮\n");
        if(winner) printf("🏆 Player %c wins with %d in a row! 🏆\n", winner, winRow);
        else printf("🤝 It's a draw!\n");
        closeLog(winner);

        // Play again with full reset of the game
        char again = read_char_yesno("\nPlay again? (y/n): ");
        if(again=='y' || again=='Y'){
            lastRow = -1; lastCol = -1;
            winCount = 0;
            for(i=0;i<max;i++){ winCoords[i][0]=-1; winCoords[i][1]=-1; }
            for(i=0;i<max;i++) for(j=0;j<max;j++) board[i][j]=' ';
            totalComputers=0;
            continue;
        }
        else break;
    }
}

// Main menu
int main(){
    srand(time(NULL));
    while(1){
        int choice = read_int_allow0("\n--- 🎮 Tic Tac Toe ---\n"
                                     "(1) 👥 Player vs Player\n"
                                     "(2) 🧍 Human vs 💻 Computer\n"
                                     "(3) 👨‍👩‍👦 Three Player Game\n"
                                     "(4) ❌ Exit\n"
                                     "👉 Enter your choice (1–4): ",1,4,0);
        if(choice==4){ 
            printf("👋 Thanks for playing Tic Tac Toe!\n"); 
            break; 
        }
        if(choice==1){ 
            forcedMode = 2;
	    modeType=1;//player vs player
            int val = read_int_allow0("Enter board size (3–10) or 0 to return: ",min,max,1);
            if(val==0) continue;
            n = val;
        } 
        else if(choice==2){ 
            forcedMode = 2;
	    modeType=2;//player vs computer
            int val = read_int_allow0("Enter board size (3–10) or 0 to return: ",min,max,1);
            if(val==0) continue;
            n = val;
        } 
        else if(choice==3){ 
            forcedMode = 3;
	    modeType=3;//3 players
            int val = read_int_allow0("Enter board size (5–10) or 0 to return: ",5,max,1);
            if(val==0) continue;
            n = val;
        } 
        else continue;

        playGame();
    }
    return 0;
}

