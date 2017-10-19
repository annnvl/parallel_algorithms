//
//  tree_mutex.hpp
//  tree_mutex
//
//  Created by No_love_no_problem on 29.02.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef tree_mutex_hpp
#define tree_mutex_hpp
#include <vector>
#include "PetersonMutex.hpp"

class TournamentTreeMutex{
public:
    TournamentTreeMutex(std::size_t num_threads);
    //поток thread_index попытается зайти в крит секцию
    void lock(std::size_t thread_index);
    //освобождение критической секции занятым ее потоком(я не хочу делать с аргументом, чтобы не ходить туда-обратно, суть та же)
    void unlock();
private:
    std::vector<PetersonMutex> _tree;
    std::size_t _size;
    std::size_t _degof2;
};

#endif /* tree_mutex_hpp */
