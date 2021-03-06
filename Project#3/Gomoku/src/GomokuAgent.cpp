#include "../include/GomokuAgent.h"
#include <cstdlib>
#include <iomanip>

GomokuAgent::GomokuAgent(int n, int m, int s, char p, char _mode) {
    //ctor
    boardDimension = n;
    winningChainLength = m;
    timeLimit = s;
    agentCharacter = p;
    if (p == 'x')
        opponentCharacter = 'o';
    else
        opponentCharacter = 'x';
    mode = _mode;
    currState.assign(boardDimension, string(boardDimension, '.'));
    remainingMoveList.clear();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            Move move(i, j);
            remainingMoveList.insert(move);
        }
    //printBoard();
    srand (time(NULL));
    ofs.open(PIPE_NAME, std::ofstream::out | std::ofstream::app);
    //...
//    currState[7][7] = 'x';
//    currState[8][6] = 'o';
//    currState[7][8] = 'x';
//    currState[7][6] = 'o';
//    currState[6][6] = 'x';
//    currState[5][5] = 'o';
//    currState[8][8] = 'x';
//    currState[9][9] = 'o';
//    currState[7][9] = 'x';
//    currState[9][6] = 'o';
//    currState[10][6] = 'x';
//    currState[9][8] = 'o';
//    currState[9][7] = 'x';
//    currState[6][10] = 'o';
//    currState[7][11] = 'x';
//    currState[7][10] = 'o';
//    remainingMoveList.erase(Move(7, 7));
//    remainingMoveList.erase(Move(8, 6));
//    remainingMoveList.erase(Move(7, 8));
//    remainingMoveList.erase(Move(7, 6));
//    remainingMoveList.erase(Move(6, 6));
//    remainingMoveList.erase(Move(5, 5));
//    remainingMoveList.erase(Move(8, 8));
//    remainingMoveList.erase(Move(9, 9));
//    remainingMoveList.erase(Move(7, 9));
//    remainingMoveList.erase(Move(9, 6));
//    remainingMoveList.erase(Move(10, 6));
//    remainingMoveList.erase(Move(9, 8));
//    remainingMoveList.erase(Move(9, 7));
//    remainingMoveList.erase(Move(6, 10));
//    remainingMoveList.erase(Move(7, 11));
//    remainingMoveList.erase(Move(7, 10));
}

string GomokuAgent::PIPE_NAME = "mypipe";
int GomokuAgent::delta[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, +1},
                                {+1, +1}, {+1, 0}, {+1, -1}, {0, -1}};

void GomokuAgent::kickOff() {
    cout << "Game kicks off..." << endl;
    outputStartInfo();
    Move opponentMove;
    Move agentMove;
    while (true) {
        cout << "Please input your move: ";
        cin >> opponentMove.x >> opponentMove.y;
        if (isIllegalMove(opponentMove))
            break;
        placePiece(opponentMove, opponentCharacter);
        printBoard();
        if (gameOver(opponentMove, opponentCharacter))
            break;
        if (mode == 's')
            agentMove = alphaBetaSearch();
        else
            agentMove = getRandomMove();
        cout << "I choose move: " << agentMove.x << " " << agentMove.y << endl;
        writeToPipe(agentMove);
        placePiece(agentMove, agentCharacter);
        printBoard();
        if (gameOver(agentMove, agentCharacter))
            break;
    }
}

void GomokuAgent::writeToPipe(Move &move) {
    ofs << move.x << move.y << endl;
}

Move GomokuAgent::getRandomMove() {
    unordered_set<Move, MoveKeyHash, MoveKeyEqual>::iterator itr = remainingMoveList.begin();
    int randomNum = rand() % remainingMoveList.size();
    Move move;
    for (int i = 0; i < randomNum; i++) {
        itr++;
        move = *itr;
    }
    return move;
}

bool GomokuAgent::isIllegalMove(Move &move) {
    if (remainingMoveList.find(move) == remainingMoveList.end()) {
        cout << "Sorry, you enter an illegal move which is already occupied. You lost!" << endl;
        return true;
    }
    return false;
}

void GomokuAgent::outputStartInfo() {
    if (agentCharacter == 'x') {
        cout << "I am player x and move first!" << endl;
        Move move(boardDimension / 2, boardDimension / 2);
        placePiece(move, 'x');
        cout << "I choose move: " << move.x << " " << move.y << endl;
        printBoard();
    } else {
        cout << "I am player o and move second!" << endl;
        printBoard();
    }
}

