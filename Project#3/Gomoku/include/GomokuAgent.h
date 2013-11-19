#ifndef GOMOKUAGENT_H
#define GOMOKUAGENT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <climits>
#include <cmath>
#include "../include/Move.h"

using namespace std;

class GomokuAgent
{
public:
    GomokuAgent(int n, int m, int s, char p, char _mode);
    void kickOff();
private:
    const string PIPE_NAME = "mypipe";
    typedef vector<string> Board;
    Board currState;
    char agentCharacter;  // 'x' or 'o'
    char opponentCharacter; // 'x' or 'o'
    char mode; // d for dumb mode(use random strategy), s for smart mode(use alpha beta search)
    ofstream ofs;
    int boardDimension;
    int winningChainLength;
    int timeLimit;
    unordered_set<Move, MoveKeyHash, MoveKeyEqual> remainingMoveList;
    struct Action {
        Move move;
        long long minimax;
        Action() {}
        Action(Move _move, int _minimax) : move(_move), minimax(_minimax) {}
    };
    int delta[4][2] = {{+1, 0}, {+1, +1}, {0, +1}, {-1, +1}};
private:
    Move alphaBetaSearch();
    Move getRandomMove();
    void placePiece(Move &move, char stoneCharacter);
    void writeToPipe(Move &move);
    void printBoard();
    void outputStartInfo();
    bool gameOver(Move &lastMove, char lastPieceType);
    bool hasUnbrokenChainOfLen(int len, Move &lastMove, char lastPieceType, Board &state);
    bool isOutOfBound(int x, int y);
    bool isIllegalMove(Move &move);
    Action maxValue(Board &state, int depth, long long alpha, long long beta);
    Action minValue(Board &state, int depth, long long alpha, long long beta);
    bool hasStoneNearby(int x, int y, Board &state);
    bool agentWon();
    bool opponentWon();
    long long evaluate(Board &state, char nextTurnChar);
    bool hasOpen(int len, char pieceType, Board &state);
    bool hasCapped(int len, char pieceType, Board &state);
    bool hasOpenStartingFrom(int x, int y, int dir, int len, char pieceType, Board &state);
    bool hasCappedStartingFrom(int x, int y, int dir, int len, char pieceType, Board &state);
};

#endif // GOMOKUAGENT_H