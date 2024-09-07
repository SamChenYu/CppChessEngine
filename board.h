#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>
#include <vector>
#include "move.h"

using Bitboard = uint64_t;

class Board {
public:
    Bitboard bitboards[12];

    const uint64_t FILE_A = 0x8080808080808080ULL;  // File A (1st column, flipped)
    const uint64_t FILE_B = 0x4040404040404040ULL;  // File B (2nd column, flipped)
    const uint64_t FILE_C = 0x2020202020202020ULL;  // File C (3rd column, flipped)
    const uint64_t FILE_D = 0x1010101010101010ULL;  // File D (4th column, flipped)
    const uint64_t FILE_E = 0x0808080808080808ULL;  // File E (5th column, flipped)
    const uint64_t FILE_F = 0x0404040404040404ULL;  // File F (6th column, flipped)
    const uint64_t FILE_G = 0x0202020202020202ULL;  // File G (7th column, flipped)
    const uint64_t FILE_H = 0x0101010101010101ULL;  // File H (8th column, flipped)



    void setupInitialPosition();
    std::vector<std::string> split(const std::string& str, char delimiter);
    void setupPosition(const std::string& fen);
    void removePiece(int pieceType, int square);
    void addPiece(int pieceType, int square);
    void movePiece(int pieceType, int fromSquare, int toSquare);
    void printFENBoard();
    void printSingleBitboards();
    void printBitboard(uint64_t bb) const;
    void printJustFENBoard();
    int coordinateToBitboardIndex(const std::string& coordinate) const;
    int pieceTypeAtSquare(int square);
    void makeMove(const Move& move);
    void undoMove(const Move& move);
    void flipColour();

    bool isKingInCheck();
    std::vector<Move> legalMoveGeneration();
    std::vector<Move> pseudoLegalMoves();
    int isGameOver();

    uint64_t generateKnightAttacks(int square) const;
    uint64_t generateBishopAttacks(int square, uint64_t blockers) const;
    uint64_t generateRookAttacks(int square, uint64_t blockers) const;
    uint64_t generateKingAttacks(int square) const;

private:
    
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
