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
}

bool UCTStrategy::valid() const {
    if (M == 0) return false;
    return true;
}

Point UCTStrategy::getPoint(const int* top, int** board,
                            const int lastX, const int lastY) {
    for (int i = 0; i < N; i++) {
        if (top[i] > 0) {
            return Point(top[i]-1, i);
        }
    }
    return Point(0, 0);
}

Point UCTStrategy::uctSearch(int** s0) {
    Time startTime = std::chrono::system_clock::now();
    Node* v0 = new Node(nullptr, s0);
    
    while (!hasTimeout(startTime)) {
        Node* vl = treePolicy(v0);
        ul delta = defaultPolicy(vl->state);
        backup(vl, delta);
    }
    return bestChild(v0, 0)->p;
}

Node* UCTStrategy::treePolicy(Node* v) {
    return nullptr;
}

Node* UCTStrategy::expand(Node* v) {
    return nullptr;
}

Node* UCTStrategy::bestChild(Node* v, double c) {
    return nullptr;
}

ul UCTStrategy::defaultPolicy(int** s) {
    return 0;
}
void UCTStrategy::backup(Node* v, ul delta) {
    return;
}

bool UCTStrategy::hasTimeout(Time& start) {
    Time now = std::chrono::system_clock::now();
    bool result = (std::chrono::microseconds(now - start).count() > timeout);
    return result;
}
