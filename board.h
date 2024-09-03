#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
using Bitboard = uint64_t;

class Board {
public:
    Bitboard bitboards[12];

    void setupInitialPosition();
    void movePiece(int pieceType, int fromSquare, int toSquare);
    void printFENBoard();
    void printSingleBitboards();
};

#endif // BOARD_H
