#ifndef MOVE_H
#define MOVE_H

#include <string>
#include <vector>

class Move {
    public:

    enum class MoveType {
        Normal,
        MovedTwice,
        Capture,
        EnPassantCapture,
        Promote,
        PromoteCapture,
        CastleKingSide,
        CastleQueenSide
    };

    std::string moveTypeToString() const;  // Function to convert MoveType to string
    // Move constructor:
    // Move(fromSquare, toSquare, pieceType, capturedPiece, enPassantSquare, promotedPieceType, moveType, whiteKingSideCastling, whiteQueenSideCastling, blackKingSideCastling, blackQueenSideCastling)


    Move() {
        fromSquare = -1;
        toSquare = -1;
        pieceType = -1;
        moveType = MoveType::Normal;
        capturedPieceType = -1;
        enPassantSquare = -1;
        promotedPieceType = -1;
        whiteKingSideCastling = false;
        whiteQueenSideCastling = false;
        blackKingSideCastling = false;
        blackQueenSideCastling = false;
    }

    Move(int fromSquare, int toSquare, int pieceType, int capturedPieceType = -1, int enPassantSquare = -1, int promotedPieceType = -1,
         MoveType moveType = MoveType::Normal, bool whiteKingSideCastling = false, bool whiteQueenSideCastling = false, 
         bool blackKingSideCastling = false, bool blackQueenSideCastling = false):
            fromSquare(fromSquare), toSquare(toSquare), pieceType(pieceType), moveType(moveType),
            capturedPieceType(capturedPieceType), enPassantSquare(enPassantSquare), promotedPieceType(promotedPieceType),
            whiteKingSideCastling(whiteKingSideCastling), whiteQueenSideCastling(whiteQueenSideCastling),
            blackKingSideCastling(blackKingSideCastling), blackQueenSideCastling(blackQueenSideCastling) {

                assert(pieceType >= 0 && pieceType < 12);
    }

    void display() const;

    std::string toString() const;

    std::string bitboardIndexToCoordinate(int index) const;


    // getters
    int getFromSquare() const {
        return fromSquare;
    }

    int getToSquare() const {
        return toSquare;
    }

    int getPieceType() const {
        return pieceType;
    }

    MoveType getMoveType() const {
        return moveType;
    }

    int getCapturedPieceType() const {
        return capturedPieceType;
    }

    int getEnPassantSquare() const {
        return enPassantSquare;
    }

    int getPromotedPieceType() const {
        return promotedPieceType;
    }

    bool getWhiteKingSideCastling() const {
        return whiteKingSideCastling;
    }

    bool getWhiteQueenSideCastling() const {
        return whiteQueenSideCastling;
    }

    bool getBlackKingSideCastling() const {
        return blackKingSideCastling;
    }

    bool getBlackQueenSideCastling() const {
        return blackQueenSideCastling;
    }

    private:
        // The current move
        int fromSquare;
        int toSquare;
        int pieceType;
        MoveType moveType;
        int capturedPieceType; // optional
        int enPassantSquare; // optional
        int promotedPieceType; // optional

        // Previous board state to undo the move
        bool whiteKingSideCastling;
        bool whiteQueenSideCastling;
        bool blackKingSideCastling;
        bool blackQueenSideCastling;
};

#endif // MOVE_H