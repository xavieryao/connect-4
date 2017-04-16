//
//  UCTStrategy.h
//  Strategy
//
//  Created by 沛然 姚 on 2017/4/16.
//

#ifndef UCTStrategy_h
#define UCTStrategy_h

#include "Point.h"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include <bitset>

typedef int ptr;
typedef unsigned long ul;

struct Node {
    Node* parent;
    std::vector<Node*> childern;
    int** state;
    ul reward;
    ul times;
    Point p;
    
    Node(Node* parent, int** state) {
        this->parent = parent;
        this->state = state;
        this->reward = -1000;
        this->times = 0;
    }
};

class UCTStrategy {
    
private:
    int M;
    int N;
    int noX;
    int noY;
    
public:
    UCTStrategy(int m, int n, int noX, int noY);
    UCTStrategy() : M(0), N(0), noX(-1), noY(-1) {};
    
    Point getPoint(const int* top, int** board,
                    const int lastX, const int lastY);
    
    bool valid() const;
    
private:
    Point uctSearch(int** s0);
    Node* treePolicy(Node* v);
    Node* expand(Node* v);
    Node* bestChild(Node* v, double c);
    ul defalutPolicy(int** s);
    void backup(Node* v, ul delta);
};

#endif /* UCTStrategy_h */
