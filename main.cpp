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

    // Testing makeMoves()
    Board board;

    // Normal Move
    board.setupPosition("8/8/8/8/8/8/8/P7 b - - 0 1");
    std::cout << "Before Normal Move: \n";
    board.printFENBoard();
    Move move = Move(56, 48, 0, 0, 0, 0, Move::MoveType::Normal, true, true, true, true); // white pawn on a1 moves to a2
    board.makeMove(move);
    std::cout << "After Normal Move: \n";
    board.printFENBoard();
    if (board.pieceTypeAtSquare(48) != 0 || board.pieceTypeAtSquare(56) != -1) {
        throw std::invalid_argument("Normal Move Test Failed");
    }

    // Moved Twice
    board.setupPosition("8/8/8/8/8/8/8/P7 b - - 0 1");
    std::cout << "Before Moved Twice: \n";
    board.printFENBoard();
    move = Move(56, 40, 0, 0, 0, 0, Move::MoveType::MovedTwice, true, true, true, true); // white pawn on a1 moves to a3
    board.makeMove(move);
    std::cout << "After Moved Twice: \n";
    board.printFENBoard();
    if (board.pieceTypeAtSquare(40) != 0 || board.pieceTypeAtSquare(56) != -1) {
        throw std::invalid_argument("Moved Twice Test Failed");
    }

    // Capture
    // Move(fromSquare, toSquare, pieceType, capturedPiece, enPassantSquare, promotedPieceType, moveType, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling)

    board.setupPosition("7n/8/8/8/8/8/8/7R w - - 0 1");
    std::cout << "Before Capture: \n";
    board.printFENBoard();
    move = Move(63, 7, 3, 7, 0, 0, Move::MoveType::Capture, true, true, true, true); // white rook on h1 captures black knight at h8
    board.makeMove(move);
    std::cout << "After Capture: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(7) != 3 || board.pieceTypeAtSquare(63) != -1) {
        throw std::invalid_argument("Capture Test Failed");
    }

    // En Passant Capture
    board.setupPosition("8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    std::cout << "Before En Passant Capture: \n";
    board.printFENBoard();
    move = Move(28, 19, 0, 6, 27, 0, Move::MoveType::EnPassantCapture, true, true, true, true); //  white pawn e5 captures black pawn on d5
    board.makeMove(move);
    std::cout << "After En Passant Capture: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(19) != 0 || board.pieceTypeAtSquare(27) != -1 || board.pieceTypeAtSquare(28) != -1 ) {
        throw std::invalid_argument("En Passant Capture Test Failed");
    }

    // Promote
    board.setupPosition("8/8/8/8/8/8/3p4/8 b - - 0 1");
    std::cout << "Before Promote: \n";
    board.printFENBoard();
    move = Move(51, 59, 6, 0, 0, 10, Move::MoveType::Promote, true, true, true, true); // black pawn d2 promotes to black queen
    board.makeMove(move);
    std::cout << "After Promote: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(59) != 10 || board.pieceTypeAtSquare(51) != -1) {
        throw std::invalid_argument("Promote Test Failed");
    }

    // Promote Capture
    board.setupPosition("8/8/8/8/8/8/3p4/4Q3 b - - 0 1");
    std::cout << "Before Promote Capture: \n";
    board.printFENBoard();
    move = Move(51, 60, 6, 4, 0, 10, Move::MoveType::PromoteCapture, true, true, true, true); // black pawn d2 captures white queen e1 and promotes to black queen
    board.makeMove(move);
    std::cout << "After Promote Capture: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(51) != -1 || board.pieceTypeAtSquare(60) != 10) {
        throw std::invalid_argument("Promote Capture Test Failed");
    }

    // White Castle King Side
    board.setupPosition("8/8/8/8/8/8/8/4K2R w KQkq - 0 1");
    std::cout << "Before White Castle King Side: \n";
    board.printFENBoard();
    move = Move(60, 62, 5, 0, 0, 0, Move::MoveType::CastleKingSide, true, true, true, true); // white king castles king side
    board.makeMove(move);
    std::cout << "After White Castle King Side: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(62) != 5 || board.pieceTypeAtSquare(61) != 3 || board.pieceTypeAtSquare(63) != -1 || board.pieceTypeAtSquare(60) != -1) {
        throw std::invalid_argument("White Castle King Side Test Failed");
    }

    // White Castle Queen Side
    board.setupPosition("8/8/8/8/8/8/8/R3K3 b KQkq - 0 1");
    std::cout << "Before White Castle Queen Side: \n";
    board.printFENBoard();
    move = Move(60, 58, 5, 0, 0, 0, Move::MoveType::CastleQueenSide, true, true, true, true); // white king castles queen side
    board.makeMove(move);
    std::cout << "After White Castle Queen Side: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(58) != 5 || board.pieceTypeAtSquare(59) != 3 || board.pieceTypeAtSquare(56) != -1 || board.pieceTypeAtSquare(57) != -1 || board.pieceTypeAtSquare(60) != -1) {
        throw std::invalid_argument("White Castle Queen Side Test Failed");
    }

    // Black Castle King Side
    board.setupPosition("4k2r/8/8/8/8/8/8/8 b KQkq - 0 1");
    std::cout << "Before Black Castle King Side: \n";
    board.printFENBoard();
    move = Move(4, 6, 11, 0, 0, 0, Move::MoveType::CastleKingSide, true, true, true, true); // black king castles king side
    board.makeMove(move);
    std::cout << "After Black Castle King Side: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(6) != 11 || board.pieceTypeAtSquare(5) != 9 || board.pieceTypeAtSquare(4) != -1 || board.pieceTypeAtSquare(7) != -1 ) {
        throw std::invalid_argument("Black Castle King Side Test Failed");
    }

    // Black Castle Queen Side
    board.setupPosition("r3k3/8/8/8/8/8/8/8 w KQkq - 0 1");
    std::cout << "Before Black Castle Queen Side: \n";
    board.printFENBoard();
    move = Move(4, 2, 11, 0, 0, 0, Move::MoveType::CastleQueenSide, true, true, true, true); // black king castles queen side
    board.makeMove(move);
    std::cout << "After Black Castle Queen Side: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(2) != 11 || board.pieceTypeAtSquare(3) != 9 || board.pieceTypeAtSquare(0) != -1 || board.pieceTypeAtSquare(1) != -1 || board.pieceTypeAtSquare(4) != -1) {
        throw std::invalid_argument("Black Castle Queen Side Test Failed");
    }

    // Move(fromSquare, toSquare, pieceType, capturedPiece, enPassantSquare, promotedPieceType, moveType, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling)
    std:: cout << "All MakeMove() Tests Passed!" << std::endl;

    std:: cout << " ----------------------------------------------------------------------" << std:: endl;
    // Testing undoMove()

    // Normal Move
    board.setupPosition("8/8/8/8/8/P7/8/8 b - - 0 1");
    std:: cout<< "Before Normal Move: \n";
    board.printFENBoard();
    move = Move(48, 40, 0, 0, 0, 0, Move::MoveType::Normal, true, true, true, true); // white pawn on a1 moves to a2
    board.undoMove(move);
    std:: cout<< "After Normal Move: \n";
    board.printFENBoard();
    if (board.pieceTypeAtSquare(48) != 0 || board.pieceTypeAtSquare(40) != -1) {
        throw std::invalid_argument("Undo Normal Move Test Failed");
    }

    // Moved Twice
    board.setupPosition("8/8/8/8/8/P7/8/8 b - - 0 1");
    std:: cout<< "Before Moved Twice: \n";
    board.printFENBoard();
    move = Move(56, 40, 0, 0, 0, 0, Move::MoveType::MovedTwice, true, true, true, true); // white pawn on a1 moves to a3
    board.undoMove(move);
    std:: cout<< "After Moved Twice: \n";
    board.printFENBoard();
    if (board.pieceTypeAtSquare(56) != 0 || board.pieceTypeAtSquare(40) != -1) {
        throw std::invalid_argument("Undo Moved Twice Test Failed");
    }

    // Capture
    // Move(fromSquare, toSquare, pieceType, capturedPiece, enPassantSquare, promotedPieceType, moveType, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling)
    board.setupPosition("7R/8/8/8/8/8/8/8 b - - 0 1");
    std:: cout<< "Before Capture: \n";
    board.printFENBoard();
    move = Move(63, 7, 3, 7, 0, 0, Move::MoveType::Capture, true, true, true, true); // white rook on h1 captures black knight at h8
    board.undoMove(move);
    std:: cout<< "After Capture: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(63) != 3 || board.pieceTypeAtSquare(7) != 7) {
        throw std::invalid_argument("Undo Capture Test Failed");
    }

    // En Passant Capture
    board.setupPosition("8/8/3P4/8/8/8/8/8 b - - 0 1");
    std:: cout<< "Before En Passant Capture: \n";
    board.printFENBoard();
    move = Move(28, 19, 0, 6, 27, 0, Move::MoveType::EnPassantCapture, true, true, true, true); //  white pawn e5 captures black pawn on d5
    board.undoMove(move);
    std:: cout<< "After En Passant Capture: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(28) != 0 || board.pieceTypeAtSquare(27) != 6 || board.pieceTypeAtSquare(19) != -1) {
        throw std::invalid_argument("Undo En Passant Capture Test Failed");
    }

    // Promote
    board.setupPosition("8/8/8/8/8/8/8/3q4 w - - 0 1");
    std:: cout<< "Before Promote: \n";
    board.printFENBoard();
    move = Move(51, 59, 6, 0, 0, 10, Move::MoveType::Promote, true, true, true, true); // black pawn d2 promotes to black queen
    board.undoMove(move);
    std:: cout<< "After Promote: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(51) != 6 || board.pieceTypeAtSquare(60) != -1) {
        throw std::invalid_argument("Undo Promote Test Failed");
    }

    // Promote Capture
    board.setupPosition("8/8/8/8/8/8/8/4q3 w - - 0 1");
    std:: cout<< "Before Promote Capture: \n";
    board.printFENBoard();
    move = Move(51, 60, 6, 4, 0, 10, Move::MoveType::PromoteCapture, true, true, true, true); // black pawn d2 captures white queen e1 and promotes to black queen
    board.undoMove(move);
    std:: cout<< "After Promote Capture: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(60) != 4 || board.pieceTypeAtSquare(51) != 6) {
        throw std::invalid_argument("Undo Promote Capture Test Failed");
    }


    // White Castle King Side
    board.setupPosition("8/8/8/8/8/8/8/5RK1 b KQkq - 0 1");
    std:: cout<< "Before White Castle King Side: \n";
    board.printFENBoard();
    move = Move(60, 62, 5, 0, 0, 0, Move::MoveType::CastleKingSide, true, true, true, true); // white king castles king side
    board.undoMove(move);
    std:: cout<< "After White Castle King Side: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(60) != 5 || board.pieceTypeAtSquare(63) != 3 || board.pieceTypeAtSquare(61) != -1 || board.pieceTypeAtSquare(62) != -1) {
        throw std::invalid_argument("Undo White Castle King Side Test Failed");
    }

    // White Castle Queen Side
    board.setupPosition("8/8/8/8/8/8/8/2KR4 b KQkq - 0 1");
    std:: cout<< "Before White Castle Queen Side: \n";
    board.printFENBoard();
    move = Move(60, 58, 5, 0, 0, 0, Move::MoveType::CastleQueenSide, true, true, true, true); // white king castles queen side
    board.undoMove(move);
    std:: cout<< "After White Castle Queen Side: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(60) != 5 || board.pieceTypeAtSquare(56) != 3 || board.pieceTypeAtSquare(57) != -1 || board.pieceTypeAtSquare(58) != -1 || board.pieceTypeAtSquare(59) != -1) {
        throw std::invalid_argument("Undo White Castle Queen Side Test Failed");
    }

    // Black Castle King Side
    board.setupPosition("5rk1/8/8/8/8/8/8/8 w KQkq - 0 1");
    std:: cout<< "Before Black Castle King Side: \n";
    board.printFENBoard();
    move = Move(4, 6, 11, 0, 0, 0, Move::MoveType::CastleKingSide, true, true, true, true); // black king castles king side
    board.undoMove(move);
    std:: cout<< "After Black Castle King Side: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(4) != 11 || board.pieceTypeAtSquare(7) != 9 || board.pieceTypeAtSquare(5) != -1 || board.pieceTypeAtSquare(6) != -1) {
        throw std::invalid_argument("Undo Black Castle King Side Test Failed");
    }

    // Black Castle Queen Side
    board.setupPosition("2kr4/8/8/8/8/8/8/8 w KQkq - 0 1");
    std:: cout<< "Before Black Castle Queen Side: \n";
    board.printFENBoard();
    move = Move(4, 2, 11, 0, 0, 0, Move::MoveType::CastleQueenSide, true, true, true, true); // black king castles queen side
    board.undoMove(move);
    std:: cout<< "After Black Castle Queen Side: \n";
    board.printFENBoard();
    if(board.pieceTypeAtSquare(4) != 11 || board.pieceTypeAtSquare(0) != 9 || board.pieceTypeAtSquare(1) != -1 || board.pieceTypeAtSquare(2) != -1 || board.pieceTypeAtSquare(3) != -1) {
        throw std::invalid_argument("Undo Black Castle Queen Side Test Failed");
    }

    std:: cout << "All UndoMove() Tests Passed!" << std::endl;

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




