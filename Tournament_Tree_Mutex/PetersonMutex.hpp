//
//  PetersonMutex.hpp
//  TournamentTreeMutex
//
//  Created by No_love_no_problem on 29.02.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef PetersonMutex_hpp
#define PetersonMutex_hpp

#include <atomic>
#include <array>
#include <thread>

class PetersonMutex {
public:
    PetersonMutex();
    //захват
    void lock(int t);
    //освобождение
    void unlock(int t) {_want[t].store(false);}
    //возвращает номер потока, который владеет мьютексом - нужно для турнира
    int wholocked();
private:
    std::array<std::atomic<bool>, 2> _want;
    std::atomic<int> _victim;
};

inline PetersonMutex::PetersonMutex() {
    _want[0].store(false);
    _want[1].store(false);
    _victim.store(0);
}

inline void PetersonMutex::lock(int t) {
    _want[t].store(true);
    _victim.store(t);
    while (_want[1 - t].load() && _victim.load() == t) {
        std::this_thread::yield();
    }
}

inline int PetersonMutex::wholocked(){
    if (_want[1-_victim]) {
        return 1 - _victim;
    }
    return _victim;
}

#endif /* PetersonMutex_hpp */
