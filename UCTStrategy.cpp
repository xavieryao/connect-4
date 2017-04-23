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
    return uctSearch(board, lastX, lastY, top);
}

Point UCTStrategy::uctSearch(int** s0, int lastX, int lastY, const int* top) {
    //    printf("uctSearch Started\n");
    Time startTime = std::chrono::system_clock::now();
    
    Node* v0 = new Node(nullptr, s0, top, true, Point(lastX, lastY));
    // 以状态s0创建根节点v0
    
    int i = 0;
    
    while (!hasTimeout(startTime)) {
        Node* vl = treePolicy(v0);
        int delta = defaultPolicy(vl);
        backup(vl, delta);
        i ++;
    }
    
    printf("%d iterations\n", i);
    Point result = bestChild(v0, 0)->action;
    for (auto child :v0->children) {
        delete child;
    }
    return result;
}

Node* UCTStrategy::treePolicy(Node* v) {
    while (!isNodeTerminal(v)) { // while节点v不是终止节点
        if (v->availableActCnt > 0) { // if 节点v是可扩展的
            return expand(v);
        } else {
            v = bestChild(v, coefficient);
        }
    }
    return v;
}

Node* UCTStrategy::expand(Node* v) {
    assert(v->availableActCnt > 0);
    
    int idx = rand()%v->availableActCnt;
    auto y = v->availableActions[idx];
    auto action = Point(v->top[y], y);
    v->availableActions[idx] = v->availableActions[v->availableActCnt-1];
    v->availableActCnt--; // 随机选择尚未选择过的行动
    
    auto newState = performAction(v->state, action, (v->mySide ? 2 : 1));
    Node* vv = new Node(v, newState, v->top, !v->mySide, action); // 添加节点
    // update top
    vv->top[y] --;
    if (y == noY && vv->top[y] == noX) vv->top[y] --;
    
    updateActions(vv);
    
    v->children.push_back(vv);
    return vv;
}

Node* UCTStrategy::bestChild(Node* v, double c) {
    assert(v->children.size() > 0);
    
    Node* candid = nullptr;
    double max_confidnece = INT_MIN;
    for (int i = 0; i < v->children.size(); i++) {
        Node* child = v->children[i];
        double a = static_cast<double>(child->reward)/child->visited;
        
        if (v->mySide) a = -a;
        
        double b = 2*log(v->visited)/child->visited;
        double confidence = a + c*sqrt(b);
        
        if (confidence > max_confidnece || (confidence==max_confidnece && rand()%2)) {
            max_confidnece = confidence;
            candid = child;
        }
    }
    return candid;
}

int UCTStrategy::defaultPolicy(Node* v) {
    //        printf("defaultPolicy\n");
    Node* vv = new Node(nullptr, v->state, v->top, v->mySide, v->action);
    int** vstate = new int*[M];
    for(int i = 0; i < M; i++) {
        vstate[i] = new int[N];
        std::memcpy(vstate[i], v->state[i], N*sizeof(int));
    }
    vv->state = vstate;
    
    UCTStrategy::GameState gs = getGameState(vv);
    while (gs == PLAYING) {
        updateActions(vv);
        int idx = rand()%vv->availableActCnt;
        // 以等概率选择行动a in A(s)
        auto y = vv->availableActions[idx];
        vv->action = Point(vv->top[y], y);
        vv->state[vv->action.x][vv->action.y] = (vv->mySide ? 2 : 1);
        vv->top[y]--;
        if (vv->top[y] == noX && y == noY) vv->top[y]--;
        vv->mySide = !vv->mySide;
//        printBoard(vv->state);
        updateActions(vv);
        gs = getGameState(vv);
    }
    
    delete vv;
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
    while (v != nullptr) {
        v->visited += 1;
        v->reward += delta;
        //        delta = -delta;
        v = v->parent;
    }
}

bool UCTStrategy::hasTimeout(Time& start) {
    Time now = std::chrono::system_clock::now();
    bool result = now - start > std::chrono::milliseconds(1300);
    //if(result) printf("timeout!\n");
    return result;
}

bool UCTStrategy::isNodeTerminal(Node* v) {
    GameState state = getGameState(v);
    //    printf("is node terminal? %d\n", state);
    return state != PLAYING;
}

int** UCTStrategy::performAction(int** s0, Point action, int pawn) {
    if (!(s0[action.x][action.y] == 0)) {
        printBoard(s0);
    }
    assert(s0[action.x][action.y] == 0);
    assert(!(noX == action.x && noY == action.y) );
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
    
//    updateActions(v);
    bool tie = (actionCount(v) == 0);
    if (tie) {
        return TIE;
    }
    
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

void UCTStrategy::updateActions(Node* v) {
    v->availableActCnt = 0;
    for (int y = 0; y < _N; y++) {
        if (v->top[y] >= 0) v->availableActions[v->availableActCnt++] = y;
    }
}

int UCTStrategy::actionCount(Node* v) {
    int availableActCnt = 0;
    for (int y = 0; y < _N; y++) {
        if (v->top[y] >= 0) availableActCnt++;
    }
    return availableActCnt;
}
