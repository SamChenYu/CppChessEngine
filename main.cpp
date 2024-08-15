#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>


// cd ~/Desktop/C++ChessEngine
// g++ -o main main.cpp
// ./main

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
    cout << "Enter FEN Notation: \n";
    string fen;
    cin >> fen;


    // Parse FEN Notation
    vector<string> board = split(fen, '/');

    if(board.size() != 8) {
        throw invalid_argument("Invalid FEN Notation");
    }
    
    for(int i = 0; i < 8; i++) {
        cout << board[i] << endl;
    }
    

    return 0;
}
