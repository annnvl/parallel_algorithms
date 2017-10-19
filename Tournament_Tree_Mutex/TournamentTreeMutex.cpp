//
//  TournamentTreeMutex.cpp
//  TournamentTreeMutex
//
//  Created by No_love_no_problem on 29.02.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#include "TournamentTreeMutex.hpp"
#include <cmath>
#include <exception>
#include <iostream>

TournamentTreeMutex::TournamentTreeMutex(std::size_t num_threads):_size(num_threads){
    //вычисление размера дерева
    int buflog = ceil(log2(_size));
    //ближайшая степень двойки (>=)
    _degof2 = pow(2, buflog);
    _tree = std::vector<PetersonMutex>(_degof2-1);
}

void TournamentTreeMutex::lock(std::size_t thread_index){
    if (thread_index >=_size) {
        throw std::invalid_argument("invalid thread_index");
    }
    std::size_t index = _degof2 + thread_index - 1;
    while (index) {
        index--;
        bool num = (index) % 2;
        index/=2;
        _tree[index].lock(num);
    }
}

void TournamentTreeMutex::unlock(){
    std::size_t index = 0;
    while (index < _degof2 - 1) {
        //куда идти дальше
        bool who = _tree[index].wholocked();
        _tree[index].unlock(who);
        index*=2;
        index++;
        index+=who;
    }
}
