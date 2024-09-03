#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>
#include <vector>
#include "move.h"

using Bitboard = uint64_t;

class Board {
public:

    void setupInitialPosition();
    std::vector<std::string> split(const std::string& str, char delimiter);
    void setupPosition(const std::string& fen);
    void removePiece(int pieceType, int square);
    void addPiece(int pieceType, int square);
    void movePiece(int pieceType, int fromSquare, int toSquare);
    void printFENBoard();
    void printSingleBitboards();

    int pieceTypeAtSquare(int square);
    void makeMove(const Move& move);
    void undoMove(const Move& move);

private:
    Bitboard bitboards[12];
    bool whiteKingSideCastling;
    bool whiteQueenSideCastling;
    bool blackKingSideCastling;
    bool blackQueenSideCastling;
    int enPassantSquare;
    int halfmoveClock;
    int fullmoveNumber;
    bool whiteToMove;
    
    
};

#endif // BOARD_H