void GomokuAgent::placePiece(Move &move, char stoneCharacter) {
    currState[move.x][move.y] = stoneCharacter;
    remainingMoveList.erase(move);
}

bool GomokuAgent::gameOver(Move &lastMove, char lastPieceType) {
    if (remainingMoveList.empty()) {
        cout << "The game board is completely full of stones. The game ended in a draw" << endl;
        return true;
    }
    if (hasUnbrokenChainOfLen(winningChainLength, lastMove, lastPieceType, currState)) {
        cout << "Player " << lastPieceType << " has created an unbroken chain of " << winningChainLength
             << ". Player " << lastPieceType << " wins!" << endl;
        return true;
    }
    return false;
}

//test whether after move lastMove, an unbroken chain of len is formed
bool GomokuAgent::hasUnbrokenChainOfLen(int len, Move &lastMove, char lastPieceType, Board &state) {
    // try all 8 direcions
    for (int i = 0; i < 8; i++) {
        int count = 1;
        int x = lastMove.x;
        int y = lastMove.y;

        // expand in both directions
        int xNext = x - delta[i][0];
        int yNext = y - delta[i][1];
        while (true) {
            if (isOutOfBound(xNext, yNext))
                break;
            if (currState[xNext][yNext] != lastPieceType)
                break;
            count++;
            xNext -= delta[i][0];
            yNext -= delta[i][1];
        }

        xNext = x + delta[i][0];
        yNext = y + delta[i][1];
        while (true) {
            if (isOutOfBound(xNext, yNext))
                break;
            if (currState[xNext][yNext] != lastPieceType)
                break;
            count++;
            xNext += delta[i][0];
            yNext += delta[i][1];
        }
        if (count >= len)
            return true;
    }
    return false;
}

bool GomokuAgent::isOutOfBound(int x, int y) {
    return (x < 0 || x >= boardDimension || y < 0 || y >= boardDimension);
}

void GomokuAgent::printBoard() {
    cout << "Now the game board looks like the following:" << endl;
    cout << "   ";
    for (int i = 0; i < boardDimension; i++)
        cout << setw(3) << i;
    cout << endl;
    for (int i = 0; i < boardDimension; i++) {
        cout << setw(3) << i;
        for (int j = 0; j < boardDimension; j++)
            cout << setw(3) << currState[i][j];
        cout << endl;
    }
}

int GomokuAgent::timeUsed() {
    timer.setEndTime();
    return timer.getTimeElapsed() / 1000;
}

Move GomokuAgent::alphaBetaSearch() {
    if (remainingMoveList.size() == boardDimension * boardDimension)
        return Move(boardDimension / 2, boardDimension / 2);
        //return Move(1, 1);
    int depth = 4;
    if (timeLimit < 150)
        depth = 3;
    timer.setStartTime();
    Action bestAction = maxValue(currState, depth, LLONG_MIN, LLONG_MAX);
    cout << "Used time(seconds): " << timeUsed() << endl;
    return bestAction.move;
}

bool GomokuAgent::agentWon(Move &lastMove, char lastPieceType, Board &state) {
//    if (hasOpen(winningChainLength - 1, agentCharacter, state)
//        && !hasOpen(winningChainLength - 1, opponentCharacter, state)
//        && !hasCapped(winningChainLength - 1, opponentCharacter, state))
//        return true;
    return (lastPieceType == agentCharacter &&
            hasUnbrokenChainOfLen(winningChainLength, lastMove, lastPieceType, state));
}

bool GomokuAgent::opponentWon(Move &lastMove, char lastPieceType, Board &state) {
//    if (hasOpen(winningChainLength - 1, opponentCharacter, state)
//        && !hasOpen(winningChainLength - 1, agentCharacter, state)
//        && !hasCapped(winningChainLength - 1, agentCharacter, state))
//        return true;
    return (lastPieceType == opponentCharacter &&
            hasUnbrokenChainOfLen(winningChainLength, lastMove, lastPieceType, state));
}

bool GomokuAgent::hasStoneNearby(int x, int y, Board &state) {
    for (int i = 0; i < 8; i++) {
        int xNext = x + delta[i][0];
        int yNext = y + delta[i][1];
        if (isOutOfBound(xNext, yNext))
            continue;
        if (state[xNext][yNext] != '.')
            return true;
    }
    return false;
}

bool GomokuAgent::timeLimitExceeded() {
    timer.setEndTime();
    return (timer.getTimeElapsed() > timeLimit * 1000);
}

