#include "board.h"
#include "move.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <bitset>

using namespace std;


void Board::precomputeAttackBitboards() {
    for (int square = 0; square < 64; ++square) {
        knightAttacks[square] = generateKnightAttacks(square);
    }
}

void Board::setupInitialPosition() {

    fill(begin(bitboards), end(bitboards), 0ULL);
    // White Pieces
    bitboards[0] = 0x00FF000000000000ULL; // White Pawns
    bitboards[1] = 0x4200000000000000ULL;// White Knights
    bitboards[2] = 0x2400000000000000ULL;// White Bishops
    bitboards[3] =  0x8100000000000000ULL; // White Rooks
    bitboards[4] = 0x0800000000000000ULL;// White Queen
    bitboards[5] =  0x1000000000000000ULL;// White King

    // Black Pieces
    bitboards[6] = 0x000000000000FF00ULL; // Black Pawns
    bitboards[7] = 0x0000000000000042ULL; // Black Knights
    bitboards[8] = 0x0000000000000024ULL; // Black Bishops
    bitboards[9] = 0x0000000000000081ULL; // Black Rooks
    bitboards[10] = 0x0000000000000008ULL; // Black Queen
    bitboards[11] = 0x0000000000000010ULL; // Black King



    whiteKingSideCastling = true;
    whiteQueenSideCastling = true;
    blackKingSideCastling = true;
    blackQueenSideCastling = true;
    enPassantSquare = -1;
    halfmoveClock = 0;
    fullmoveNumber = 0;
    whiteToMove = true;
}

// Function to split a string by a delimiter
vector<string> Board::split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void Board::setupPosition(const std::string& fen) {
    vector<string> splitString = split(fen, ' '); // split the board from other FEN variables
    if (splitString.size() != 6) {
        throw invalid_argument("Invalid FEN Notation");
    }

    vector<string> boardRows = split(splitString[0], '/'); // split the board ranks
    if (boardRows.size() != 8) {
        throw invalid_argument("Invalid FEN Notation");
    }

    // Reset bitboards
    fill(begin(bitboards), end(bitboards), 0ULL);

    // Iterate from the top row (rank 0) to the bottom row (rank 7)
    for (int rank = 0; rank < 8; ++rank) {
        int file = 0;
        for (char c : boardRows[rank]) {
            if (isdigit(c)) {
                int emptySquares = c - '0';
                file += emptySquares;
            } else {
                int pieceType = -1;
                switch (c) {
                    case 'P': pieceType = 0; break; // White Pawn
                    case 'N': pieceType = 1; break; // White Knight
                    case 'B': pieceType = 2; break; // White Bishop
                    case 'R': pieceType = 3; break; // White Rook
                    case 'Q': pieceType = 4; break; // White Queen
                    case 'K': pieceType = 5; break; // White King
                    case 'p': pieceType = 6; break; // Black Pawn
                    case 'n': pieceType = 7; break; // Black Knight
                    case 'b': pieceType = 8; break; // Black Bishop
                    case 'r': pieceType = 9; break; // Black Rook
                    case 'q': pieceType = 10; break; // Black Queen
                    case 'k': pieceType = 11; break; // Black King
                    default: throw invalid_argument("Invalid FEN Notation");
                }

                int square = (rank * 8) + file;
                bitboards[pieceType] |= (1ULL << square);
                file++;
            }
        }

        if (file != 8) {
            throw invalid_argument("Invalid FEN Notation");
        }
    }
    // Setup additional FEN variables
    whiteToMove = (splitString[1].find('w') != string::npos);
    string castlingRights = splitString[2];
    whiteKingSideCastling = castlingRights.find('K') != string::npos;
    whiteQueenSideCastling = castlingRights.find('Q') != string::npos;
    blackKingSideCastling = castlingRights.find('k') != string::npos;
    blackQueenSideCastling = castlingRights.find('q') != string::npos;
    
    string enPassant = splitString[3];
    enPassantSquare = (enPassant == "-") ? -1 : coordinateToBitboardIndex(enPassant);

    halfmoveClock = stoi(splitString[4]);

    // The fullmoveNumber can be used as needed; it's not included here
    fullmoveNumber = stoi(splitString[5]);
}

// Move a piece from one square to another
void Board::movePiece(int pieceType, int fromSquare, int toSquare) {
    bitboards[pieceType] &= ~(1ULL << fromSquare); // Clear the original square
    bitboards[pieceType] |= (1ULL << toSquare);    // Set the destination square
}

void Board::removePiece(int pieceType, int square) {
    bitboards[pieceType] &= ~(1ULL << square);
}

void Board::addPiece(int pieceType, int square) {
    bitboards[pieceType] |= (1ULL << square);
}

void Board::printFENBoard() {
    const char pieceSymbols[] = "PNBRQKpnbrqk";

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            char pieceChar = '-'; // EmptySquare
            
            for (int pieceType = 11; pieceType >= 0; --pieceType) {
                if (bitboards[pieceType] & (1ULL << square)) {
                    pieceChar = pieceSymbols[pieceType];
                    break;
                }
            }
            std::cout << pieceChar << ' ';
        }
        std::cout << std::endl;
    }

    std:: cout << "White to move: " << (whiteToMove ? "true" : "false") << std::endl;
    std:: cout << "White King Side Castling: " << (whiteKingSideCastling ? "true" : "false") << std::endl;
    std:: cout << "White Queen Side Castling: " << (whiteQueenSideCastling ? "true" : "false") << std::endl;
    std:: cout << "Black King Side Castling: " << (blackKingSideCastling ? "true" : "false") << std::endl;
    std:: cout << "Black Queen Side Castling: " << (blackQueenSideCastling ? "true" : "false") << std::endl;
    std:: cout << "En Passant Square: " << enPassantSquare << std::endl;
    std:: cout << "Halfmove Clock: " << halfmoveClock << std::endl;
    std:: cout << "Fullmove Number: " << fullmoveNumber << std::endl;

}

