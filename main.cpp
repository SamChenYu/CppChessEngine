#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "board.h"
#include "move.h"
#include "evaluation.h"
// cd ~/Desktop/C++ChessEngine
// g++ -std=c++11 -o chessengine.out main.cpp board.cpp move.cpp evaluation.cpp
// ./chessengine.out
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

using namespace std;

int nodesSearched = 0;
int maxDepth = 10;
int bestMoveIndex = -1;
int secondBestMoveIndex = -1;
int thirdBestMoveIndex = -1;

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
    move = Move(28, 19, 0, 6, 19, 0, Move::MoveType::EnPassantCapture, true, true, true, true); //  white pawn e5 captures black pawn on d5
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
    move = Move(28, 19, 0, 6, 19, 0, Move::MoveType::EnPassantCapture, true, true, true, true); //  white pawn e5 captures black pawn on d5
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


    // Testing kingChecks()

    std:: cout << " ----------------------------------------------------------------------" << std:: endl;

    board.setupPosition("8/8/8/8/8/1p6/2K5/8 b - - 0 1");
    std:: cout<< "White King in Check from Black Pawn: \n";
    board.printFENBoard();
    if(!board.isKingInCheck()) {
        throw std::invalid_argument("White King in Check from Black Pawn Test Failed");
    }

    board.setupPosition("8/8/8/8/3n4/8/2K5/8 b - - 0 1");
    std:: cout<< "White King in Check from Black Knight: \n";
    board.printFENBoard();
    if(!board.isKingInCheck()) {
        throw std::invalid_argument("White King in Check from Black Knight Test Failed");
    }

    board.setupPosition("8/8/8/6b1/8/8/3K4/8 b - - 0 1");
    std:: cout<< "White King in Check from Black Bishop: \n";
    board.printFENBoard();
    if(!board.isKingInCheck()) {
        throw std::invalid_argument("White King in Check from Black Bishop Test Failed");
    }

    board.setupPosition("8/8/8/6b1/5n2/8/3K4/8 b - - 0 1");
    std:: cout<< "White King not in check by blocked bishop: \n";
    board.printFENBoard();
    if(board.isKingInCheck()) {
        throw std::invalid_argument("White King not in check by blocked bishop Test Failed");
    }

    board.setupPosition("8/8/8/3r4/8/8/3K4/8 b - - 0 1");
    std:: cout<< "White King in Check from Black Rook: \n";
    board.printFENBoard();
    if(!board.isKingInCheck()) {
        throw std::invalid_argument("White King in Check from Black Rook Test Failed");
    }

    board.setupPosition("8/8/8/3r4/8/3R4/3K4/8 b - - 0 1");
    std:: cout<< "White King not in check by blocked rook: \n";
    board.printFENBoard();
    if(board.isKingInCheck()) {
        throw std::invalid_argument("White King not in check by blocked rook Test Failed");
    }

    board.setupPosition("8/8/8/3q4/8/8/3K4/8 b - - 0 1");
    std:: cout<< "White King in Check from Black Queen: \n";
    board.printFENBoard();
    if(!board.isKingInCheck()) {
        throw std::invalid_argument("White King in Check from Black Queen Test Failed");
    }

    board.setupPosition("8/8/8/3q4/8/3P4/3K4/8 b - - 0 1");
    std:: cout<< "White King not in check by blocked queen: \n";
    board.printFENBoard();
    if(board.isKingInCheck()) {
        throw std::invalid_argument("White King not in check by blocked queen Test Failed");
    }
    
    std:: cout << "All isKingInCheck() Tests Passed!" << std::endl;

}

