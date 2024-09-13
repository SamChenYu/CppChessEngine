#include "evaluation.h"
#include "board.h"

#include <iostream>
#include <cmath>


double evaluate(Board& board) {
    // -> note: only uses the mg piece square tables

    double whitePos = 0.0;
    int whiteMaterial = 0;
    double blackPos = 0.0;
    int blackMaterial = 0;

    for (int i = 0; i < 12; ++i) {
        uint64_t bitboard = board.bitboards[i];
        int pieceIndex = i % 6;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            bitboard &= bitboard - 1;  // Remove the lowest set bit
            if (i < 6) {
                whitePos += mg_pieceSquareTable[i][square];
                whiteMaterial += pieceValues[i];
            } else {
                blackPos += mg_pieceSquareTable[i][square];
                blackMaterial += pieceValues[i];
            }
        }
    }

    // Calculate material difference
    int materialDifference = whiteMaterial - blackMaterial;
    
    // Dynamic weighting based on material imbalance
    double materialWeight;
    double positionWeight;
    int absDiff = std::abs(materialDifference);
    if (absDiff > 8) {
        materialWeight = 0.9;
        positionWeight = 0.1;
    } else if (absDiff > 6) {
        materialWeight = 0.7;
        positionWeight = 0.3;
    } else if (absDiff > 4) {
        materialWeight = 0.5;
        positionWeight = 0.5;
    } else if (absDiff > 1) {
        materialWeight = 0.3;
        positionWeight = 0.7;
    } else {
        materialWeight = 0.4;
        positionWeight = 0.6;
    }


        // Calculating material difference values
        // Sigmoid function:
        // y = 1/(1.1 + e^(-x+4)) { 40 > x > 0 }
        double materialValue;
        if(materialDifference > 0) {                                           // POSITIVE Material Diff
            materialValue = 1.0 / (1.1 + std::exp(-(materialDifference) + 4));
        } else if(materialDifference < 0) {
            materialDifference *= -1;                                          // NEGATIVE Material Diff
            materialValue = 1.0 / (1.1 + std::exp(-(materialDifference) + 4));
            materialValue *= -1.0;
        } else {                                                               // EQUAL Material Diff
            materialValue = 0.0;
        }
        materialValue *= materialWeight;





    // Calculating positional difference values
    const double positionScalar = 1169.0; // Highest amount of points possible given a full board
    double whitePositionValue = (whitePos / positionScalar) * positionWeight;
    double blackPositionValue = (blackPos / positionScalar) * positionWeight;

    // Evaluating the total position
    double evaluation = (whitePositionValue - blackPositionValue + materialValue);

    // Print intermediate values for debugging
    // std::cout << "White Position: " << whitePos << std::endl;
    // std::cout << "Black Position: " << blackPos << std::endl;
    // std::cout << "Material Difference: " << materialDifference << std::endl;
    // std::cout << "Material Value: " << materialValue << std::endl;
    // std::cout << "White Position Value: " << whitePositionValue << std::endl;
    // std::cout << "Black Position Value: " << blackPositionValue << std::endl;
    // std::cout << "Evaluation: " << evaluation << std::endl;

    return evaluation;
}



