//
//  UCTStrategy.h
//  Strategy
//
//  Created by 沛然 姚 on 2017/4/16.
//

#ifndef UCTStrategy_h
#define UCTStrategy_h

#include "Point.h"
#include "Judge.h"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include <bitset>
#include <chrono>
#include <cstdio>
#include <cassert>

typedef int ptr;
typedef std::chrono::time_point<std::chrono::system_clock> Time;

extern int _M, _N;

struct Node {
    Node* parent;
    std::vector<Node*> children;
    int** state;
    double reward;
    int visited;
    Point action;
    bool mySide;
    std::vector<Point> availableActions;
    
    Node(Node* parent, int** state, bool mySide = true, Point action = Point()) {
        this->parent = parent;
        this->state = state;
        this->reward = 0;
        this->visited = 1;
        this->mySide = mySide;
        this->action = action;
    }
    
    ~Node() {

        for (int i = 0; i<_M; i++) {
            delete[] state[i];
        }
        delete[] state;
        for (Node* child : children) {
            delete child;
        }
    }
    
};

class UCTStrategy {
    
private:
    int M;
    int N;
    int noX;
    int noY;
    
    const int* top;
    int timeout = 150000;
    double coefficient = 1.0;
    
    enum GameState {
        COMPUTER_WIN, USER_WIN, TIE, PLAYING
    };
    
public:
    UCTStrategy(int m, int n, int noX, int noY);
    UCTStrategy() : M(0), N(0), noX(-1), noY(-1) {};
    
    Point getPoint(const int M, const int N, const int* top, int** board,
                   const int lastX, const int lastY, const int noX, const int noY);
    
    bool valid() const;
    
private:
    Point uctSearch(int** s0, int lastX, int lastY);
    Node* treePolicy(Node* v);
    Node* expand(Node* v);
    Node* bestChild(Node* v, double c);
    int defaultPolicy(Node* v);
    void backup(Node* v, int delta);
    bool hasTimeout(Time& start);
    bool isNodeTerminal(Node* v);
    std::vector<Point> actions(int** s);
    int** performAction(int** s0, Point action, int pawn);
    GameState getGameState(Node* v);
    void printBoard(int** board);
};

#endif /* UCTStrategy_h */