double minimax(Board& board, int depth, double alpha, double beta, bool isMaximising) {
    nodesSearched++;

    if(depth == maxDepth) {
        return evaluate(board);
    }

    vector<Move> moves = board.legalMoveGeneration();

    // checking if a leaf is a terminal node
    if(moves.empty()) {
        int terminate = board.isGameOver();
        if(terminate != 2) {
            if(terminate == -1 || terminate == 0 || terminate == 1) { // need to refactor -> this checks checkmate / stalemate 
                return terminate;
            }
        }
    }

    // minimax
    if (isMaximising) {
        double bestValue = -std::numeric_limits<double>::infinity();
        double secondBestValue = -std::numeric_limits<double>::infinity() + 1.0;
        double thirdBestValue = -std::numeric_limits<double>::infinity() + 2.0;

        for (int i = 0; i < moves.size(); i++) {
            Move a = moves[i];
            cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
            board.printJustFENBoard();
            cout << "making for ";
            moves[i].display();
            board.makeMove(moves[i]);
            board.printJustFENBoard();
            board.flipColour();
            double tempValue = minimax(board, depth + 1, alpha, beta, false);
            board.flipColour();
            cout << "----------------------------------------" << endl;
            board.printJustFENBoard();
            cout << "undoing for ";
            moves[i].display();
            board.undoMove(moves[i]);
            board.printJustFENBoard();
            cout << "----------------------------------------" << endl;

            if (tempValue >= bestValue) {
                thirdBestValue = secondBestValue;
                secondBestValue = bestValue;
                bestValue = tempValue;

                if (depth == 0) {
                    thirdBestMoveIndex = secondBestMoveIndex;
                    secondBestMoveIndex = bestMoveIndex;
                    bestMoveIndex = i;
                }
            } else if (tempValue >= secondBestValue) {
                thirdBestValue = secondBestValue;
                secondBestValue = tempValue;

                if (depth == 0) {
                    thirdBestMoveIndex = secondBestMoveIndex;
                    secondBestMoveIndex = i;
                }
            } else if (tempValue >= thirdBestValue) {
                thirdBestValue = tempValue;

                if (depth == 0) {
                    thirdBestMoveIndex = i;
                }
            }

            alpha = std::max(alpha, bestValue);

            // Pruning
            if (beta <= alpha) {
                break;
            }
        }
        return bestValue;
    } else {
        double leastValue = std::numeric_limits<double>::infinity();
        double secondLeastValue = std::numeric_limits<double>::infinity() + 1.0;
        double thirdLeastValue = std::numeric_limits<double>::infinity();

        for (int i = 0; i < moves.size(); i++) {
            Move a = moves[i];
            cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
            board.printJustFENBoard();
            cout << "making for ";
            moves[i].display();
            board.makeMove(moves[i]);
            board.printJustFENBoard();
            board.flipColour();
            double tempValue = minimax(board, depth + 1, alpha, beta, false);
            board.flipColour();
            cout << "----------------------------------------" << endl;
            board.printJustFENBoard();
            cout << "undoing for ";
            moves[i].display();
            board.undoMove(moves[i]);
            board.printJustFENBoard();
            cout << "----------------------------------------" << endl;

            if (tempValue <= leastValue) {
                thirdLeastValue = secondLeastValue;
                secondLeastValue = leastValue;
                leastValue = tempValue;

                if (depth == 0) {
                    thirdBestMoveIndex = secondBestMoveIndex;
                    secondBestMoveIndex = bestMoveIndex;
                    bestMoveIndex = i;
                }
            } else if (tempValue <= secondLeastValue) {
                thirdLeastValue = secondLeastValue;
                secondLeastValue = tempValue;

                if (depth == 0) {
                    thirdBestMoveIndex = secondBestMoveIndex;
                    secondBestMoveIndex = i;
                }
            } else if (tempValue <= thirdLeastValue) {
                thirdLeastValue = tempValue;

                if (depth == 0) {
                    thirdBestMoveIndex = i;
                }
            }

            beta = std::min(beta, leastValue);

            // Pruning
            if (beta <= alpha) {
                break;
            }
        }
        return leastValue;
    }
}

int main() {
    cout << "Enter FEN Notation / Empty For Default Position: \n";
    string fen;
    getline(cin, fen); // input from terminal
    Board board;
    if(fen.empty()) {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        board.setupPosition(fen);
    } else {
        board.setupPosition(fen);
    }
    cout << "FEN Board: \n";
    board.printFENBoard();
    double eval = evaluate(board);
    //cout << "Evaluation: " << eval << endl;
    //cout << "isKingInCheck: " << board.isKingInCheck() << endl;
    //unitTest();
    // 3q4/8/8/2b1np2/1r6/8/3K4/8 b - - 0 1
    vector<Move> moves = board.legalMoveGeneration();
    cout << "Number of Moves: " << moves.size() << endl;
    for (int i = 0; i < moves.size(); i++) {
        moves[i].toString();
        //moves[i].display();
    }

    //minimax(board, 0, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), true);
    //board.printFENBoard();
    //cout << "Nodes Searched: " << nodesSearched << endl;
    //cout << "Best Move: " << bestMoveIndex << endl;
    //moves[bestMoveIndex].toString();
    //unitTest();

    
    return 0;
}



