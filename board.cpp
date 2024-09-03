#include "board.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

using namespace std;

void Board::setupInitialPosition() {
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