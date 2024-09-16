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

    const uint64_t RANK_1 = 0x00000000000000FFULL;  // Rank 1 (Bottom row, 1st rank)
    const uint64_t RANK_2 = 0x000000000000FF00ULL;  // Rank 2
    const uint64_t RANK_3 = 0x0000000000FF0000ULL;  // Rank 3
    const uint64_t RANK_4 = 0x00000000FF000000ULL;  // Rank 4
    const uint64_t RANK_5 = 0x000000FF00000000ULL;  // Rank 5
    const uint64_t RANK_6 = 0x0000FF0000000000ULL;  // Rank 6
    const uint64_t RANK_7 = 0x00FF000000000000ULL;  // Rank 7
    const uint64_t RANK_8 = 0xFF00000000000000ULL;  // Rank 8 (Top row, 8th rank)


    /*
        Precomputed Attack Bitboards    
    */
   void precomputeAttackBitboards();
   uint64_t knightAttacks[64];
   uint64_t getKnightAttacks(int square);





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

    uint64_t generateBishopXRay(int square, uint64_t blockers) const;
    uint64_t generateRookXRay(int square, uint64_t blockers) const;


    uint64_t kingDangerSquares() const;
    bool isWhiteToMove() const {
        return whiteToMove;
    }

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
