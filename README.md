# C++ Chess Engine
Utilising Minimax Algorithim with Alpha-Beta Pruning, Multi-Threading and Precomputed-Tables
~Roughly 1.5 million nodes per second for evaluation and move generation

## Board Representation: 12 Bitboards (Big Endian)
<img width="400" alt="16 17 19 19 20 21 22 23" src="https://github.com/user-attachments/assets/7f730726-ab3c-40e6-a5d7-85deac51db99">

## EVALUATION Method  
The evaluation ranges from -1 to 1:  
1: Checkmate for white  
-1: Checkmate for black  
0: Equal evaluation or stalemate  

The evaluation is dynamically weighted between the position and material difference.

The positional difference is calculated with piece square tables. The material difference is calculated with a sigmoid function:  

$y = \frac{1}{1.1 + e^{(-x+4)}} \quad \text{for} \quad 1 < x < 39$

where x is the numerial material difference and y is the material evaluation.  
[Link to PeSTO's Evaluation Function](https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function)

