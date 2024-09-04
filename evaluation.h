#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"

// Evaluation Tables from https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function

extern int mg_pieceSquareTable[12][64];
extern int mg_pieceSquareTable[12][64];
extern int pieceValues[12];

double evaluate(Board& board);

#endif // EVALUATION_H
