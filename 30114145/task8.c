#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>





#define SIDES 4
#define SQUARED_SIDES (SIDES * SIDES)
#define PIECES (SIDES * SIDES - 1)





struct termios oldt, newt;

//盤面での現在の数字の並び順
int board[SQUARED_SIDES];

//クリアまでの経過したターン
int elapsedTurns;


//盤面を表示する
//
void Board(void)
{
    for (int i = 0; i < SQUARED_SIDES; i++)
    {
        if ((i % SIDES) == 0)
        {
            if (i != 0)
            {
                putchar('|');
            }
            printf("\n ");

            for (int j = 0; j < SIDES + 1; j++)
            {
                printf("----");

            }
            printf("-\n |");
        }
        else
        {
            printf("|");
        }
        if (board[i] == 0)
        {
            printf("   ");


        }
        else
        {
            //ここで数字の配列w入力する。

            printf(" %02d ", board[i]);

        }
    }

    //次の段へ行く
    printf("|\n ");

    for (int j = 0; j < SIDES + 1; j++)
    {


        printf("----");



    }
    printf("-\n ");
}



//空マスの動きを現在表示されているボードをエスケープシーケンスを用いて編集することによって対応させる。
void rewindCursor(const int lines, const int columns)
{

    printf("\x1b[%dA", lines);
    printf("\x1b[%dD", columns);

}

#define ARROW_UP 'A'

#define ARROW_DOWM 'B'

#define ARROW_RIGHT 'C'

#define ARROW_LEFT 'D'

char getArrowKey(void)
{
    while (getchar() != '\033')
    {
    }

    getchar();

    return getchar();
}

//ボードをランダムに並べる。

void initBoardGame(void)
{

    //すでに決まったマスを保管する
    bool decidedNum[SQUARED_SIDES];

    
    memset(decidedNum, false, sizeof(decidedNum));

    //ランダム関数を初期化
    srand((int)time(NULL));

    for (int i = 0; i < SQUARED_SIDES; i++)
    {

        
        int randomNum = rand() % SQUARED_SIDES;

        if (!decidedNum[randomNum])
        {

            board[i] = randomNum;
            decidedNum[randomNum] = true;
            
        }

        else
        {
            i--;
        }
    }
}

//空白マスの位置（配列内で空白のindex）を探す。
int findEmptyIndex(void)
{
    for (int EmptyIndex = 0; EmptyIndex < SQUARED_SIDES; EmptyIndex++)
    {
        if (!board[EmptyIndex])
        {
            return EmptyIndex;
        }
    }
    return 0;
}

//入力された動きに対してそれぞれ処理を行う。
void getMove(void)
{
    const int EmptyIndex = findEmptyIndex();
    while (true)
    {
        switch (getArrowKey())
        {
        
        //空白マスを下に移動させる。
        case ARROW_DOWM:

            if (EmptyIndex < (SQUARED_SIDES - SIDES))
            {
                //数字の並びを保持している配列において空白マスのIndexとその下のマス(空白マスのIndex＋４)を入れ替える。
                board[EmptyIndex] = board[(EmptyIndex + SIDES)];
                board[(EmptyIndex + SIDES)] = 0;
                return;
            }

            break;

        //空白マスを上に移動させる。
        case ARROW_UP:

            if (EmptyIndex >= SIDES)
            {
                board[EmptyIndex] = board[(EmptyIndex - SIDES)];
                board[(EmptyIndex - SIDES)] = 0;
                return;
            }

            break;

        //空白マスを左に移動させる。
        case ARROW_LEFT:

            if ((EmptyIndex % SIDES) != 0)
            {
                board[EmptyIndex] = board[(EmptyIndex - 1)];
                board[(EmptyIndex - 1)] = 0;
                return;
            }

            break;

        //空白マスを右に移動させる。
        case ARROW_RIGHT:

            if ((EmptyIndex % SIDES) != (SIDES - 1))
            {
                board[EmptyIndex] = board[(EmptyIndex + 1)];
                board[(EmptyIndex + 1)] = 0;
                return;
            }

            break;

        }
    }
}



//終了判定を行う。
//並び順になっていなければTrueで継続
bool continueGame(void)
{
    for (int i = 0; i < (SQUARED_SIDES - 1); i++)
    {
        if (board[i] != (i + 1))
        {
            return true;
        }
    }
    return false;
}




void restoreTerminal(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}




void configureTerminal(void)
{
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ICANON;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    atexit(restoreTerminal);
}

//ファイル出力
//必須項目にファイル出力を使用することとあったので解くまでに動かした回数をファイルに出力する
void fileoutput(int turn){
    FILE *fp;
    char fname[] = "task8.dat";
    fp = fopen(fname, "a");
    fprintf(fp, "%d", turn);
    fclose(fp);
}


int main(void)
{
    elapsedTurns = 0;
    configureTerminal();
    initBoardGame();
    Board();
    while (continueGame())
    {
        elapsedTurns += 1;
        getMove();
        rewindCursor(2 * (SIDES + 1), 0);
        Board();
    }
    fileoutput(elapsedTurns);
    return 0;
}
