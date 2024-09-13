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



// Precomputing Tables
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

uint64_t Board::getKnightAttacks(int square) {
    return knightAttacks[square];
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





std::vector<Move> Board::legalMoveGeneration() {
    //return pseudoLegalMoves();
    vector<Move> legalMoves;

    // https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/
    // 1) - check the square the opponent is attacking -> generate your own king moves
    // remove your own king during opponent attack generation because of sliding pieces

    // below checks for the amount of pieces attacking the king as well as opponent attack bitboard to generate legal moves
    uint64_t kingDangerSquares = 0;
    uint64_t enemyPawns, enemyKnights, enemyBishopsQueens, enemyRooksQueens, enemyKing;
    int kingIndex, kingSquare; // index is only used for blocker calculation
    int kingAttacks = 0; // if double 
    if (!whiteToMove) {
        enemyPawns = bitboards[0];                   // White pawns
        enemyKnights = bitboards[1];                 // White knights
        enemyBishopsQueens = bitboards[2] | bitboards[4];   // White bishops and queens
        enemyRooksQueens = bitboards[3] | bitboards[4];     // White rooks and queens
        enemyKing = bitboards[5];                    // White king
        kingIndex = 11;
        kingSquare = __builtin_ctzll(bitboards[11]);
    } else {
        enemyPawns = bitboards[6];                   // Black pawns
        enemyKnights = bitboards[7];                 // Black knights
        enemyBishopsQueens = bitboards[8] | bitboards[10];  // Black bishops and queens
        enemyRooksQueens = bitboards[9] | bitboards[10];    // Black rooks and queens
        enemyKing = bitboards[11];                   // Black king
        kingIndex = 5;
        kingSquare = __builtin_ctzll(bitboards[5]);
    }

    // Calculate the blocker bitboard (all pieces)
    uint64_t blockers = 0; 
    for (int i = 0; i < 12; ++i) {
        blockers |= bitboards[i];
    }

    if (!whiteToMove) {
        // Checking pawn attacks
        if ((enemyPawns >> 9) & (1ULL << kingSquare) & ~FILE_A) kingAttacks++;
        if ((enemyPawns >> 7) & (1ULL << kingSquare) & ~FILE_H) kingAttacks++;

        kingDangerSquares |= ((enemyPawns >> 9) & ~FILE_A);  // Pawns attacking from the left
        kingDangerSquares |= ((enemyPawns >> 7) & ~FILE_H);  // Pawns attacking from the right

    } else {
        if ((enemyPawns << 9) & (1ULL << kingSquare) & ~FILE_H) kingAttacks++;
        if ((enemyPawns << 7) & (1ULL << kingSquare) & ~FILE_A) kingAttacks++;

        kingDangerSquares |= ((enemyPawns << 9) & ~FILE_H);
        kingDangerSquares |= ((enemyPawns << 7) & ~FILE_A);
    }

    // Checking knight attacks
    uint64_t knightAttacks = 0;
    while (enemyKnights) {
        int square = __builtin_ctzll(enemyKnights);
        knightAttacks |= generateKnightAttacks(square);

        kingDangerSquares |= generateKnightAttacks(square);
        enemyKnights &= enemyKnights - 1;
    }
    if (knightAttacks & (1ULL << kingSquare)) kingAttacks++;

    // Checking bishop/queen (diagonal) attacks
    uint64_t bishopAttacks = 0;
    while (enemyBishopsQueens) {
        int square = __builtin_ctzll(enemyBishopsQueens);
        bishopAttacks |= generateBishopAttacks(square, blockers);

        kingDangerSquares |= generateBishopAttacks(square, blockers);
        enemyBishopsQueens &= enemyBishopsQueens - 1;
    }
    if (bishopAttacks & (1ULL << kingSquare)) kingAttacks++;

    // Checking rook/queen (horizontal/vertical) attacks
    uint64_t rookAttacks = 0;
    while (enemyRooksQueens) {
        int square = __builtin_ctzll(enemyRooksQueens);
        rookAttacks |= generateRookAttacks(square, blockers);
        kingDangerSquares |= generateRookAttacks(square, blockers);
        enemyRooksQueens &= enemyRooksQueens - 1;
    }
    if (rookAttacks & (1ULL << kingSquare)) kingAttacks++;

    // Checking king attacks
    kingDangerSquares |= generateKingAttacks(__builtin_ctzll(enemyKing));


    bool check, doubleCheck;
    // Check if there's a double check
    if (kingAttacks >= 2) {
        std::cout << "Double check!\n";
        doubleCheck = true;
        check = true;
    } else if (kingAttacks == 1) {
        std::cout << "Single check!\n";
        doubleCheck = false;
        check = true;
    } else {
        std::cout << "King is not in check.\n";
        doubleCheck = false;
        check = false;
    }
    std:: cout << "King Danger Squares\n";
    std::bitset<64> dangerSquares(kingDangerSquares);
    for (int i = 0; i < 64; i++) {
        std::cout << dangerSquares[i];
        if ((i + 1) % 8 == 0) {
            std::cout << std::endl;
        }
    }

    // 2) -> if in check, then evade, captures or block the check
    //    -> if in double check, only evade
    //    -> en passant checks


    if(doubleCheck) {
        // Evade the double check
        uint64_t kingMoves = generateKingAttacks(kingSquare) & ~kingDangerSquares;
        while (kingMoves) {
            int toSquare = __builtin_ctzll(kingMoves);
            if(pieceTypeAtSquare(toSquare) == -1) {
                legalMoves.push_back(Move(kingSquare, toSquare, kingIndex, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            } else {
                legalMoves.push_back(Move(kingSquare, toSquare, kingIndex, pieceTypeAtSquare(toSquare), -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
            kingMoves &= kingMoves - 1;
        }

    } else if(check) {
        // Evade, capture or block the check
    }





    // 3) -> pinned pieces
    /*
    
        1 -Moves from the opponent’s sliding pieces.
        2 -“Sliding piece” moves from my king in the opposite direction.
        3 -The overlap of these two rays.
        If the result of 3) lands on one of my pieces, then it is pinned.

        Once all the pinned pieces are identified, you can remove them from the board and calculate the moves from the enemy’s pinning piece to your king. This will give you a “ray” of legal moves for each of your pinned pieces.

        Note that since knights can only move in an L-shape, they can never move along a pin ray. The only piece types you need to consider are:

        On a diagonal ray: bishops, queens, and pawns (captures only).
        On a non-diagonal ray: rooks, queens, and pawns (pushes only).
    
    */





    

    return legalMoves;
}

uint64_t Board::kingDangerSquares() const{
    uint64_t kingDangerSquares = 0;
    return 0;
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
        bool isPromotingRank = (isWhiteTurn && fromSquare / 8 == 1) || (!isWhiteTurn && fromSquare / 8 == 6);
        
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

    return moves;
}
