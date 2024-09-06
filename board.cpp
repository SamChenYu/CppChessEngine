#include "board.h"
#include "move.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <bitset>

using namespace std;

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
    string castlingRights = splitString[1];
    whiteKingSideCastling = castlingRights.find('K') != string::npos;
    whiteQueenSideCastling = castlingRights.find('Q') != string::npos;
    blackKingSideCastling = castlingRights.find('k') != string::npos;
    blackQueenSideCastling = castlingRights.find('q') != string::npos;
    whiteToMove = (splitString[1].find('w') != string::npos);
    string enPassant = splitString[2];
    enPassantSquare = (enPassant == "-" ? -1 : (8 * (enPassant[1] - '1') + (enPassant[0] - 'a')));

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
            removePiece(move.getCapturedPieceType(), move.getEnPassantSquare());
            movePiece(pieceType, fromSquare, toSquare);
            break;
            
        case Move::MoveType::Promote:
            //std:: cout << "Promote Executing\n";
            removePiece(pieceType, fromSquare);
            addPiece(move.getPromotedPieceType(), toSquare);
            break;
            
        case Move::MoveType::PromoteCapture:
            std:: cout << "Promote Capture Executing\n";
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

        whiteToMove = !whiteToMove;
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
            addPiece(move.getCapturedPieceType(), move.getEnPassantSquare());
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
        whiteToMove = !whiteToMove;
        // Full move clocks etc Todo
}

void Board::flipColour() {
    whiteToMove = !whiteToMove;
}

