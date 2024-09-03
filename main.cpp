#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "board.h"
// cd ~/Desktop/C++ChessEngine
// g++ -std=c++11 -o chessengine.out main.cpp board.cpp
// ./chessengine.out
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

using namespace std;

int main() {
    cout << "Enter FEN Notation / Empty For Default Position: \n";
    string fen;
    getline(cin, fen); // input from terminal

    Board board;

    if(fen.empty()) {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        board.setupInitialPosition();
    } else {
        board.setupPosition(fen);
    }

    board.printSingleBitboards();     
    cout << "FEN Board: \n";
    board.printFENBoard();

    

    return 0;
}



