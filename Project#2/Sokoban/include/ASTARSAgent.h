#ifndef ASTARSAGENT_H
#define ASTARSAGENT_H

#include <string>
#include <unordered_set>
#include <queue>
#include "../include/Puzzle.h"
#include "../include/State.h"
#include "../include/Timer.h"
//ASTARS agent, takes in filename and out filename

using namespace std;

class ASTARSAgent
{
public:
    ASTARSAgent(string in, string out, bool statFlag);
    void solve();
private:
    void outputSol(State &state);
    int heuristic(State &state);
    int betterHeuristic(State &state);
    int computeDist(const Location &loc1, const Location &loc2);
    bool isDeadState(State &state, int lastMoveDir);
    bool canPushBoxToGoalAgainstWall(State &state, int lastMoveDir);
    bool clockwiseDirIsBlocked(State &state, int lastMoveDir);
    bool counterclockwiseDirIsBlocked(State &state, int lastMoveDir);
    void outputStat();
    void printState(State &state);
    string inputFilename;
    string outputFilename;
    Puzzle puzzle;
    State goalState;
    unordered_set<State, StateKeyHash, StateKeyEqual> visitedStates;
    priority_queue<State, vector<State>, greater<State> > pQueue;
    static int delta[4][2];// = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    static char direction[4];// = {'u', 'r', 'd', 'l'};
    int nodesGeneratedCount;
    int repeatedNodesCount;
    Timer timer;
    bool needStat;
};

#endif // ASTARSAGENT_H
