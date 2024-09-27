#include "move.h"
#include <iostream>
#include <string>

using namespace std;

std::string Move::moveTypeToString() const {
    switch (moveType) {
        case MoveType::Normal: return "Normal";
        case MoveType::MovedTwice: return "Moved Twice";
        case MoveType::Capture: return "Capture";
        case MoveType::EnPassantCapture: return "En Passant Capture";
        case MoveType::Promote: return "Promote";
        case MoveType::PromoteCapture: return "Promote Capture";
        case MoveType::CastleKingSide: return "Castle King Side";
        case MoveType::CastleQueenSide: return "Castle Queen Side";
        default: return "Unknown MoveType";
    }
};  // Function to convert MoveType to string


// debugging
void Move::display() const {
    string pieces[] = {
        "White Pawn", "White Knight","White Bishop", "White Rook", "White Queen", "White King",
        "Black Pawn", "Black Knight", "Black Bishop", "Black Rook", "Black Queen", "Black King"
    };
    cout << pieces[pieceType] << " moves from Square: " << bitboardIndexToCoordinate(fromSquare) << " to Square: " << bitboardIndexToCoordinate(toSquare) << " (" << moveTypeToString() << ")" << endl;
    cout << "capturedPieceType: " << capturedPieceType << endl;
    cout << "enPassantSquare: " << enPassantSquare << endl;
    cout << "promotedPieceType: " << promotedPieceType << endl;
    cout << "whiteKingSideCastling: " << whiteKingSideCastling << endl;
    cout << "whiteQueenSideCastling: " << whiteQueenSideCastling << endl;
    cout << "blackKingSideCastling: " << blackKingSideCastling << endl;
    cout << "blackQueenSideCastling: " << blackQueenSideCastling << endl;
}

// for displaying to user
std::string Move::toString() const {
    string pieces[] = {
        "White Pawn", "White Knight", "White Bishop", "White Rook", "White Queen", "White King",
        "Black Pawn", "Black Knight", "Black Bishop", "Black Rook", "Black Queen", "Black King"
    };

    string result = pieces[pieceType] + " moves from Square: " + bitboardIndexToCoordinate(fromSquare) +
                    " to Square: " + bitboardIndexToCoordinate(toSquare) + " (" + moveTypeToString() + ")";

    if (moveType == MoveType::Capture || moveType == MoveType::PromoteCapture) {
        result += " and captures " + pieces[capturedPieceType];
    }

    if (moveType == MoveType::Promote || moveType == MoveType::PromoteCapture) {
        result += " and promotes to " + pieces[promotedPieceType];
    }

    return result;
}



std::string Move::bitboardIndexToCoordinate(int index) const {
    if (index < 0 || index >= 64) {
        std::cout<< "Index: " << index << std::endl;    
        throw std::out_of_range("Index must be between 0 and 63.");
    }

    // Calculate file (column) and rank (row) based on flipped bitboard (bit 0 = a8, bit 1 = b8)
    int file = index % 8;
    int rank = index / 8;

    // Convert file and rank to chess coordinate
    char fileChar = 'a' + file;  // 'a' for file 0, 'b' for file 1, etc.
    char rankChar = '8' - rank;  // '8' for rank 0, '7' for rank 1, etc.

    // Create the coordinate string
    std::string coordinate;
    coordinate += fileChar;
    coordinate += rankChar;

    return coordinate;
}