GomokuAgent::Action GomokuAgent::maxValue(Board &state, int depth, long long alpha, long long beta) {
    //printBoard();
    if (depth == 0) {
        //printBoard();
        Action action(Move(1, 1), evaluate(state, agentCharacter));
        return action;
    }
    Move bestMove = *remainingMoveList.begin();
    long long minimax = LLONG_MIN;
    unordered_set<Move, MoveKeyHash, MoveKeyEqual>::iterator itr = remainingMoveList.begin();
    for (; itr != remainingMoveList.end(); itr++) {
        if (timeLimitExceeded())
            break;
        Move move = *itr;
        if (state[move.x][move.y] == '.' && hasStoneNearby(move.x, move.y, state)) {
            state[move.x][move.y] = agentCharacter;
            if (agentWon(move, agentCharacter, state)) {
                minimax = LLONG_MAX;
                bestMove = move;
                state[move.x][move.y] = '.';
                break;
            }
            Action action = minValue(state, depth - 1, alpha, beta);
            if (action.minimax > minimax) {
                minimax = action.minimax;
                bestMove = move;
            }
            if (minimax >= beta) {
                state[move.x][move.y] = '.';
                break;
            }
            alpha = max(alpha, minimax);
            state[move.x][move.y] = '.';
        }
    }
    return Action(bestMove, minimax);
}

GomokuAgent::Action GomokuAgent::minValue(Board &state, int depth, long long alpha, long long beta) {
    //printBoard();
    if (depth == 0) {
        //printBoard();
        Action action(Move(1, 1), evaluate(state, opponentCharacter));
        return action;
    }
    Move bestMove = *remainingMoveList.begin();
    long long minimax = LLONG_MAX;
    unordered_set<Move, MoveKeyHash, MoveKeyEqual>::iterator itr = remainingMoveList.begin();
    for (; itr != remainingMoveList.end(); itr++) {
        if (timeLimitExceeded())
            break;
        Move move = *itr;
        if (state[move.x][move.y] == '.' && hasStoneNearby(move.x, move.y, state)) {
            state[move.x][move.y] = opponentCharacter;
            if (opponentWon(move, opponentCharacter, state)) {
                minimax = LLONG_MIN;
                bestMove = move;
                state[move.x][move.y] = '.';
                break;
            }
            Action action = maxValue(state, depth - 1, alpha, beta);
            if (action.minimax < minimax) {
                minimax = action.minimax;
                bestMove = move;
            }
            if (minimax <= alpha) {
                state[move.x][move.y] = '.';
                break;
            }
            beta = min(beta, minimax);
            state[move.x][move.y] = '.';
        }
    }
    return Action(bestMove, minimax);
}

long long GomokuAgent::evaluate(Board &state, char nextTurnChar) {
    if (nextTurnChar == agentCharacter) {
        if (hasOpen(winningChainLength - 1, agentCharacter, state) || hasCapped(winningChainLength - 1, agentCharacter, state))
            return LLONG_MAX / 10;
    }
    if (nextTurnChar == opponentCharacter) {
        if (hasOpen(winningChainLength - 1, opponentCharacter, state) || hasCapped(winningChainLength - 1, opponentCharacter, state))
            return LLONG_MIN / 10;
    }
    long long utility = 0;
    for (int i = winningChainLength; i >= 1; i--) {
        if (hasOpen(i, agentCharacter, state) && !hasOpen(i, opponentCharacter, state)) {
            utility = pow(100, i);
            break;
        }
        if (hasOpen(i, agentCharacter, state) && hasOpen(i, opponentCharacter, state)) {
            utility = pow(100, i) / 300;
            break;
        }
        if (!hasOpen(i, agentCharacter, state) && hasOpen(i, opponentCharacter, state)) {
            utility = -pow(100, i);;
            break;
        }
        if (hasCapped(i, agentCharacter, state) && !hasCapped(i, opponentCharacter, state)) {
            utility = pow(100, i) / 300;
            break;
        }
        if (hasCapped(i, agentCharacter, state) && hasCapped(i, opponentCharacter, state)) {
            utility = pow(100, i) / 500;
            break;
        }
        if (!hasCapped(i, agentCharacter, state) && hasCapped(i, opponentCharacter, state)) {
            utility = -pow(100, i) / 150;
            break;
        }
    }
    //cout << "utility = " << utility << endl;
    return utility;
}

