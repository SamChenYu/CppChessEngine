#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "board.h"
#include "move.h"
// cd ~/Desktop/C++ChessEngine
// g++ -std=c++11 -o chessengine.out main.cpp board.cpp move.cpp
// ./chessengine.out
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

using namespace std;

void unitTest() {
    // Testing every kind of move
    Board board;

    // Normal Move
    board.setupPosition("8/8/8/8/8/8/8/P7 b - - 0 1");
    Move move = Move(56, 48, 0, 0, 0, 0, Move::MoveType::Normal, true, true, true, true); // white pawn on a1 moves to a2
    board.makeMove(move);
    if (board.pieceTypeAtSquare(48) != 0 && board.pieceTypeAtSquare(56) != -1) {
        throw std::invalid_argument("Normal Move Test Failed");
    }

    // Moved Twice
    board.setupPosition("8/8/8/8/8/8/8/P7 b - - 0 1");
    move = Move(56, 40, 0, 0, 0, 0, Move::MoveType::MovedTwice, true, true, true, true); // white pawn on a1 moves to a3
    board.makeMove(move);
    if (board.pieceTypeAtSquare(40) != 0 && board.pieceTypeAtSquare(56) != -1) {
        throw std::invalid_argument("Moved Twice Test Failed");
    }

    // Capture
    // Move(fromSquare, toSquare, pieceType, capturedPiece, enPassantSquare, promotedPieceType, moveType, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling)

    board.setupPosition("7n/8/8/8/8/8/8/7R w - - 0 1");
    move = Move(56, 0, 3, 7, 0, 0, Move::MoveType::Capture, true, true, true, true); // white rook on h1 captures black knight at h8
    if(board.pieceTypeAtSquare(0) != 3 && board.pieceTypeAtSquare(56) != -1) {
        throw std::invalid_argument("Capture Test Failed");
    }

    // En Passant Capture
    board.setupPosition("8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    move = Move(27, 36, 0, 6, 28, 0, Move::MoveType::EnPassantCapture, true, true, true, true); //  white pawn e5 captures black pawn on d5
    board.makeMove(move);
    if(board.pieceTypeAtSquare(36) != 0 && board.pieceTypeAtSquare(28) != -1 && board.pieceTypeAtSquare(27) != -1) {
        throw std::invalid_argument("En Passant Capture Test Failed");
    }

    // Promote
    board.setupPosition("8/8/8/8/8/8/3p4/8 b - - 0 1");
    move = Move(52, 60, 6, 0, 0, 10, Move::MoveType::Promote, true, true, true, true); // black pawn d2 promotes to black queen
    board.makeMove(move);
    if(board.pieceTypeAtSquare(60) != 10 && board.pieceTypeAtSquare(52) != -1) {
        throw std::invalid_argument("Promote Test Failed");
    }

    // Promote Capture
    board.setupPosition("8/8/8/8/8/8/3p4/4Q3 b - - 0 1");
    move = Move(52, 59, 6, 4, 0, 10, Move::MoveType::PromoteCapture, true, true, true, true); // black pawn d2 captures white queen e1 and promotes to black queen
    board.makeMove(move);
    if(board.pieceTypeAtSquare(59) != 10 && board.pieceTypeAtSquare(52) != -1 && board.pieceTypeAtSquare(59) != 4) {
        throw std::invalid_argument("Promote Capture Test Failed");
    }

    // White Castle King Side
    board.setupPosition("8/8/8/8/8/8/8/4K2R w KQkq - 0 1");
    move = Move(59, 57, 0, 0, 0, 0, Move::MoveType::CastleKingSide, true, true, true, true); // white king castles king side
    board.makeMove(move);
    if(board.pieceTypeAtSquare(57) != 5 && board.pieceTypeAtSquare(58) != 3 && board.pieceTypeAtSquare(59) != -1 && board.pieceTypeAtSquare(56) != -1 && board.pieceTypeAtSquare(63) != -1) {
        throw std::invalid_argument("White Castle King Side Test Failed");
    }

    // White Castle Queen Side
    board.setupPosition("8/8/8/8/8/8/8/R3K3 b KQkq - 0 1");
    move = Move(59, 61, 0, 0, 0, 0, Move::MoveType::CastleQueenSide, true, true, true, true); // white king castles queen side
    board.makeMove(move);
    if(board.pieceTypeAtSquare(61) != 5 && board.pieceTypeAtSquare(60) != 3 && board.pieceTypeAtSquare(59) != -1 && board.pieceTypeAtSquare(56) != -1 && board.pieceTypeAtSquare(63) != -1) {
        throw std::invalid_argument("White Castle Queen Side Test Failed");
    }

    // Black Castle King Side
    board.setupPosition("4k2r/8/8/8/8/8/8/8 b KQkq - 0 1");
    move = Move(3, 1, 6, 0, 0, 0, Move::MoveType::CastleKingSide, true, true, true, true); // black king castles king side
    board.makeMove(move);
    if(board.pieceTypeAtSquare(1) != 11 && board.pieceTypeAtSquare(2) != 9 && board.pieceTypeAtSquare(3) != -1 && board.pieceTypeAtSquare(0) != -1 && board.pieceTypeAtSquare(7) != -1) {
        throw std::invalid_argument("Black Castle King Side Test Failed");
    }

    // Black Castle Queen Side
    board.setupPosition("r3k3/8/8/8/8/8/8/8 w KQkq - 0 1");
    move = Move(3, 5, 6, 0, 0, 0, Move::MoveType::CastleQueenSide, true, true, true, true); // black king castles queen side
    board.makeMove(move);
    if(board.pieceTypeAtSquare(5) != 11 && board.pieceTypeAtSquare(4) != 9 && board.pieceTypeAtSquare(3) != -1 && board.pieceTypeAtSquare(0) != -1 && board.pieceTypeAtSquare(7) != -1) {
        throw std::invalid_argument("Black Castle Queen Side Test Failed");
    }

    // Move(fromSquare, toSquare, pieceType, capturedPiece, enPassantSquare, promotedPieceType, moveType, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling)
    std:: cout << "All Tests Passed!" << std::endl;
}


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




    //Move move = Move(56, 40, 3, 0, 0, 0, Move::MoveType::Normal, true, true, true, true);
    //move.display();
    //board.makeMove(move);
    //board.printSingleBitboards();     
    //cout << "FEN Board: \n";
    //board.printFENBoard();

    unitTest();
    return 0;
}




