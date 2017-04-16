//
//  UCTStrategy.cpp
//  Strategy
//
//  Created by 沛然 姚 on 2017/4/16.
//  Copyright © 2017年 Yongfeng Zhang. All rights reserved.
//

#include "UCTStrategy.h"

UCTStrategy::UCTStrategy(int m, int n, int noX, int noY) {
    this->M = m;
    this->N = n;
    this->noX = noX;
    this->noY = noY;
    
    srand(time(0));
}

bool UCTStrategy::valid() const {
    if (M == 0) return false;
    return true;
}

Point UCTStrategy::getPoint(const int* top, int** board,
                            const int lastX, const int lastY) {
    return uctSearch(board, lastX, lastY);
}

Point UCTStrategy::uctSearch(int** s0, int lastX, int lastY) {
    Time startTime = std::chrono::system_clock::now();
    Node* v0 = new Node(nullptr, s0, false, Point(lastX, lastY));
    v0->availableActions = actions(s0);
    
    while (!hasTimeout(startTime)) {
        Node* vl = treePolicy(v0);
        ul delta = defaultPolicy(vl);
        backup(vl, delta);
    }
    return bestChild(v0, 0)->action;
}

Node* UCTStrategy::treePolicy(Node* v) {
    while (!isNodeTerminal(v)) { // while节点v不是终止节点
        if (v->availableActions.size() > 0) { // if 节点v是可扩展的
            return expand(v);
        } else {
            v = bestChild(v, coefficient);
        }
    }
    return v;
}

Node* UCTStrategy::expand(Node* v) {
    auto action = v->availableActions.back();
    v->availableActions.pop_back(); // 选择尚未选择过的行动
    auto newState = performAction(v->state, action);
    Node* vv = new Node(v, newState, !v->mySide, action); // 添加节点
    v->childern.push_back(vv);
    return vv;
}

Node* UCTStrategy::bestChild(Node* v, double c) {
    Node* candid = nullptr;
    double max_confidnece = LONG_MIN;
    for (auto child : v->childern) {
        double confidence = child->reward/child->visited + c*sqrt(2*log(v->visited)/child->visited);
        if (confidence > max_confidnece) {
            max_confidnece = confidence;
        }
        candid = child;
    }
    return candid;
}

ul UCTStrategy::defaultPolicy(Node* v) {
    Node* vv = v;
    UCTStrategy::GameState gs = PLAYING;
    while (gs == PLAYING) {
        std::vector<Point> availableActions = actions(vv->state);
        int idx = rand()%availableActions.size();
        vv->action = availableActions[idx];
        vv->state = performAction(vv->state, vv->action);
        vv->mySide = !vv->mySide;
    }
    
    int reward = 0;
    switch (gs) {
        case COMPUTER_WIN:
            reward = 1;
            break;
        case USER_WIN:
            reward = -1;
        default:
            reward = 0;
    }
    return reward;
}

void UCTStrategy::backup(Node* v, ul delta) {
    while (v != nullptr) {
        v->visited += 1;
        v->reward += delta;
        delta = -delta;
        v = v->parent;
    }
}

bool UCTStrategy::hasTimeout(Time& start) {
    Time now = std::chrono::system_clock::now();
    bool result = (std::chrono::microseconds(now - start).count() > timeout);
    return result;
}

bool UCTStrategy::isNodeTerminal(Node* v) {
    return getGameState(v) != PLAYING;
}

std::vector<Point> UCTStrategy::actions(int** s) {
    std::vector<Point> acts;
    for (int y = 0; y < N; y++) {
        Point p;
        p.y = y;
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

int** UCTStrategy::performAction(int** s0, Point action) {
    int** board = new int*[M];
    for(int i = 0; i < M; i++) {
        board[i] = new int[N];
        std::memcpy(board[i], s0[i], N*sizeof(int));
    }
    return board;
}

UCTStrategy::GameState UCTStrategy::getGameState(Node* v) {
    if(v->mySide && machineWin(v->action.x, v->action.y, M, N, v->state))
        return COMPUTER_WIN;
    if (!v->mySide && userWin(v->action.x, v->action.y, M, N, v->state))
        return USER_WIN;
    
    bool tie = true;
    for (int i = 0; i < N; i++) {
        if (v->state[0][i] == 0 && i != noY && 0 != noX) {
            tie = false;
        }
    }
    
    return tie ? TIE : PLAYING;
}