bool GomokuAgent::hasOpenStartingFrom(int x, int y, int dir, int len, char pieceType, Board &state) {
    if (state[x][y] != pieceType)
        return false;
    int expandLen = 0;
    int xNext = x - delta[dir][0];
    int yNext = y - delta[dir][1];
    if (isOutOfBound(xNext, yNext))
        return false;
    if (state[xNext][yNext] != '.')
        return false;

    // expand...
    while (true) {
        if (isOutOfBound(xNext, yNext))
            break;
        if (state[xNext][yNext] != pieceType && state[xNext][yNext] != '.')
            break;
        expandLen++;
        xNext -= delta[dir][0];
        yNext -= delta[dir][1];
    }
    xNext = x;
    yNext = y;
    int count = 0;
    while (true) {
        if (isOutOfBound(xNext, yNext))
            return false;
        if (state[xNext][yNext] != pieceType)
            return false;
        count++;
        if (count == len)
            break;
        xNext += delta[dir][0];
        yNext += delta[dir][1];
    }
    if (count != len)
        return false;
        //return true;
    xNext += delta[dir][0];
    yNext += delta[dir][1];
    if (isOutOfBound(xNext, yNext))
        return false;
    if (state[xNext][yNext] != '.')
        return false;

    // expand...
    while (true) {
        if (isOutOfBound(xNext, yNext))
            break;
        if (state[xNext][yNext] != pieceType && state[xNext][yNext] != '.')
            break;
        expandLen++;
        xNext += delta[dir][0];
        yNext += delta[dir][1];
    }

    if (count + expandLen < winningChainLength)
        return false;

    return true;
}

bool GomokuAgent::hasOpen(int len, char pieceType, Board &state) {
    for (int i = 0; i < boardDimension; i++)
        for (int j = 0; j < boardDimension; j++)
            for (int k = 0; k < 8; k++) {
                if (hasOpenStartingFrom(i, j, k, len, pieceType, state))
                    return true;
            }
    return false;
}

bool GomokuAgent::hasCappedStartingFrom(int x, int y, int dir, int len, char pieceType, Board &state) {
    if (state[x][y] != pieceType)
        return false;
    int xNext = x - delta[dir][0];
    int yNext = y - delta[dir][1];
    if (isOutOfBound(xNext, yNext)
       || (state[xNext][yNext] != pieceType && state[xNext][yNext] != '.')) {
        // reverse direction is blocked
        xNext = x;
        yNext = y;
        int count = 0;
        while (true) {
            if (isOutOfBound(xNext, yNext))
                return false;
            if (state[xNext][yNext] != pieceType)
                return false;
            count++;
            if (count == len)
                break;
            xNext += delta[dir][0];
            yNext += delta[dir][1];
        }
        if (count != len)
            return false;
        xNext += delta[dir][0];
        yNext += delta[dir][1];
        if (isOutOfBound(xNext, yNext))
            return false;
        if (state[xNext][yNext] != '.')
            return false;
        int expandLen = 0;
        // expand...
        while (true) {
            if (isOutOfBound(xNext, yNext))
                break;
            if (state[xNext][yNext] != pieceType && state[xNext][yNext] != '.')
                break;
            expandLen++;
            xNext += delta[dir][0];
            yNext += delta[dir][1];
        }
        return (expandLen + count >= winningChainLength);
    } else {
        //reverse direction is unblocked
        int expandLen = 0;
        // expand...
        while (true) {
            if (isOutOfBound(xNext, yNext))
                break;
            if (state[xNext][yNext] != pieceType && state[xNext][yNext] != '.')
                break;
            expandLen++;
            xNext -= delta[dir][0];
            yNext -= delta[dir][1];
        }
        xNext = x;
        yNext = y;
        int count = 0;
        while (true) {
            if (isOutOfBound(xNext, yNext))
                return false;
            if (state[xNext][yNext] != pieceType)
                return false;
            count++;
            if (count == len)
                break;
            xNext += delta[dir][0];
            yNext += delta[dir][1];
        }
        if (count != len)
            return false;
        xNext += delta[dir][0];
        yNext += delta[dir][1];
        if (isOutOfBound(xNext, yNext))
            return true;
        if (state[xNext][yNext] != '.' && state[xNext][yNext] != pieceType)
            return true;
        return false;
    }
}

bool GomokuAgent::hasCapped(int len, char pieceType, Board &state) {
    for (int i = 0; i < boardDimension; i++)
        for (int j = 0; j < boardDimension; j++)
            for (int k = 0; k < 8; k++) {
                if (hasCappedStartingFrom(i, j, k, len, pieceType, state))
                    return true;
            }
    return false;
}
