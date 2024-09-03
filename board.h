#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>
#include <vector>
using Bitboard = uint64_t;

class Board {
public:
    Bitboard bitboards[12];
    bool whiteKingSideCastling;
    bool whiteQueenSideCastling;
    bool blackKingSideCastling;
    bool blackQueenSideCastling;
    int enPassantSquare;
    int halfmoveClock;
    int fullmoveNumber;
    bool whiteToMove;

    void setupInitialPosition();
    std::vector<std::string> split(const std::string& str, char delimiter);
    void setupPosition(const std::string& fen);
    void movePiece(int pieceType, int fromSquare, int toSquare);
    void printFENBoard();
    void printSingleBitboards();
};

#endif // BOARD_H
