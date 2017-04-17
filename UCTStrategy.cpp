//
//  UCTStrategy.cpp
//  Strategy
//
//  Created by 沛然 姚 on 2017/4/16.
//  Copyright © 2017年 Yongfeng Zhang. All rights reserved.
//

#include "UCTStrategy.h"
#include <iostream>

UCTStrategy::UCTStrategy(int m, int n, int noX, int noY) {
    this->M = m;
    this->N = n;
    this->noX = noX;
    this->noY = noY;
    
    srand(static_cast<unsigned>(time(0)));
}

bool UCTStrategy::valid() const {
    if (M == 0) return false;
    return true;
}

Point UCTStrategy::getPoint(const int m, const int n, const int* top,
                            int** board,const int lastX, const int lastY,
                            const int noX, const int noY) {
    this->M = m;
    this->N = n;
    this->noX = noX;
    this->noY = noY;
    return uctSearch(board, lastX, lastY);
}

Point UCTStrategy::uctSearch(int** s0, int lastX, int lastY) {
    //    printf("uctSearch Started\n");
    Time startTime = std::chrono::system_clock::now();
    
    Node* v0 = new Node(nullptr, s0, true, Point(lastX, lastY));
    v0->availableActions = actions(v0->state);
    // 以状态s0创建根节点v0
    
    int i = 0;
    
    while (i <= 6000) {
        //        printf("uctSearch iteration\n");
        Node* vl = treePolicy(v0);
        int delta = defaultPolicy(vl);
        backup(vl, delta);
        i ++;
    }
    Point result = bestChild(v0, 0)->action;
    for (auto child :v0->children) {
        delete child;
    }
    return result;
}

Node* UCTStrategy::treePolicy(Node* v) {
    //    printf("tree policy\n");
    while (!isNodeTerminal(v)) { // while节点v不是终止节点
        //        printf("tree policy iteration\n");
        if (v->availableActions.size() > 0) { // if 节点v是可扩展的
            //            printf("v 可扩展\n");
            return expand(v);
        } else {
            //            printf("v 不可扩展\n");
            v = bestChild(v, coefficient);
        }
    }
    return v;
}

Node* UCTStrategy::expand(Node* v) {
    //    printf("扩展\n");
    assert(v->availableActions.size() > 0);
    int idx = rand()%v->availableActions.size();
    auto action = v->availableActions[idx];
    v->availableActions[idx] = v->availableActions.back();
    v->availableActions.pop_back(); // 选择尚未选择过的行动
    auto newState = performAction(v->state, action, (v->mySide ? 2 : 1));
    Node* vv = new Node(v, newState, !v->mySide, action); // 添加节点
    vv->availableActions = actions(vv->state);
    
    v->children.push_back(vv);
    //    printf("给v增加儿子\n");
    //    printf("儿子的action %d,%d\n", action.x, action.y);
    return vv;
}

Node* UCTStrategy::bestChild(Node* v, double c) {
    //    printf("bestChild\n");
    assert(v->children.size() > 0);
    
    Node* candid = nullptr;
    double max_confidnece = INT_MIN;
    for (int i = 0; i < v->children.size(); i++) {
        Node* child = v->children[i];
        //        printf("遍历儿子 %d, %d\n", child->action.x, child->action.y);
        double a = static_cast<double>(child->reward)/child->visited;
        double b = 2*log(v->visited)/child->visited;
        double confidence = a + c*sqrt(b);
        //        printf("a: %f, b:%f, confidence %f, coefficient %f\n", a, b, confidence, c);
        if (confidence > max_confidnece) {
            max_confidnece = confidence;
            candid = child;
        }
    }
    //    printf("best儿子的action %d,%d\n", candid->action.x, candid->action.y);
    return candid;
}

int UCTStrategy::defaultPolicy(Node* v) {
    //        printf("defaultPolicy\n");
    Node* vv = new Node(nullptr, v->state, v->mySide, v->action);
    int** vstate = new int*[M];
    for(int i = 0; i < M; i++) {
        vstate[i] = new int[N];
        std::memcpy(vstate[i], v->state[i], N*sizeof(int));
    }
    vv->state = vstate;
    
    UCTStrategy::GameState gs = getGameState(vv);
    while (gs == PLAYING) {
        std::vector<Point> availableActions = actions(vv->state);
        int idx = rand()%availableActions.size();
        // 以等概率选择行动a in A(s)
        vv->action = availableActions[idx];
        int** newState = performAction(vv->state, vv->action, (vv->mySide ? 2 : 1));
        
        for (int i = 0; i < M; i++) {
            delete[] vv->state[i];
        }
        delete vv->state;
        
        vv->state = newState;
        vv->mySide = !vv->mySide;
        gs = getGameState(vv);
    }
    //    printf("gs: %d\n", gs);
    assert(gs != PLAYING);
    int reward = 0;
    switch (gs) {
        case COMPUTER_WIN:
            reward = -2;
            break;
        case USER_WIN:
            reward = 2;
            break;
        default:
            reward = 0;
    }
    return reward;
}

void UCTStrategy::backup(Node* v, int delta) {
    //    printf("backup %d,%d : %d\n", v->action.x, v->action.y, delta);
    while (v != nullptr) {
        v->visited += 1;
        v->reward += delta;
        delta = -delta;
        v = v->parent;
    }
}

bool UCTStrategy::hasTimeout(Time& start) {
    Time now = std::chrono::system_clock::now();
    bool result = (std::chrono::duration_cast<std::chrono::microseconds>(now - start).count() > timeout);
    //if(result) printf("timeout!\n");
    return result;
}

bool UCTStrategy::isNodeTerminal(Node* v) {
    GameState state = getGameState(v);
    //    printf("is node terminal? %d\n", state);
    return state != PLAYING;
}

std::vector<Point> UCTStrategy::actions(int** s) {
    std::vector<Point> acts;
    for (int y = 0; y < N; y++) {
        Point p;
        p.y = y;
        p.x = M-1;
        for (int x = 0; x < M; x++) {
            if (s[x][y] != 0) {
                p.x = x-1;
                break;
            }
        }
        if (p.y == noY && p.x == noX) {
            p.x -= 1;
        }
        if (p.x >= 0 && p.x < M && p.y >= 0 & p.y < N) {
            acts.push_back(p);
        }
    }
    return acts;
}

int** UCTStrategy::performAction(int** s0, Point action, int pawn) {
    assert(s0[action.x][action.y] == 0);
    assert(!(noX == action.x && noY == action.y && pawn==2) );
    int** board = new int*[M];
    for(int i = 0; i < M; i++) {
        board[i] = new int[N];
        std::memcpy(board[i], s0[i], N*sizeof(int));
    }
    board[action.x][action.y] = pawn;
    return board;
}

UCTStrategy::GameState UCTStrategy::getGameState(Node* v) {
    if (v->action.x == -1) return PLAYING;
    if(machineWin(v->action.x, v->action.y, M, N, v->state) || userWin(v->action.x, v->action.y, M, N, v->state)) {
        //        printBoard(v->state);
        GameState gs = (v->mySide ? USER_WIN : COMPUTER_WIN);
        return gs;
    }
    
    bool tie = actions(v->state).size() == 0;
    if (tie) return TIE;
    
    return PLAYING;
}


void UCTStrategy::printBoard(int **board) {
    for (int x= 0 ; x < M; x++) {
        for (int y = 0; y < N; y++) {
            printf("%d", board[x][y]);
        }
        printf("\n");
    }
}
