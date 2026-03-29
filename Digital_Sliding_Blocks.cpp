#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string>
#include <cmath>
#include <algorithm>

#ifdef _WIN32
    #define CLEAR_SCREEN "cls"
#else
    #define CLEAR_SCREEN "clear"
#endif

const int N = 4;                // 棋盘大小 4x4
const int TARGET[N][N] = {      // 目标状态，0 代表空白格
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12},
    {13, 14, 15, 0}
};

int board[N][N];        // 当前棋盘
int blankRow, blankCol; // 空白格的位置
int steps;              // 已走步数

// 初始化棋盘为目标状态
void setTargetBoard() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            board[i][j] = TARGET[i][j];
            if (board[i][j] == 0) {
                blankRow = i;
                blankCol = j;
            }
        }
    }
}

// 显示棋盘
void displayBoard() {
    std::cout << "\n  当前棋盘 (步数: " << steps << ")\n";
    std::cout << "  ---------------------\n";
    for (int i = 0; i < N; ++i) {
        std::cout << "  |";
        for (int j = 0; j < N; ++j) {
            if (board[i][j] == 0) {
                std::cout << "   .  |";   // 空白格显示为点
            } else {
                // 数字右对齐，宽度为3
                std::cout << std::setw(3) << board[i][j] << "  |";
            }
        }
        std::cout << "\n  ---------------------\n";
    }
    std::cout << "\n";
}

// 尝试移动空白格 (dx, dy 为行、列上的变化量)
// 移动成功返回 true，并更新空白格位置；否则返回 false
bool tryMove(int dx, int dy) {
    int newRow = blankRow + dx;
    int newCol = blankCol + dy;
    if (newRow < 0 || newRow >= N || newCol < 0 || newCol >= N) {
        return false; // 移动非法
    }
    // 交换空白格与相邻数字
    std::swap(board[blankRow][blankCol], board[newRow][newCol]);
    blankRow = newRow;
    blankCol = newCol;
    return true;
}

// 随机打乱棋盘（从目标状态开始执行随机移动，保证有解）
void shuffleBoard(int moveCount = 300) {
    setTargetBoard();                     // 重置为目标状态
    const int dx[] = {-1, 1, 0, 0};       // 上，下，左，右
    const int dy[] = {0, 0, -1, 1};
    for (int i = 0; i < moveCount; ++i) {
        int dir = rand() % 4;
        tryMove(dx[dir], dy[dir]);        // 随机移动，不计步数
    }
}

// 重置游戏（打乱棋盘，步数归零）
void resetGame() {
    shuffleBoard(300 + rand() % 200);    // 随机移动 300~500 次，增加随机性
    steps = 0;
}

// 检查是否胜利
bool isWin() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (board[i][j] != TARGET[i][j]) {
                return false;
            }
        }
    }
    return true;
}

// 用户移动，成功则步数+1
bool userMove(int dx, int dy) {
    if (tryMove(dx, dy)) {
        ++steps;
        return true;
    }
    return false;
}

// 清除屏幕
void clearScreen() {
    system(CLEAR_SCREEN);
}

// 显示操作说明
void showHelp() {
    std::cout << "操作说明:\n";
    std::cout << "  W / w : 向上移动空白格\n";
    std::cout << "  S / s : 向下移动空白格\n";
    std::cout << "  A / a : 向左移动空白格\n";
    std::cout << "  D / d : 向右移动空白格\n";
    std::cout << "  R / r : 重新开始游戏\n";
    std::cout << "  Q / q : 退出游戏\n";
    std::cout << "------------------------\n";
}

int main() {
    srand(static_cast<unsigned>(time(nullptr))); // 随机种子

    resetGame();    // 初始化游戏

    char command;
    bool quit = false;

    while (!quit) {
        clearScreen();
        displayBoard();
        showHelp();

        std::cout << "请输入指令: ";
        std::cin >> command;
        // 忽略输入行中剩余的字符，避免干扰后续输入
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        bool validMove = false;
        switch (tolower(command)) {
            case 'w':
                validMove = userMove(-1, 0); // 上
                break;
            case 's':
                validMove = userMove(1, 0);  // 下
                break;
            case 'a':
                validMove = userMove(0, -1); // 左
                break;
            case 'd':
                validMove = userMove(0, 1);  // 右
                break;
            case 'r':
                resetGame();
                validMove = true;   // 重置后无需判断胜利，直接继续
                break;
            case 'q':
                quit = true;
                validMove = true;
                break;
            default:
                std::cout << "无效指令，请重新输入！\n";
                std::cin.get(); // 暂停一下，让用户看到提示
                break;
        }

        // 移动无效（出界）时给出提示
        if (!validMove && (tolower(command) == 'w' || tolower(command) == 's' ||
                           tolower(command) == 'a' || tolower(command) == 'd')) {
            std::cout << "移动非法，无法向该方向移动！\n";
            std::cin.get();
        }

        // 胜利判定（仅在非重置/退出且有效移动后检查）
        if (!quit && validMove && (tolower(command) == 'w' || tolower(command) == 's' ||
                                   tolower(command) == 'a' || tolower(command) == 'd')) {
            if (isWin()) {
                clearScreen();
                displayBoard();
                std::cout << "\n🎉 恭喜！你赢了！ 🎉\n";
                std::cout << "总步数: " << steps << "\n";
                std::cout << "是否开始新游戏？ (y/n): ";
                char choice;
                std::cin >> choice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (tolower(choice) == 'y') {
                    resetGame();
                } else {
                    quit = true;
                }
            }
        }
    }

    std::cout << "感谢游玩！\n";
    return 0;
}