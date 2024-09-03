#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "board.h"


// cd ~/Desktop/C++ChessEngine
// g++ -std=c++11 -o chessengine main.cpp board.cpp
// ./chessengine

// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

using namespace std;


// Function to split a string by a delimiter
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


int main() {
    cout << "Enter FEN Notation / Empty For Default Position: \n";
    string fen;
    getline(cin, fen); // input from terminal

    Board board;

    if(fen.empty()) {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        board.setupInitialPosition();
        board.printSingleBitboards();     
        board.printFENBoard();
    } else {
        vector<string> splitString = split(fen, '/');
        if(splitString.size() != 8) {
            throw invalid_argument("Invalid FEN Notation");
        }
        // Import into bitboards    
        for(int i = 0; i < 8; i++) {
            cout << splitString[i] << endl;
        }
    }


    return 0;
}