int Board::isGameOver() {
    if(moveGeneration().empty()) {
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
    if (whiteToMove) {
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

    // Check for pawn attacks on the king
    uint64_t pawnAttacks = whiteToMove ? ((enemyPawns >> 7) & ~FILE_A) | ((enemyPawns >> 9) & ~FILE_H)
                                       : ((enemyPawns << 7) & ~FILE_H) | ((enemyPawns << 9) & ~FILE_A);
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
    const uint64_t notAFile = 0xFEFEFEFEFEFEFEFEULL;  // Mask for not A-file (file 1)
    const uint64_t notABFile = 0xFCFCFCFCFCFCFCFCULL; // Mask for not A-file and B-file
    const uint64_t notHFile = 0x7F7F7F7F7F7F7F7FULL;  // Mask for not H-file (file 8)
    const uint64_t notGHFile = 0x3F3F3F3F3F3F3F3FULL; // Mask for not G-file and H-file

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
    if (square % 8 != 7) attacks |= bitboard >> 1;        // East (right)
    if (square % 8 != 0) attacks |= bitboard << 1;        // West (left)
    if (square / 8 != 7) attacks |= bitboard >> 8;        // North (up)
    if (square / 8 != 0) attacks |= bitboard << 8;        // South (down)

    if (square % 8 != 7 && square / 8 != 7) attacks |= bitboard >> 9;  // Northeast (up-right)
    if (square % 8 != 0 && square / 8 != 7) attacks |= bitboard >> 7;  // Northwest (up-left)
    if (square % 8 != 7 && square / 8 != 0) attacks |= bitboard << 7;  // Southeast (down-right)
    if (square % 8 != 0 && square / 8 != 0) attacks |= bitboard << 9;  // Southwest (down-left)

    return attacks;
}




std::vector<Move> Board::moveGeneration() {
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

        // Single pawn move (White moves down, Black moves up)
        uint64_t possibleMove = isWhiteTurn ? (1ULL << (fromSquare - 8)) : (1ULL << (fromSquare + 8));

        // Determine if the pawn is about to promote
        bool isPromotingRank = (isWhiteTurn && (fromSquare / 8 == 6)) || (!isWhiteTurn && (fromSquare / 8 == 1));

        // Add the move only if the pawn is not on the promotion rank
        if (!isPromotingRank && !(blockers & possibleMove)) {
            moves.push_back(Move(fromSquare, __builtin_ctzll(possibleMove), playerPieceType, -1, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
        }


        // Double pawn move (White from rank 2, Black from rank 7, with adjusted flipped ranks)
        if ((isWhiteTurn && (fromSquare / 8 == 6)) || (!isWhiteTurn && (fromSquare / 8 == 1))) {
            uint64_t doubleMove = (isWhiteTurn ? (1ULL << (fromSquare - 16)) : (1ULL << (fromSquare + 16)));
            if (!(blockers & (possibleMove | doubleMove))) {
                moves.push_back(Move(fromSquare, __builtin_ctzll(doubleMove), playerPieceType, -1, -1, (fromSquare + (__builtin_ctzll(doubleMove) - fromSquare) / 2), Move::MoveType::MovedTwice, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }

        // Pawn captures (adjust the directions for the flipped bitboard)
        uint64_t captureMoves = (isWhiteTurn ? ((playerPawns << 9) & opponentPieces & ~FILE_H) | ((playerPawns << 7) & opponentPieces & ~FILE_A)
                                            : ((playerPawns >> 9) & opponentPieces & ~FILE_A) | ((playerPawns >> 7) & opponentPieces & ~FILE_H));
        
        while (captureMoves) {
            int toSquare = __builtin_ctzll(captureMoves);
            moves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            captureMoves &= captureMoves - 1;
        }

        // Promotion handling (rank 7 for white pawns, rank 2 for black pawns in your flipped board)
        if ((isWhiteTurn && (fromSquare / 8 == 1)) || (!isWhiteTurn && (fromSquare / 8 == 6))) {
            int promotionSquare = fromSquare + (isWhiteTurn ? -8 : 8);

            // Normal promotion moves
            moves.push_back(Move(fromSquare, promotionSquare, playerPieceType, -1, -1, playerPieceType + 1, Move::MoveType::Promote, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            moves.push_back(Move(fromSquare, promotionSquare, playerPieceType, -1, -1, playerPieceType + 2, Move::MoveType::Promote, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            moves.push_back(Move(fromSquare, promotionSquare, playerPieceType, -1, -1, playerPieceType + 3, Move::MoveType::Promote, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            moves.push_back(Move(fromSquare, promotionSquare, playerPieceType, -1, -1, playerPieceType + 4, Move::MoveType::Promote, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
        }

        // En passant capture
        if (enPassantSquare != -1) {
            int enPassantTarget = isWhiteTurn ? (enPassantSquare + 8) : (enPassantSquare - 8);
            if ((playerPawnsMask & (1ULL << enPassantSquare)) && !(blockers & (1ULL << enPassantSquare))) {
                moves.push_back(Move(fromSquare, enPassantTarget, playerPieceType, pieceTypeAtSquare(enPassantSquare), enPassantSquare, -1, Move::MoveType::Capture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            }
        }

        // Promotion capture
        uint64_t promotionCaptureMoves = (isWhiteTurn ? ((playerPawns << 9) & opponentPieces & ~FILE_H) | ((playerPawns << 7) & opponentPieces & ~FILE_A)
                                                    : ((playerPawns >> 9) & opponentPieces & ~FILE_A) | ((playerPawns >> 7) & opponentPieces & ~FILE_H));
        
        while (promotionCaptureMoves) {
            int toSquare = __builtin_ctzll(promotionCaptureMoves);
            moves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, playerPieceType + 1, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            moves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, playerPieceType + 2, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            moves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, playerPieceType + 3, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
            moves.push_back(Move(fromSquare, toSquare, playerPieceType, pieceTypeAtSquare(toSquare), -1, playerPieceType + 4, Move::MoveType::PromoteCapture, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));

            
            promotionCaptureMoves &= promotionCaptureMoves - 1;
        }

        playerPawnsMask &= playerPawnsMask - 1;
    }


    // Generate knight moves
    uint64_t playerKnightsMask = playerKnights;
    while (playerKnightsMask) {
        int fromSquare = __builtin_ctzll(playerKnightsMask);
        uint64_t knightAttacks = generateKnightAttacks(fromSquare);
        
        while (knightAttacks) {
            int toSquare = __builtin_ctzll(knightAttacks);
            if (!(blockers & (1ULL << toSquare)) || (opponentPieces & (1ULL << toSquare))) {
                moves.push_back(Move(fromSquare, toSquare, playerPieceType+1, opponentPieceType, -1, -1, Move::MoveType::Normal, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling));
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


    // Generate castling moves (if applicable)
    //if (canCastleKingSide(isWhiteTurn)) {
    //    moves.push_back(Move(kingFromSquare, kingFromSquare + 2, playerPieceType, -1, -1, -1, Move::MoveType::CastleKingSide));
    //}
    //if (canCastleQueenSide(isWhiteTurn)) {
    //    moves.push_back(Move(kingFromSquare, kingFromSquare - 2, playerPieceType, -1, -1, -1, Move::MoveType::CastleQueenSide));
    //}

    return moves;
}