void Board::printJustFENBoard() {
        const char pieceSymbols[] = "PNBRQKpnbrqk";

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            char pieceChar = '-'; // EmptySquare
            
            for (int pieceType = 11; pieceType >= 0; --pieceType) {
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

    for(int index=0; index<12; ++index) {
        
        Bitboard bb = bitboards[index];
        std::cout << pieces[index] << ":\n";
        for (int rank = 0; rank < 8; ++rank) {
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

void Board::printBitboard(uint64_t bitboard) const {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << ((bitboard & (1ULL << square)) ? "1 " : ". ");
        }
        std::cout << std::endl;
    }
}

int Board::coordinateToBitboardIndex(const std::string& coordinate) const {
    if (coordinate.size() != 2) {
        throw std::invalid_argument("Coordinate must be 2 characters long.");
    }

    // Extract the file and rank from the coordinate string
    char fileChar = coordinate[0];
    char rankChar = coordinate[1];

    // Convert the file ('a' to 'h') and rank ('8' to '1') into indices
    int file = fileChar - 'a';  // 'a' becomes 0, 'b' becomes 1, etc.
    int rank = '8' - rankChar;  // '8' becomes 0, '7' becomes 1, etc.

    // Ensure the file and rank are within valid ranges
    if (file < 0 || file >= 8 || rank < 0 || rank >= 8) {
        throw std::out_of_range("Invalid file or rank in coordinate.");
    }

    // Calculate the bitboard index (bit 0 = a8, bit 1 = b8, etc.)
    return rank * 8 + file;
}


int Board::pieceTypeAtSquare(int square) {
    for (int pieceType = 0; pieceType < 12; ++pieceType) {
        if (bitboards[pieceType] & (1ULL << square)) {
            return pieceType;
        }
    }
    return -1;
}

void Board::makeMove(const Move& move) {
    int fromSquare = move.getFromSquare();
    int toSquare = move.getToSquare();
    int pieceType = move.getPieceType();
    Move::MoveType moveType = move.getMoveType();

    switch(moveType) {
        case Move::MoveType::Normal:
            //std:: cout << "Normal move executing\n";
            movePiece(pieceType, fromSquare, toSquare);
            break;
            
        case Move::MoveType::MovedTwice:
            //std:: cout << "Moved Twice Executing\n";
            movePiece(pieceType, fromSquare, toSquare);
            break;
            
        case Move::MoveType::Capture:
            //std:: cout << " Capture Executing\n";
            removePiece(move.getCapturedPieceType(), toSquare);
            movePiece(pieceType, fromSquare, toSquare);
            break;
            
        case Move::MoveType::EnPassantCapture:
            //std:: cout << "En Passant Capture Executing\n";
            if(pieceType == 0) { // checks whether the piece is white pawn
                removePiece(6, move.getEnPassantSquare() + 8);
            } else {
                removePiece(0, move.getEnPassantSquare() - 8);
            }
            movePiece(pieceType, fromSquare, move.getEnPassantSquare());
            break;
            
        case Move::MoveType::Promote:
            //std:: cout << "Promote Executing\n";
            removePiece(pieceType, fromSquare);
            addPiece(move.getPromotedPieceType(), toSquare);
            break;
            
        case Move::MoveType::PromoteCapture:
            //std:: cout << "Promote Capture Executing\n";
            removePiece(pieceType, fromSquare);
            removePiece(move.getCapturedPieceType(), toSquare);
            addPiece(move.getPromotedPieceType(), toSquare);
            break;
            
        case Move::MoveType::CastleKingSide:
            //std:: cout << "Castle King Side Executing\n";
            if (pieceType == 5) { // checks whether the piece is white king
                movePiece(5, fromSquare, toSquare); // move the white king
                movePiece(3, 63, 61); // move the white rook
                whiteKingSideCastling = false;
                whiteQueenSideCastling = false;
            } else {
                movePiece(11, fromSquare, toSquare); // move the black king
                movePiece(9, 7, 5); // move the black rook
                blackKingSideCastling = false;
                blackQueenSideCastling = false;
            }
            break;
            
        case Move::MoveType::CastleQueenSide:
            //std:: cout << "Castle Queen Side Executing\n";
            if (pieceType == 5) { // checks whether the piece is white king
                movePiece(5, fromSquare, toSquare); // move the white king
                movePiece(3, 56, 59); // move the white rook
                whiteKingSideCastling = false;
                whiteQueenSideCastling = false;
            } else {
                movePiece(11, fromSquare, toSquare); // move the black king
                movePiece(9, 0, 3); // move the black rook
                blackKingSideCastling = false;
                blackQueenSideCastling = false;
            }
            break;
            
        default:
            throw std::invalid_argument("Unknown MoveType");

        // whiteToMove = !whiteToMove;
        // Full move clocks etc Todo

    }

}

void Board::undoMove(const Move& move) {

    int fromSquare = move.getFromSquare();
    int toSquare = move.getToSquare();
    int pieceType = move.getPieceType();
    Move::MoveType moveType = move.getMoveType();

    switch(moveType) {
        case Move::MoveType::Normal:
            movePiece(pieceType, toSquare, fromSquare);
            break;
            
        case Move::MoveType::MovedTwice:
            movePiece(pieceType, toSquare, fromSquare);
            break;
            
        case Move::MoveType::Capture:
            movePiece(pieceType, toSquare, fromSquare);
            addPiece(move.getCapturedPieceType(), toSquare);
            break;
            
        case Move::MoveType::EnPassantCapture:
            movePiece(pieceType, toSquare, fromSquare);
            if(pieceType == 0) { // checks whether the piece is white pawn
                addPiece(6, move.getEnPassantSquare() + 8);
            } else {
                addPiece(0, move.getEnPassantSquare() - 8);
            }
            break;
            
        case Move::MoveType::Promote:
            removePiece(move.getPromotedPieceType(), toSquare);
            addPiece(pieceType, fromSquare);
            break;
            

        case Move::MoveType::PromoteCapture:
            addPiece(pieceType, fromSquare);
            removePiece(move.getPromotedPieceType(), toSquare);
            addPiece(move.getCapturedPieceType(), toSquare);
            break;
            
        case Move::MoveType::CastleKingSide:
            if (pieceType == 5) { // checks whether the piece is white king
                movePiece(5, toSquare, fromSquare); // move the white king
                movePiece(3, 61, 63); // move the white rook
            } else {
                movePiece(11, toSquare, fromSquare); // move the black king
                movePiece(9, 5, 7); // move the black rook
            }
            break;
            
        case Move::MoveType::CastleQueenSide:
            if (pieceType == 5) { // checks whether the piece is white king
                movePiece(5, toSquare, fromSquare); // move the white king
                movePiece(3, 59, 56); // move the white rook
            } else {
                movePiece(11, toSquare, fromSquare); // move the black king
                movePiece(9, 3, 0); // move the black rook
            }
            break;
            
        default:
            throw std::invalid_argument("Unknown MoveType");
    }
        // whiteToMove = !whiteToMove;
        // Full move clocks etc Todo
        whiteKingSideCastling = move.getWhiteKingSideCastling();
        whiteQueenSideCastling = move.getWhiteQueenSideCastling();
        blackKingSideCastling = move.getBlackKingSideCastling();
        blackQueenSideCastling = move.getBlackQueenSideCastling();
        enPassantSquare = move.getEnPassantSquare();
        //halfmoveClock = move.getHalfmoveClock();
        //fullmoveNumber = move.getFullmoveNumber();

}

void Board::flipColour() {
    whiteToMove = !whiteToMove;
}

int Board::isGameOver() {
    if(legalMoveGeneration().empty()) {
        if(isKingInCheck()) {
            return whiteToMove ? -1 : 1;
        } else {
            return 0;
        }
    }
    return 2; // Game is not over
}

bool Board::isKingInCheck() {
    int kingSquare;
    uint64_t enemyPawns, enemyKnights, enemyBishopsQueens, enemyRooksQueens, enemyKing;

    // Find the king's square and set up the enemy pieces' bitboards
    if (!whiteToMove) {
        kingSquare = __builtin_ctzll(bitboards[11]); // Black king
        enemyPawns = bitboards[0];                   // White pawns
        enemyKnights = bitboards[1];                 // White knights
        enemyBishopsQueens = bitboards[2] | bitboards[4];   // White bishops and queens
        enemyRooksQueens = bitboards[3] | bitboards[4];     // White rooks and queens
        enemyKing = bitboards[5];                    // White king
    } else {
        kingSquare = __builtin_ctzll(bitboards[5]);  // White king
        enemyPawns = bitboards[6];                   // Black pawns
        enemyKnights = bitboards[7];                 // Black knights
        enemyBishopsQueens = bitboards[8] | bitboards[10];  // Black bishops and queens
        enemyRooksQueens = bitboards[9] | bitboards[10];    // Black rooks and queens
        enemyKing = bitboards[11];                   // Black king
    }

    // Calculate the blocker bitboard (all pieces)
    uint64_t blockers = 0;
    for (int i = 0; i < 12; ++i) {
        blockers |= bitboards[i];
    }

    // Print debug information
    // std::cout << "King Square: " << kingSquare << std::endl;
    // std::cout << "Blockers:" << std::endl;
    // printBitboard(blockers);
    // std::cout << "Enemy Pawns:" << std::endl;
    // printBitboard(enemyPawns);
    // std::cout << "Enemy Knights:" << std::endl;
    // printBitboard(enemyKnights);
    // std::cout << "Enemy Bishops/Queens:" << std::endl;
    // printBitboard(enemyBishopsQueens);
    // std::cout << "Enemy Rooks/Queens:" << std::endl;
    // printBitboard(enemyRooksQueens);
    // std::cout << "Enemy King:" << std::endl;
    // printBitboard(enemyKing);

        //uint64_t currentPawnCaptureMoves = isWhiteTurn ? 
        //    (((1ULL << (fromSquare - 9)) & opponentPieces & ~FILE_A) | ((1ULL << (fromSquare - 7)) & opponentPieces & ~FILE_H)) :
        //    (((1ULL << (fromSquare + 9)) & opponentPieces & ~FILE_H) | ((1ULL << (fromSquare + 7)) & opponentPieces & ~FILE_A));




    // Check for pawn attacks on the king
    uint64_t pawnAttacks = whiteToMove ? (((enemyPawns << 9) & ~FILE_A) | ((enemyPawns << 7) & ~FILE_H))
                                    : (((enemyPawns >> 9) & ~FILE_A) | ((enemyPawns >> 7) & ~FILE_H));
    if (pawnAttacks & (1ULL << kingSquare)) {
        //std::cout << "King is in check by pawn attack" << std::endl;
        return true;
    }

    // Check for knight attacks on the king
    uint64_t knightAttacks = generateKnightAttacks(kingSquare);
    if (enemyKnights & knightAttacks) {
        //std::cout << "King is in check by knight attack" << std::endl;
        return true;
    }

    // Check for diagonal attacks (bishops and queens) using blockers
    if (generateBishopAttacks(kingSquare, blockers) & enemyBishopsQueens) {
        //std::cout << "King is in check by bishop/queen diagonal attack" << std::endl;
        return true;
    }

    // Check for horizontal and vertical attacks (rooks and queens) using blockers
    if (generateRookAttacks(kingSquare, blockers) & enemyRooksQueens) {
        //std::cout << "King is in check by rook/queen horizontal/vertical attack" << std::endl;
        return true;
    }

    // Check for king attacks (adjacent squares)
    uint64_t kingAttacks = generateKingAttacks(kingSquare);
    if (enemyKing & kingAttacks) {
        //std::cout << "King is in check by enemy king" << std::endl;
        return true;
    }

    return false;  // No attack found on the king
}




uint64_t Board::generateKnightAttacks(int square) const {
    uint64_t knight = (1ULL << square);  // Start with the knight's current position

    // Masks to prevent knight moves from overflowing to the other side
    static const uint64_t notAFile = 0xFEFEFEFEFEFEFEFEULL;  // Mask for not A-file (file 1)
    static const uint64_t notABFile = 0xFCFCFCFCFCFCFCFCULL; // Mask for not A-file and B-file
    static const uint64_t notHFile = 0x7F7F7F7F7F7F7F7FULL;  // Mask for not H-file (file 8)
    static const uint64_t notGHFile = 0x3F3F3F3F3F3F3F3FULL; // Mask for not G-file and H-file

    // Generate knight moves by shifting the bitboard
    uint64_t attacks = 0;

    // Knight moves toward the top-right (northeast)
    attacks |= (knight << 17) & notAFile;    // Up 2, right 1
    attacks |= (knight << 10) & notABFile;   // Up 1, right 2

    // Knight moves toward the top-left (northwest)
    attacks |= (knight << 15) & notHFile;    // Up 2, left 1
    attacks |= (knight << 6) & notGHFile;    // Up 1, left 2

    // Knight moves toward the bottom-right (southeast)
    attacks |= (knight >> 15) & notAFile;    // Down 2, right 1
    attacks |= (knight >> 6) & notABFile;    // Down 1, right 2

    // Knight moves toward the bottom-left (southwest)
    attacks |= (knight >> 17) & notHFile;    // Down 2, left 1
    attacks |= (knight >> 10) & notGHFile;   // Down 1, left 2

    return attacks;
}

uint64_t Board::generateBishopAttacks(int square, uint64_t blockers) const {
    uint64_t attacks = 0;

    // North-East diagonal (down-right, since the board is flipped)
    for (int shift = square - 9; shift >= 0 && (shift % 8 != 7); shift -= 9) {
        attacks |= (1ULL << shift);
        if (blockers & (1ULL << shift)) break;  // Stop if blocked
    }

    // North-West diagonal (down-left)
    for (int shift = square - 7; shift >= 0 && (shift % 8 != 0); shift -= 7) {
        attacks |= (1ULL << shift);
        if (blockers & (1ULL << shift)) break;  // Stop if blocked
    }

    // South-East diagonal (up-right)
    for (int shift = square + 7; shift < 64 && (shift % 8 != 7); shift += 7) {
        attacks |= (1ULL << shift);
        if (blockers & (1ULL << shift)) break;  // Stop if blocked
    }

    // South-West diagonal (up-left)
    for (int shift = square + 9; shift < 64 && (shift % 8 != 0); shift += 9) {
        attacks |= (1ULL << shift);
        if (blockers & (1ULL << shift)) break;  // Stop if blocked
    }

    return attacks;
}

uint64_t Board::generateRookAttacks(int square, uint64_t blockers) const {
    uint64_t attacks = 0;

    // Generate attacks to the right (East direction)
    for (int shift = square - 1; shift % 8 != 7 && shift >= 0; shift--) {
        attacks |= (1ULL << shift);
        if (blockers & (1ULL << shift)) break;  // Stop if blocked
    }

    // Generate attacks to the left (West direction)
    for (int shift = square + 1; shift % 8 != 0 && shift < 64; shift++) {
        attacks |= (1ULL << shift);
        if (blockers & (1ULL << shift)) break;  // Stop if blocked
    }

    // Generate attacks upwards (South direction)
    for (int shift = square - 8; shift >= 0; shift -= 8) {
        attacks |= (1ULL << shift);
        if (blockers & (1ULL << shift)) break;  // Stop if blocked
    }

    // Generate attacks downwards (North direction)
    for (int shift = square + 8; shift < 64; shift += 8) {
        attacks |= (1ULL << shift);
        if (blockers & (1ULL << shift)) break;  // Stop if blocked
    }

    return attacks;
}

uint64_t Board::generateKingAttacks(int square) const {
    uint64_t attacks = 0;
    uint64_t bitboard = 1ULL << square;

    // King's movement directions: one square in any direction
    if (square % 8 != 7) attacks |= bitboard << 1;        // East (right)
    if (square % 8 != 0) attacks |= bitboard >> 1;        // West (left)
    if (square / 8 != 7) attacks |= bitboard << 8;        // North (up)
    if (square / 8 != 0) attacks |= bitboard >> 8;        // South (down)

    if (square % 8 != 7 && square / 8 != 7) attacks |= bitboard << 9;  // Northeast (up-right)
    if (square % 8 != 0 && square / 8 != 7) attacks |= bitboard << 7;  // Northwest (up-left)
    if (square % 8 != 7 && square / 8 != 0) attacks |= bitboard >> 7;  // Southeast (down-right)
    if (square % 8 != 0 && square / 8 != 0) attacks |= bitboard >> 9;  // Southwest (down-left)

    return attacks;
}

// These are used only to generate x ray bitboards

uint64_t Board::generateBishopXRay(int square, int kingSquare, uint64_t blockers) const {
    uint64_t attacks = 0;
    bool xRay = false;
    int kingRank = kingSquare / 8;
    int kingFile = kingSquare % 8;
    int pieceRank = square / 8;
    int pieceFile = square % 8;

    // North-East diagonal (up-right)
    if (kingRank > pieceRank && kingFile > pieceFile) {
        for (int shift = square + 9; shift < 64 && (shift % 8 != 0); shift += 9) {
            attacks |= (1ULL << shift);
            if (blockers & (1ULL << shift)) {
                if (xRay || shift == kingSquare) break;
                xRay = true;
            }
        }
    }

    // North-West diagonal (up-left)
    if (kingRank > pieceRank && kingFile < pieceFile) {
        for (int shift = square + 7; shift < 64 && (shift % 8 != 7); shift += 7) {
            attacks |= (1ULL << shift);
            if (blockers & (1ULL << shift)) {
                if (xRay || shift == kingSquare) break;
                xRay = true;
            }
        }
    }

    // South-East diagonal (down-right)
    if (kingRank < pieceRank && kingFile > pieceFile) {
        for (int shift = square - 7; shift >= 0 && (shift % 8 != 0); shift -= 7) {
            attacks |= (1ULL << shift);
            if (blockers & (1ULL << shift)) {
                if (xRay || shift == kingSquare) break;
                xRay = true;
            }
        }
    }

    // South-West diagonal (down-left)
    if (kingRank < pieceRank && kingFile < pieceFile) {
        for (int shift = square - 9; shift >= 0 && (shift % 8 != 7); shift -= 9) {
            attacks |= (1ULL << shift);
            if (blockers & (1ULL << shift)) {
                if (xRay || shift == kingSquare) break;
                xRay = true;
            }
        }
    }

    // Only return attacks if the king was encountered on the x-ray path
    return (attacks & (1ULL << kingSquare)) ? attacks : 0;
}






uint64_t Board::generateRookXRay(int square, int kingSquare, uint64_t blockers) const {
    uint64_t attacks = 0;
    bool xRay = false;
    int kingRank = kingSquare / 8;
    int kingFile = kingSquare % 8;
    int pieceRank = square / 8;
    int pieceFile = square % 8;

    // East (right) direction
    if (kingRank == pieceRank && kingFile > pieceFile) {
        for (int shift = square + 1; shift % 8 != 0 && shift < 64; shift++) {
            attacks |= (1ULL << shift);
            if (blockers & (1ULL << shift)) {
                if (xRay || shift == kingSquare) break;
                xRay = true;
            }
        }
    }

    // West (left) direction
    if (kingRank == pieceRank && kingFile < pieceFile) {
        for (int shift = square - 1; shift % 8 != 7 && shift >= 0; shift--) {
            attacks |= (1ULL << shift);
            if (blockers & (1ULL << shift)) {
                if (xRay || shift == kingSquare) break;
                xRay = true;
            }
        }
    }

    // North (up) direction
    if (kingFile == pieceFile && kingRank < pieceRank) {
        for (int shift = square - 8; shift >= 0; shift -= 8) {
            attacks |= (1ULL << shift);
            if (blockers & (1ULL << shift)) {
                if (xRay || shift == kingSquare) break;
                xRay = true;
            }
        }
    }

    // South (down) direction
    if (kingFile == pieceFile && kingRank > pieceRank) {
        for (int shift = square + 8; shift < 64; shift += 8) {
            attacks |= (1ULL << shift);
            if (blockers & (1ULL << shift)) {
                if (xRay || shift == kingSquare) break;
                xRay = true;
            }
        }
    }

    // Only return attacks if the king was encountered on the x-ray path
    return (attacks & (1ULL << kingSquare)) ? attacks : 0;
}



// Precomputing Table getters
uint64_t Board::getKnightAttacks(int square) {
    return knightAttacks[square];
}



std::vector<Move> Board::legalMoveGeneration() {
    
    vector<Move> legalMoves;

    // https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/
    // ******************************************************************************
    // 1) - check the square the opponent is attacking to check single check / double check
    // remove your own king during opponent attack generation because of sliding pieces can see through
    // ******************************************************************************

    uint64_t kingDangerSquares = 0;
    uint64_t pawns, knights, bishopQueens, rookQueens, king;
    uint64_t enemyPawns, enemyKnights, enemyBishopsQueens, enemyRooksQueens, enemyKing;
    int kingIndex;
    int kingSquare;
    int kingAttacks = 0; // for double check or single check
    int kingAttackedAtSquare = -1; // which square is checking the king
    int checkingPieceType = -1; // which piece type is checking the king
    int playerPieceType = whiteToMove ? 0 : 6;  // White pieces start at 0, Black pieces start at 6
    int opponentPieceType = whiteToMove ? 6 : 0;


    if (!whiteToMove) {
        enemyPawns = bitboards[0];                   // White pawns
        enemyKnights = bitboards[1];                 // White knights
        enemyBishopsQueens = bitboards[2] | bitboards[4];   // White bishops and queens
        enemyRooksQueens = bitboards[3] | bitboards[4];     // White rooks and queens
        enemyKing = bitboards[5];                    // White king
        pawns = bitboards[6];                        // Black pawns
        knights = bitboards[7];                      // Black knights
        bishopQueens = bitboards[8] | bitboards[10];
        rookQueens = bitboards[9] | bitboards[10];
        king = bitboards[11];
        kingIndex = 11;
        kingSquare = __builtin_ctzll(bitboards[11]);
    } else {
        enemyPawns = bitboards[6];                   // Black pawns
        enemyKnights = bitboards[7];                 // Black knights
        enemyBishopsQueens = bitboards[8] | bitboards[10];  // Black bishops and queens
        enemyRooksQueens = bitboards[9] | bitboards[10];    // Black rooks and queens
        enemyKing = bitboards[11];                   // Black king
        pawns = bitboards[0];                        // White pawns
        knights = bitboards[1];                      // White knights
        bishopQueens = bitboards[2] | bitboards[4];
        rookQueens = bitboards[3] | bitboards[4];
        king = bitboards[5];
        kingIndex = 5;
        kingSquare = __builtin_ctzll(bitboards[5]);
    }

    // Calculate the blocker bitboard except for current king
    uint64_t blockers = 0; 
    for (int i = 0; i < 12; ++i) {
        if(i == kingIndex) continue;
        blockers |= bitboards[i];
    }


    if (!whiteToMove) {
        // Checking pawn attacks
        if ((enemyPawns >> 9) & (1ULL << kingSquare) & ~FILE_A) {kingAttacks++; kingAttackedAtSquare = kingSquare + 9; checkingPieceType = 0;}
        if ((enemyPawns >> 7) & (1ULL << kingSquare) & ~FILE_H) {kingAttacks++; kingAttackedAtSquare = kingSquare + 7; checkingPieceType = 0;}

        kingDangerSquares |= ((enemyPawns >> 9) & ~FILE_A);  // Pawns attacking from the left
        kingDangerSquares |= ((enemyPawns >> 7) & ~FILE_H);  // Pawns attacking from the right

    } else {
        if ((enemyPawns << 9) & (1ULL << kingSquare) & ~FILE_H) {kingAttacks++; kingAttackedAtSquare = kingSquare - 9; checkingPieceType = 6;}
        if ((enemyPawns << 7) & (1ULL << kingSquare) & ~FILE_A) {kingAttacks++; kingAttackedAtSquare = kingSquare - 7; checkingPieceType = 6;}
        kingDangerSquares |= ((enemyPawns << 9) & ~FILE_H);
        kingDangerSquares |= ((enemyPawns << 7) & ~FILE_A);
    }

    // Checking knight attacks
    uint64_t knightAttacks = 0;
    uint64_t enemyKnightsCopy = enemyKnights;
    while (enemyKnightsCopy) {
        int square = __builtin_ctzll(enemyKnightsCopy);
        knightAttacks |= generateKnightAttacks(square);
        kingDangerSquares |= generateKnightAttacks(square);

        if(knightAttacks & (1ULL << kingSquare)) {
            kingAttacks++; 
            kingAttackedAtSquare = square;
            checkingPieceType = whiteToMove? 7 : 1;
        }

        enemyKnightsCopy &= enemyKnightsCopy - 1;
    }

    // Checking bishop/queen (diagonal) attacks
    uint64_t bishopAttacks = 0;
    uint64_t enemyBishopsQueensCopy = enemyBishopsQueens;
    while (enemyBishopsQueensCopy) {
        int square = __builtin_ctzll(enemyBishopsQueensCopy);
        bishopAttacks |= generateBishopAttacks(square, blockers);
        if (bishopAttacks & (1ULL << kingSquare)) {
            kingAttacks++;
            kingAttackedAtSquare = square;
            checkingPieceType = whiteToMove? 8 : 2;
        }
        kingDangerSquares |= generateBishopAttacks(square, blockers);
        enemyBishopsQueensCopy &= enemyBishopsQueensCopy - 1;
    }


    // Checking rook/queen (horizontal/vertical) attacks
    uint64_t rookAttacks = 0;
    uint64_t enemyRooksQueensCopy = enemyRooksQueens;
    while (enemyRooksQueensCopy) {
        int square = __builtin_ctzll(enemyRooksQueensCopy);
        rookAttacks |= generateRookAttacks(square, blockers);
        if (rookAttacks & (1ULL << kingSquare)) {
            kingAttacks++;
            kingAttackedAtSquare = square;
            checkingPieceType = whiteToMove? 9 : 3;
        }
        kingDangerSquares |= generateRookAttacks(square, blockers);
        enemyRooksQueensCopy &= enemyRooksQueensCopy - 1;
    }


    // Checking king attacks
    kingDangerSquares |= generateKingAttacks(__builtin_ctzll(enemyKing));

    //Check if there's a double check
    // if (kingAttacks >= 2) {
    //     std::cout << "Double check!\n";
    // } else if (kingAttacks == 1) {
    //     std::cout << "Single check!\n";
    // } else {
    //     std::cout << "King is not in check.\n";
    // }
    // std:: cout << "King Danger Squares\n";
    // std::bitset<64> dangerSquares(kingDangerSquares);
    // for (int i = 0; i < 64; i++) {
    //     std::cout << dangerSquares[i];
    //     if ((i + 1) % 8 == 0) {
    //         std::cout << std::endl;
    //     }
    // }
    // std::cout << "King Attacked At Square: " << kingAttackedAtSquare << std::endl;
    // std:: cout << "Piece type attacking :" << checkingPieceType << std::endl;
    // ******************************************************************************
    // 2)   -> if in double check, you can only evade
    //      -> if in single check, then evade, captures or block the check
    //      -> en passant checks
    // ******************************************************************************

    if(kingAttacks > 0) {
        // Evade the double check by only moving the king
        // Also make sure that any captures are legal and on enemy pieces
        uint64_t kingMoves = generateKingAttacks(kingSquare) & ~kingDangerSquares;
        while (kingMoves) {
            int toSquare = __builtin_ctzll(kingMoves);
            if(pieceTypeAtSquare(toSquare) == -1) {
                legalMoves.push_back(Move(kingSquare, toSquare, kingIndex, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                kingMoves &= kingMoves - 1;
                continue;
            } 
            if(whiteToMove) {
                // check that the piece is an enemy piece
                if(pieceTypeAtSquare(toSquare) >= 6) {
                    legalMoves.push_back(Move(kingSquare, toSquare, kingIndex, pieceTypeAtSquare(toSquare), -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }
            } else {
                if(pieceTypeAtSquare(toSquare) < 6) {
                    legalMoves.push_back(Move(kingSquare, toSquare, kingIndex, pieceTypeAtSquare(toSquare), -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }
            }   
            kingMoves &= kingMoves - 1;
        }

    }

    if(kingAttacks == 2) return legalMoves; // No other moves can be made if in double check

    if(kingAttacks == 1) {
        // ******************************************************************************
        // Capture the attacking piece with other pieces
        // Find all captures that can be made at square kingAttackedAtSquare
        // ******************************************************************************

        // Pawns
        uint64_t leftPawnCaptures = (whiteToMove) ? ((pawns >> 9) & ~FILE_A) : ((pawns << 7) & ~FILE_H);
        uint64_t rightPawnCaptures = (whiteToMove) ? ((pawns >> 7) & ~FILE_H) : ((pawns << 9) & ~FILE_A);
        int pieceType = whiteToMove? 0 : 6;
        bool isPromotingRank = (whiteToMove && kingAttackedAtSquare / 8 == 0) || (!whiteToMove && kingAttackedAtSquare / 8 == 7);

        if (leftPawnCaptures & (1ULL << kingAttackedAtSquare)) {

            int fromSquare = whiteToMove ? kingAttackedAtSquare + 9 : kingAttackedAtSquare - 7;

            if(isPromotingRank) {
                int knightIndex;
                if(kingIndex == 5) {
                    knightIndex = 1;
                } else {
                    knightIndex = 7;
                }
                //std::cout << "LeftPawn promotion capture at  " << fromSquare << " to " << kingAttackedAtSquare << std::endl;
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, knightIndex, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, knightIndex+1, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, knightIndex+2, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, knightIndex+3, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            } else {               
                //std::cout << "Left Pawn capture at  " << fromSquare << " to " << kingAttackedAtSquare << std::endl;
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }


        if (rightPawnCaptures & (1ULL << kingAttackedAtSquare)) {

            int fromSquare = whiteToMove ? kingAttackedAtSquare + 7 : kingAttackedAtSquare - 9;

            if(isPromotingRank) {
                int knightIndex;
                if(kingIndex == 5) {
                    knightIndex = 1;
                } else {
                    knightIndex = 7;
                }
                //std::cout << "Right Pawn promotion capture at  " << fromSquare << " to " << kingAttackedAtSquare << std::endl;
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, knightIndex, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, knightIndex+1, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, knightIndex+2, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, knightIndex+3, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            } else {               
                //std::cout << "Right Pawn capture at  " << fromSquare << " to " << kingAttackedAtSquare << std::endl;
                legalMoves.push_back(Move(fromSquare, kingAttackedAtSquare, pieceType, checkingPieceType, -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }

        // Knights
        uint64_t knightCopy = knights; // create the copy so we can reuse it later
        while (knightCopy) {
            int square = __builtin_ctzll(knightCopy);
            uint64_t knightMoves = generateKnightAttacks(square);
            if (knightMoves & (1ULL << kingAttackedAtSquare)) {
                //std::cout << "Knight capture at  " << square << " to " << kingAttackedAtSquare << std::endl;
                pieceType = whiteToMove ? 1 : 7;
                legalMoves.push_back(Move(square, kingAttackedAtSquare, pieceType, checkingPieceType, -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            knightCopy &= knightCopy - 1;
        }

        // Bishops/Queens
        uint64_t bishopQueensCopy = bishopQueens; // create the copy so we can reuse it later
        while (bishopQueensCopy) {
            int square = __builtin_ctzll(bishopQueensCopy);
            uint64_t bishopMoves = generateBishopAttacks(square, blockers);
            if (bishopMoves & (1ULL << kingAttackedAtSquare)) {
                //std::cout << "Bishop capture at  " << square << " to " << kingAttackedAtSquare << std::endl;
                pieceType = pieceTypeAtSquare(square);
                legalMoves.push_back(Move(square, kingAttackedAtSquare, pieceType, checkingPieceType, -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            bishopQueensCopy &= bishopQueensCopy - 1;
        }


        // Rooks/Queens
        uint64_t rookQueensCopy = rookQueens; // create the copy so we can reuse it later
        while (rookQueensCopy) {
            int square = __builtin_ctzll(rookQueensCopy);
            uint64_t rookMoves = generateRookAttacks(square, blockers);
            if (rookMoves & (1ULL << kingAttackedAtSquare)) {
                //std::cout << "Rook capture at  " << square << " to " << kingAttackedAtSquare << std::endl;
                pieceType = pieceTypeAtSquare(square);
                legalMoves.push_back(Move(square, kingAttackedAtSquare, pieceType, checkingPieceType, -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            rookQueensCopy &= rookQueensCopy - 1;
        }
        

        // ******************************************************************************
        // Block the sliding piece
        // find the bitboard of the sliding piece that is attacking our king so that we can find moves that block it from kingAttackedAtSquare
        // ******************************************************************************
        if(checkingPieceType == 0 || checkingPieceType == 6 || checkingPieceType == 1 || checkingPieceType == 7) {
            return legalMoves; // You can't block a pawn or a knight attacks
        }

        uint64_t blockingMask = 0ULL;
        int kingRank = kingSquare / 8;
        int kingFile = kingSquare % 8;
        int pieceRank = kingAttackedAtSquare / 8;
        int pieceFile = kingAttackedAtSquare % 8;

        // Directional deltas for rooks and bishops (used for queens as well)
        int rankDelta = pieceRank - kingRank;
        int fileDelta = pieceFile - kingFile;


        // finding the bitboard of the attacks of te sliding piece that is attacking our king        

        // If the checking piece is a rook or queen and the attack is on the same rank (horizontal attack)
        if((checkingPieceType == 3 || checkingPieceType == 4 || checkingPieceType == 9 || checkingPieceType == 10) && kingRank == pieceRank) {
            int direction = (fileDelta > 0) ? 1 : -1;  // Right or left
            for (int file = kingFile + direction; file != pieceFile; file += direction) {
                blockingMask |= (1ULL << (kingRank * 8 + file));  // Set bit for each square between king and piece
            }
        } 

        // If the checking piece is a rook or queen and the attack is on the same file (vertical attack)
        if((checkingPieceType == 3 || checkingPieceType == 4 || checkingPieceType == 9 || checkingPieceType == 10) && kingFile == pieceFile) {
            int direction = (rankDelta > 0) ? 8 : -8;  // Up or down
            for (int rank = kingRank + (direction / 8); rank != pieceRank; rank += (direction / 8)) {
                blockingMask |= (1ULL << (rank * 8 + kingFile));  // Set bit for each square between king and piece
            }
        }
            // If the checking piece is a bishop or queen and the attack is along a diagonal
        if(checkingPieceType == 2 || checkingPieceType == 4 || checkingPieceType == 8 || checkingPieceType == 10) {
            int direction = ((rankDelta > 0) ? 8 : -8) + ((fileDelta > 0) ? 1 : -1);  // Diagonal direction
            for (int square = kingSquare + direction; square != kingAttackedAtSquare; square += direction) {
                blockingMask |= (1ULL << square);  // Set bit for each square between king and piece
            }
        }


        // std:: cout << "Blocking Mask\n";
        // std::bitset<64> printing(blockingMask);
        // for (int i = 0; i < 64; i++) {
        //     std::cout << printing[i];
        //     if ((i + 1) % 8 == 0) {
        //         std::cout << std::endl;
        //     }
        // }


    // Check pawn normal move / double move
    uint64_t blockingMaskCopy = blockingMask;
    uint64_t pawnsCopy = pawns;

    // For white pawns
    if (whiteToMove) {
        while (blockingMaskCopy) {
            int square = __builtin_ctzll(blockingMaskCopy);  // Get the least significant bit set
            // Check if a white pawn can move 1 square forward
            if ((square >= 8) && ((1ULL << (square + 8)) & pawnsCopy)) {
                // Ensure the square is within bounds and the destination is occupied by a pawn
                //std::cout << "White pawn can move from " << (square + 8) << " to " << square << std::endl;
                legalMoves.push_back(Move(square + 8, square, 0, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }

            // Check if a white pawn can move 2 squares forward from the starting rank
            if ((square >= 16) && (square / 8 == 3) && ((1ULL << (square + 16)) & pawnsCopy)) {
                // Ensure the square is within bounds and the destination is occupied by a pawn
                //std::cout << "White pawn can move from " << (square + 16) << " to " << square << std::endl;
                legalMoves.push_back(Move(square + 16, square, 0, -1, -1, -1, Move::MoveType::MovedTwice, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }

            // Clear the least significant bit
            blockingMaskCopy &= blockingMaskCopy - 1;
        }
    }
        // For black pawns
        else {
            while (blockingMaskCopy) {
                int square = __builtin_ctzll(blockingMaskCopy);  // Get the least significant bit set

                // Check if a black pawn can move 1 square forward
                if ((square < 56) && ((1ULL << (square - 8)) & pawnsCopy)) {
                    // Ensure the square is within bounds and the destination is occupied by a pawn
                    //std::cout << "Black pawn can move from " << (square - 8) << " to " << square << std::endl;
                    legalMoves.push_back(Move(square - 8, square, 6, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }

                // Check if a black pawn can move 2 squares forward from the starting rank
                if ((square < 48) && (square / 8 == 4) && ((1ULL << (square - 16)) & pawnsCopy)) {
                    // Ensure the square is within bounds and the destination is occupied by a pawn
                    //std::cout << "Black pawn can move from " << (square - 16) << " to " << square << std::endl;
                    legalMoves.push_back(Move(square - 16, square, 6, -1, -1, -1, Move::MoveType::MovedTwice, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }

                // Clear the least significant bit
                blockingMaskCopy &= blockingMaskCopy - 1;
            }
        }

        // check knight moves
        knightCopy = knights;
        while(knightCopy) {
            int square = __builtin_ctzll(knightCopy);
            uint64_t knightMoves = generateKnightAttacks(square);
            if(knightMoves & blockingMask) {
                //std::cout << "Knight can move from " << square << " to " << __builtin_ctzll(blockingMask&knightMoves)  << std::endl;
                legalMoves.push_back(Move(square, __builtin_ctzll(blockingMask&knightMoves), playerPieceType + 1, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            knightCopy &= knightCopy - 1;
        }

        // check bishop queen moves
        bishopQueensCopy = bishopQueens;
        while(bishopQueensCopy) {
            int square = __builtin_ctzll(bishopQueensCopy);
            uint64_t bishopMoves = generateBishopAttacks(square, blockers);
            if(bishopMoves & blockingMask) {
                //std::cout << "Bishop can move from " << square << " to " << __builtin_ctzll(blockingMask&bishopMoves) << std::endl;
                legalMoves.push_back(Move(square, __builtin_ctzll(blockingMask&bishopMoves), playerPieceType + 2, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            bishopQueensCopy &= bishopQueensCopy - 1;
        }

        // check rook queen moves
        rookQueensCopy = rookQueens;
        while(rookQueensCopy) {
            int square = __builtin_ctzll(rookQueensCopy);
            uint64_t rookMoves = generateRookAttacks(square, blockers);
            if(rookMoves & blockingMask) {
                //std::cout << "Rook can move from " << square << " to " << __builtin_ctzll(blockingMask&rookMoves) << std::endl;
                legalMoves.push_back(Move(square, __builtin_ctzll(blockingMask&rookMoves), playerPieceType + 3, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            rookQueensCopy &= rookQueensCopy - 1;
        }

        return legalMoves;
    }

    // ******************************************************************************
    // At this stage our king is not in check, so we can generate all the legal moves but we have to be careful of
    // pinned pieces, we will generate ALL pseudo legal moves but take into account the xray attacks for pinned pieces.
    // 3) -> pinned pieces
    /*
        1 -Moves from the opponent’s sliding pieces.
        2 -“Sliding attacking” moves from my king in the opposite direction.
        3 -The overlap of these two rays.
        If the result of 3) lands on one of my pieces, then it is pinned.

        Once all the pinned pieces are identified, you can remove them from the board and calculate the moves from the enemy’s pinning piece to your king. This will give you a “ray” of legal moves for each of your pinned pieces.

        Note that since knights can only move in an L-shape, they can never move along a pin ray. The only piece types you need to consider are:

        On a diagonal ray: bishops, queens, and pawns (captures only).
        On a non-diagonal ray: rooks, queens, and pawns (pushes only).
    */
   // ******************************************************************************
   uint64_t xrayBitboard = 0ULL; // get attack rays from all sliding pieces - will ignore one friendly piece
    blockers |= king; // add the king to the blockers so that we can get the xray bitboard  
    // Checking bishop/queen (diagonal) attacks
    enemyBishopsQueensCopy = enemyBishopsQueens;
    while (enemyBishopsQueensCopy) {
        int square = __builtin_ctzll(enemyBishopsQueensCopy);
        xrayBitboard |= generateBishopXRay(square, kingSquare, blockers);
        enemyBishopsQueensCopy &= enemyBishopsQueensCopy - 1;
    }

    // Checking rook/queen (horizontal/vertical) attacks
    rookAttacks = 0;
    enemyRooksQueensCopy = enemyRooksQueens;
    while (enemyRooksQueensCopy) {
        int square = __builtin_ctzll(enemyRooksQueensCopy);
        xrayBitboard |= generateRookXRay(square, kingSquare, blockers);
        enemyRooksQueensCopy &= enemyRooksQueensCopy - 1;
    }


    // std:: cout << "xrayBitboard\n";
    // std::bitset<64> xray(xrayBitboard);
    // for (int i = 0; i < 64; i++) {
    //     std::cout << xray[i];
    //     if ((i + 1) % 8 == 0) {
    //         std::cout << std::endl;
    //     }
    // }

    // ******************************************************************************
    // now we can generate all pseudo legal moves and take into account the !(fromSquare && xrayBitBoard)
    // ******************************************************************************

    uint64_t opponentPieces = enemyPawns | enemyKnights | enemyBishopsQueens | enemyRooksQueens | enemyKing;


    //     std:: cout << "ORIGINAL opponent pieces\n";
    //     std::bitset<64> sq3(opponentPieces);
    //     for (int i = 0; i < 64; i++) {
    //         std::cout << sq3[i];
    //         if ((i + 1) % 8 == 0) {
    //             std::cout << std::endl;
    //         }
    //     }

    uint64_t rooks = bitboards[playerPieceType + 3];
    uint64_t bishops = bitboards[playerPieceType + 2];
    uint64_t queens = bitboards[playerPieceType + 4];

        // Generate pawn moves
    uint64_t playerPawnsMask = pawns;
    while (playerPawnsMask) {
        int fromSquare = __builtin_ctzll(playerPawnsMask);
        if(((xrayBitboard & (1ULL << fromSquare)))) {
            playerPawnsMask &= playerPawnsMask - 1;
            continue; // if the pawn is pinned, we can't move it
        }
        bool isPromotingRank = (whiteToMove && fromSquare / 8 == 0) || (!whiteToMove && fromSquare / 8 == 7);
        // Single pawn move (White moves down, Black moves up)
        uint64_t singleMove = whiteToMove ? (1ULL << (fromSquare - 8)) : (1ULL << (fromSquare + 8));
        if (!(blockers & singleMove) ) {
            legalMoves.push_back(Move(fromSquare, __builtin_ctzll(singleMove), playerPieceType, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
        }

        // Double pawn move (White from rank 6, Black from rank 1)
        if ((whiteToMove && (fromSquare / 8 == 6)) || (!whiteToMove && (fromSquare / 8 == 1))) {
            uint64_t doubleMove = whiteToMove ? (1ULL << (fromSquare - 16)) : (1ULL << (fromSquare + 16));
            if (!(blockers & (singleMove | doubleMove))  ) {
                legalMoves.push_back(Move(fromSquare, __builtin_ctzll(doubleMove), playerPieceType, -1, -1, (fromSquare + (whiteToMove ? -8 : 8)), Move::MoveType::MovedTwice, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }

        // Pawn captures for the current pawn
        uint64_t currentPawnCaptureMoves = whiteToMove ? 
            (((1ULL << (fromSquare - 9)) & opponentPieces & ~FILE_A) | ((1ULL << (fromSquare - 7)) & opponentPieces & ~FILE_H)) :
            (((1ULL << (fromSquare + 9)) & opponentPieces & ~FILE_H) | ((1ULL << (fromSquare + 7)) & opponentPieces & ~FILE_A));

        while (currentPawnCaptureMoves) {
            int toSquare = __builtin_ctzll(currentPawnCaptureMoves);
            if (!isPromotingRank   ) {
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            currentPawnCaptureMoves &= currentPawnCaptureMoves - 1; // Clear the least significant bit
        }

        // Promotion handling
        if (isPromotingRank ) {
            int promotionSquare = fromSquare + (whiteToMove ? -8 : 8);
            if (!(blockers & (1ULL << promotionSquare))) {
                // Normal promotion moves
                for (int promotedType = playerPieceType + 1; promotedType <= playerPieceType + 4; ++promotedType) {
                    legalMoves.push_back(Move(fromSquare, promotionSquare, playerPieceType, -1, -1, promotedType, Move::MoveType::Promote, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }
            }

            // Promotion captures
            currentPawnCaptureMoves = whiteToMove ? 
                (((1ULL << (fromSquare - 9)) & opponentPieces & ~FILE_A) | ((1ULL << (fromSquare - 7)) & opponentPieces & ~FILE_H)) :
                (((1ULL << (fromSquare + 9)) & opponentPieces & ~FILE_H) | ((1ULL << (fromSquare + 7)) & opponentPieces & ~FILE_A));

            while (currentPawnCaptureMoves) {
                int toSquare = __builtin_ctzll(currentPawnCaptureMoves);
                for (int promotedType = playerPieceType + 1; promotedType <= playerPieceType + 4; ++promotedType) {
                    legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, promotedType, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }
                currentPawnCaptureMoves &= currentPawnCaptureMoves - 1;
            }
        }

        // En passant capture handling
        if (enPassantSquare != -1 ) {
            int enPassantTarget = whiteToMove ? (enPassantSquare + 8) : (enPassantSquare - 8);
            if ((whiteToMove && (fromSquare / 8 == 4)) || (!whiteToMove && (fromSquare / 8 == 3))) {
                if (fromSquare == enPassantTarget - (whiteToMove ? 9 : -9) || fromSquare == enPassantTarget - (whiteToMove ? 7 : -7)) {
                    int enemyPawnSquare = whiteToMove ? (enPassantSquare - 8) : (enPassantSquare + 8);
                    
                    Move enpassant = Move(fromSquare, enPassantSquare, playerPieceType, pieceTypeAtSquare(enemyPawnSquare), enPassantSquare, -1, Move::MoveType::EnPassantCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling);
                    makeMove(enpassant);
                    if(!isKingInCheck()) {
                        legalMoves.push_back(enpassant);
                    }
                    undoMove(enpassant);
                }
            }
        }

        playerPawnsMask &= playerPawnsMask - 1; // Clear the least significant bit
    }


    // Generate knight moves
    uint64_t playerKnightsMask = knights;
    while (playerKnightsMask) {
        int fromSquare = __builtin_ctzll(playerKnightsMask);
        if(((xrayBitboard & (1ULL << fromSquare)))) {
            playerKnightsMask &= playerKnightsMask - 1;
            continue; // if the knight is pinned, we can't move it
        }
        uint64_t knightAttacks = generateKnightAttacks(fromSquare);
        
        while (knightAttacks) {
            int toSquare = __builtin_ctzll(knightAttacks);

            // Check if the target square contains an opponent's piece
            if (opponentPieces & (1ULL << toSquare)) {
                // Capture move
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 1, pieceTypeAtSquare(toSquare), -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & (1ULL << toSquare))) {
                // Normal move
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 1, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            
            knightAttacks &= knightAttacks - 1;
        }
        
        playerKnightsMask &= playerKnightsMask - 1;
    }



    // Generate bishop moves
    uint64_t playerBishopsMask = bishops;
    while (playerBishopsMask) {
        int fromSquare = __builtin_ctzll(playerBishopsMask);
        if(((xrayBitboard & (1ULL << fromSquare)))) {
            playerBishopsMask &= playerBishopsMask - 1;
            continue; // if the bishop is pinned, we can't move it
        }
        uint64_t bishopAttacks = generateBishopAttacks(fromSquare, blockers);
        
        while (bishopAttacks) {
            int toSquare = __builtin_ctzll(bishopAttacks);
            uint64_t destinationMask = (1ULL << toSquare);
            
            if (opponentPieces & destinationMask ) {
                // Capture move
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 2, pieceTypeAtSquare(toSquare), -1, -1, 
                                    Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & destinationMask)) {
                // Normal move (not blocked)
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 2, -1, -1, -1, 
                                    Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            }
            
            bishopAttacks &= bishopAttacks - 1;
        }
        
        playerBishopsMask &= playerBishopsMask - 1;
    }


    // Generate rook moves
    uint64_t playerRooksMask = rooks;
    while (playerRooksMask) {
        int fromSquare = __builtin_ctzll(playerRooksMask);
        if(((xrayBitboard & (1ULL << fromSquare)))) {
            playerRooksMask &= playerRooksMask - 1;
            continue; // if the rook is pinned, we can't move it
        }
        uint64_t rookAttacks = generateRookAttacks(fromSquare, blockers);
        
        while (rookAttacks) {
            int toSquare = __builtin_ctzll(rookAttacks);
            uint64_t destinationMask = (1ULL << toSquare);

            if (opponentPieces & destinationMask  ) {
                // Capture move
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 3, pieceTypeAtSquare(toSquare), -1, -1, 
                                    Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & destinationMask) ) {
                // Normal move (not blocked)
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 3, -1, -1, -1, 
                                    Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            }

            rookAttacks &= rookAttacks - 1;
        }
        
        playerRooksMask &= playerRooksMask - 1;
    }

    // Generate queen moves
    uint64_t playerQueensMask = queens;
    while (playerQueensMask) {
        int fromSquare = __builtin_ctzll(playerQueensMask);
        if(((xrayBitboard & (1ULL << fromSquare)))) {
            playerQueensMask &= playerQueensMask - 1;
            continue; // if the queen is pinned, we can't move it
        }
        uint64_t queenAttacks = generateBishopAttacks(fromSquare, blockers) | generateRookAttacks(fromSquare, blockers);
        
        while (queenAttacks) {
            int toSquare = __builtin_ctzll(queenAttacks);
            uint64_t destinationMask = (1ULL << toSquare);

            if (opponentPieces & destinationMask ) {
                // Capture move
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 4, pieceTypeAtSquare(toSquare), -1, -1, 
                                    Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & destinationMask)) {
                // Normal move (not blocked)
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 4, -1, -1, -1, 
                                    Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            }

            queenAttacks &= queenAttacks - 1;
        }
        
        playerQueensMask &= playerQueensMask - 1;
    }


    // Generate king moves
    uint64_t playerKingMask = king;
    while (playerKingMask) {
        int fromSquare = __builtin_ctzll(playerKingMask);
        uint64_t kingAttacks = generateKingAttacks(fromSquare);

        while (kingAttacks) {
            int toSquare = __builtin_ctzll(kingAttacks);
            uint64_t destinationMask = (1ULL << toSquare);

            if ((opponentPieces & destinationMask) && !(kingDangerSquares & destinationMask)) {
                // Capture move
                
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 5, pieceTypeAtSquare(toSquare), -1, -1, 
                                    Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & destinationMask)) {
                // Normal move (not blocked)
                legalMoves.push_back(Move(fromSquare, toSquare, playerPieceType + 5, -1, -1, -1, 
                                    Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            }
            kingAttacks &= kingAttacks - 1;
        }
        playerKingMask &= playerKingMask - 1;
    }

    // Generate castling moves
    if(whiteToMove) {
        if(whiteKingSideCastling) {
            if(!(blockers & (1ULL << 61)) && !(blockers & (1ULL << 62)) && !isKingInCheck()) {
                legalMoves.push_back(Move(60, 62, 5, -1, -1, -1, Move::MoveType::CastleKingSide, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }
        if(whiteQueenSideCastling) {
            if(!(blockers & (1ULL << 57)) && !(blockers & (1ULL << 58)) && !isKingInCheck()) {
                legalMoves.push_back(Move(60, 58, 5, -1, -1, -1, Move::MoveType::CastleQueenSide, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }
    } else {
        if(blackKingSideCastling) {
            if(!(blockers & (1ULL << 5)) && !(blockers & (1ULL << 6)) && !isKingInCheck()) {
                legalMoves.push_back(Move(4, 6, 11, -1, -1, -1, Move::MoveType::CastleKingSide, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }
        if(blackQueenSideCastling) {
            if(!(blockers & (1ULL << 1)) && !(blockers & (1ULL << 2)) && !isKingInCheck()) {
                legalMoves.push_back(Move(4, 2, 11, -1, -1, -1, Move::MoveType::CastleQueenSide, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }
    }

    // ******************************************************************************
    // en passant discovery check - edge case
    // sometimes en passant cannot be played because of a discovery attack on your own king
    // to handle this we will just use make move / unmake move for en passant moves and check if the king is in check
    // ******************************************************************************
    
    return legalMoves;
}

std::vector<Move> Board::pseudoLegalMoves() {
    std::vector<Move> moves;

    // Determine which player's moves to generate
    bool isWhiteTurn = whiteToMove;
    int playerPieceType = isWhiteTurn ? 0 : 6;  // White pieces start at 0, Black pieces start at 6
    int opponentPieceType = isWhiteTurn ? 6 : 0;
    
    uint64_t playerPawns = bitboards[playerPieceType];
    uint64_t playerKnights = bitboards[playerPieceType + 1];
    uint64_t playerBishops = bitboards[playerPieceType + 2];
    uint64_t playerRooks = bitboards[playerPieceType + 3];
    uint64_t playerQueens = bitboards[playerPieceType + 4];
    uint64_t playerKing = bitboards[playerPieceType + 5];
    
    uint64_t opponentPawns = bitboards[opponentPieceType];
    uint64_t opponentKnights = bitboards[opponentPieceType + 1];
    uint64_t opponentBishops = bitboards[opponentPieceType + 2];
    uint64_t opponentRooks = bitboards[opponentPieceType + 3];
    uint64_t opponentQueens = bitboards[opponentPieceType + 4];
    uint64_t opponentKing = bitboards[opponentPieceType + 5];

    // Combine all opponent's pieces
    uint64_t opponentPieces = opponentPawns | opponentKnights | opponentBishops | opponentRooks | opponentQueens | opponentKing;
    
    // Combine all blockers (pieces of both players)
    uint64_t blockers = 0;
    for (int i = 0; i < 12; ++i) {
        blockers |= bitboards[i];
    }





    // Generate pawn moves
    uint64_t playerPawnsMask = playerPawns;
    while (playerPawnsMask) {
        int fromSquare = __builtin_ctzll(playerPawnsMask);
        bool isPromotingRank = (isWhiteTurn && fromSquare / 8 == 0) || (!isWhiteTurn && fromSquare / 8 == 7);
        // Single pawn move (White moves down, Black moves up)
        uint64_t singleMove = isWhiteTurn ? (1ULL << (fromSquare - 8)) : (1ULL << (fromSquare + 8));
        if (!(blockers & singleMove)) {
            //moves.push_back(Move(fromSquare, __builtin_ctzll(singleMove), playerPieceType, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
        }

        // Double pawn move (White from rank 6, Black from rank 1)
        if ((isWhiteTurn && (fromSquare / 8 == 6)) || (!isWhiteTurn && (fromSquare / 8 == 1))) {
            uint64_t doubleMove = isWhiteTurn ? (1ULL << (fromSquare - 16)) : (1ULL << (fromSquare + 16));
            if (!(blockers & (singleMove | doubleMove))) {
                moves.push_back(Move(fromSquare, __builtin_ctzll(doubleMove), playerPieceType, -1, -1, (fromSquare + (isWhiteTurn ? -8 : 8)), Move::MoveType::MovedTwice, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }

        // Pawn captures for the current pawn
        uint64_t currentPawnCaptureMoves = isWhiteTurn ? 
            (((1ULL << (fromSquare - 9)) & opponentPieces & ~FILE_A) | ((1ULL << (fromSquare - 7)) & opponentPieces & ~FILE_H)) :
            (((1ULL << (fromSquare + 9)) & opponentPieces & ~FILE_H) | ((1ULL << (fromSquare + 7)) & opponentPieces & ~FILE_A));

        while (currentPawnCaptureMoves) {
            int toSquare = __builtin_ctzll(currentPawnCaptureMoves);
            if (!isPromotingRank) {
                moves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            currentPawnCaptureMoves &= currentPawnCaptureMoves - 1; // Clear the least significant bit
        }

        // Promotion handling
        if (isPromotingRank) {
            int promotionSquare = fromSquare + (isWhiteTurn ? -8 : 8);
            if (!(blockers & (1ULL << promotionSquare))) {
                // Normal promotion moves
                for (int promotedType = playerPieceType + 1; promotedType <= playerPieceType + 4; ++promotedType) {
                    moves.push_back(Move(fromSquare, promotionSquare, playerPieceType, -1, -1, promotedType, Move::MoveType::Promote, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }
            }

            // Promotion captures
            currentPawnCaptureMoves = isWhiteTurn ? 
                (((1ULL << (fromSquare - 9)) & opponentPieces & ~FILE_A) | ((1ULL << (fromSquare - 7)) & opponentPieces & ~FILE_H)) :
                (((1ULL << (fromSquare + 9)) & opponentPieces & ~FILE_H) | ((1ULL << (fromSquare + 7)) & opponentPieces & ~FILE_A));

            while (currentPawnCaptureMoves) {
                int toSquare = __builtin_ctzll(currentPawnCaptureMoves);
                for (int promotedType = playerPieceType + 1; promotedType <= playerPieceType + 4; ++promotedType) {
                    moves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, promotedType, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }
                currentPawnCaptureMoves &= currentPawnCaptureMoves - 1;
            }
        }

        // En passant capture handling
        if (enPassantSquare != -1) {
            int enPassantTarget = isWhiteTurn ? (enPassantSquare + 8) : (enPassantSquare - 8);
            if ((isWhiteTurn && (fromSquare / 8 == 4)) || (!isWhiteTurn && (fromSquare / 8 == 3))) {
                if (fromSquare == enPassantTarget - (isWhiteTurn ? 9 : -9) || fromSquare == enPassantTarget - (isWhiteTurn ? 7 : -7)) {
                    int enemyPawnSquare = isWhiteTurn ? (enPassantSquare - 8) : (enPassantSquare + 8);
                    moves.push_back(Move(fromSquare, enPassantSquare, playerPieceType, pieceTypeAtSquare(enemyPawnSquare), enPassantSquare, -1, Move::MoveType::EnPassantCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
                }
            }
        }

        playerPawnsMask &= playerPawnsMask - 1; // Clear the least significant bit
    }


    // Generate knight moves
    uint64_t playerKnightsMask = playerKnights;
    while (playerKnightsMask) {
        int fromSquare = __builtin_ctzll(playerKnightsMask);
        uint64_t knightAttacks = generateKnightAttacks(fromSquare);
        
        while (knightAttacks) {
            int toSquare = __builtin_ctzll(knightAttacks);

            // Check if the target square contains an opponent's piece
            if (opponentPieces & (1ULL << toSquare)) {
                // Capture move
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 1, pieceTypeAtSquare(toSquare), -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & (1ULL << toSquare))) {
                // Normal move
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 1, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            
            knightAttacks &= knightAttacks - 1;
        }
        
        playerKnightsMask &= playerKnightsMask - 1;
    }


    // Generate bishop moves
    uint64_t playerBishopsMask = playerBishops;
    while (playerBishopsMask) {
        int fromSquare = __builtin_ctzll(playerBishopsMask);
        uint64_t bishopAttacks = generateBishopAttacks(fromSquare, blockers);
        
        while (bishopAttacks) {
            int toSquare = __builtin_ctzll(bishopAttacks);
            uint64_t destinationMask = (1ULL << toSquare);
            
            if (opponentPieces & destinationMask) {
                // Capture move
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 2, pieceTypeAtSquare(toSquare), -1, -1, 
                                    Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & destinationMask)) {
                // Normal move (not blocked)
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 2, -1, -1, -1, 
                                    Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            }
            
            bishopAttacks &= bishopAttacks - 1;
        }
        
        playerBishopsMask &= playerBishopsMask - 1;
    }


    // Generate rook moves
    uint64_t playerRooksMask = playerRooks;
    while (playerRooksMask) {
        int fromSquare = __builtin_ctzll(playerRooksMask);
        uint64_t rookAttacks = generateRookAttacks(fromSquare, blockers);
        
        while (rookAttacks) {
            int toSquare = __builtin_ctzll(rookAttacks);
            uint64_t destinationMask = (1ULL << toSquare);

            if (opponentPieces & destinationMask) {
                // Capture move
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 3, pieceTypeAtSquare(toSquare), -1, -1, 
                                    Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & destinationMask)) {
                // Normal move (not blocked)
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 3, -1, -1, -1, 
                                    Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            }

            rookAttacks &= rookAttacks - 1;
        }
        
        playerRooksMask &= playerRooksMask - 1;
    }

    // Generate queen moves
    uint64_t playerQueensMask = playerQueens;
    while (playerQueensMask) {
        int fromSquare = __builtin_ctzll(playerQueensMask);
        uint64_t queenAttacks = generateBishopAttacks(fromSquare, blockers) | generateRookAttacks(fromSquare, blockers);
        
        while (queenAttacks) {
            int toSquare = __builtin_ctzll(queenAttacks);
            uint64_t destinationMask = (1ULL << toSquare);

            if (opponentPieces & destinationMask) {
                // Capture move
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 4, pieceTypeAtSquare(toSquare), -1, -1, 
                                    Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & destinationMask)) {
                // Normal move (not blocked)
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 4, -1, -1, -1, 
                                    Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            }

            queenAttacks &= queenAttacks - 1;
        }
        
        playerQueensMask &= playerQueensMask - 1;
    }


    // Generate king moves
    uint64_t playerKingMask = playerKing;
    while (playerKingMask) {
        int fromSquare = __builtin_ctzll(playerKingMask);
        uint64_t kingAttacks = generateKingAttacks(fromSquare);
        
        while (kingAttacks) {
            int toSquare = __builtin_ctzll(kingAttacks);
            uint64_t destinationMask = (1ULL << toSquare);

            if (opponentPieces & destinationMask) {
                // Capture move
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 5, pieceTypeAtSquare(toSquare), -1, -1, 
                                    Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            } else if (!(blockers & destinationMask)) {
                // Normal move (not blocked)
                moves.push_back(Move(fromSquare, toSquare, playerPieceType + 5, -1, -1, -1, 
                                    Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, 
                                    blackKingSideCastling, blackQueenSideCastling));
            }

            kingAttacks &= kingAttacks - 1;
        }

        playerKingMask &= playerKingMask - 1;
    }

    // Generate castling moves
    if(isWhiteTurn) {
        if(whiteKingSideCastling) {
            if(!(blockers & (1ULL << 61)) && !(blockers & (1ULL << 62)) && !isKingInCheck()) {
                moves.push_back(Move(60, 62, 5, -1, -1, -1, Move::MoveType::CastleKingSide, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }
        if(whiteQueenSideCastling) {
            if(!(blockers & (1ULL << 57)) && !(blockers & (1ULL << 58)) && !isKingInCheck()) {
                moves.push_back(Move(60, 58, 5, -1, -1, -1, Move::MoveType::CastleQueenSide, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }
    } else {
        if(blackKingSideCastling) {
            if(!(blockers & (1ULL << 5)) && !(blockers & (1ULL << 6)) && !isKingInCheck()) {
                moves.push_back(Move(4, 6, 11, -1, -1, -1, Move::MoveType::CastleKingSide, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }
        if(blackQueenSideCastling) {
            if(!(blockers & (1ULL << 1)) && !(blockers & (1ULL << 2)) && !isKingInCheck()) {
                moves.push_back(Move(4, 2, 11, -1, -1, -1, Move::MoveType::CastleQueenSide, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }
    }
    for (int i = moves.size() - 1; i >= 0; i--) {
        makeMove(moves[i]);
        if (isKingInCheck()) {
            undoMove(moves[i]);  // Undo the move before erasing
            moves.erase(moves.begin() + i);
        } else {
            undoMove(moves[i]);  // Only undo if the move is valid
        }
    }


    return moves;
}
