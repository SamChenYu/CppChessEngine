#include "board.h"
#include <iostream>
#include <string>

using namespace std;

// Setup initial position
void Board::setupInitialPosition() {
    // White Pieces
    bitboards[0] = 0x000000000000FF00ULL; // White Pawns
    bitboards[1] = 0x0000000000000042ULL; // White Knights
    bitboards[2] = 0x0000000000000024ULL; // White Bishops
    bitboards[3] = 0x0000000000000081ULL; // White Rooks
    bitboards[4] = 0x0000000000000008ULL; // White Queen
    bitboards[5] = 0x0000000000000010ULL; // White King

    // Black Pieces
    bitboards[6] = 0x00FF000000000000ULL; // Black Pawns
    bitboards[7] = 0x4200000000000000ULL; // Black Knights
    bitboards[8] = 0x2400000000000000ULL; // Black Bishops
    bitboards[9] = 0x8100000000000000ULL; // Black Rooks
    bitboards[10] = 0x0800000000000000ULL; // Black Queen
    bitboards[11] = 0x1000000000000000ULL; // Black King
}

// Move a piece from one square to another
void Board::movePiece(int pieceType, int fromSquare, int toSquare) {
    bitboards[pieceType] &= ~(1ULL << fromSquare); // Clear the original square
    bitboards[pieceType] |= (1ULL << toSquare);    // Set the destination square
}

void Board::printFENBoard() {
    const char pieceSymbols[] = "PNBRQKpnbrqk";

    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            char pieceChar = '-'; // EmptySquare
            
            for (int pieceType = 0; pieceType < 12; ++pieceType) {
                if (bitboards[pieceType] & (1ULL << square)) {
                    pieceChar = pieceSymbols[pieceType];
                    break;
                }
            }
            std::cout << pieceChar << ' ';
        }
        std::cout << std::endl;
    }
}

void Board::printSingleBitboards() {

    std::string pieces[] = {
        "White Pawn", "White Knight","White Bishop", "White Rook", "White Queen", "White King",
        "Black Pawn", "Black Knight", "Black Bishop", "Black Rook", "Black Queen", "Black King"
    };

    const char pieceSymbols[] = "PNBRQKpnbrqk";

    for(int index=0; index<12; index++) {
        
        Bitboard bb = bitboards[index];
        std::cout << pieces[index] << ":\n";
        for (int rank = 7; rank >= 0; --rank) {
            for (int file = 0; file < 8; ++file) {
                int square = rank * 8 + file;
                char pieceChar = '-'; // Default to empty square
                
                if (bb & (1ULL << square)) {
                    pieceChar = pieceSymbols[index];  
                }
                std::cout << pieceChar << ' ';
            }
            std::cout << '\n'; // New rank
        }
    }
}