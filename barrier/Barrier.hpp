//
//  Barrier.hpp
//  Barrier
//
//  Created by No_love_no_problem on 04.04.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef Barrier_hpp
#define Barrier_hpp

#include <mutex>
#include <condition_variable>

class Barrier {
public:
    //конструктор по числу потоков
    Barrier(std::size_t numthreads):_numThreads(numthreads), _coming(0), _inside(0){};
    
    //подходим к барьеру и ждем остальных
    void enter();
    
private:
    //общее число потоков
    std::size_t _numThreads;
    
    //первые ворота - на вход
    std::condition_variable _cvFirstDoors;
    
    //вторые ворота - на выход
    std::condition_variable _cvSecondDoors;
    std::mutex _mtx;
    
    //число потоков, которые хотят пройти через барьер
    std::size_t _coming;
    
    //число потоков, находящихся внутри барьера (перед 2ми воротами)
    std::size_t _inside;
};

inline void Barrier::enter(){
    std::unique_lock<std::mutex> lock(_mtx);
    
    //пока кто-то есть внутри, заходить нельзя
    while (_inside) {
        _cvFirstDoors.wait(lock);
    }
    
    //заходим
    _coming++;
    
    //это значит, что пришли все
    if (_coming == _numThreads) {
        _cvSecondDoors.notify_all();
        _inside = _numThreads;
    }
    
    //ждем остальных
    while (_coming < _numThreads) {
        _cvSecondDoors.wait(lock);
    }
    
    //выходим
    _inside--;
    
    //если все вышли, первые ворота можно открыть
    if (!_inside) {
        _coming = 0;
        _cvFirstDoors.notify_all();
    }
}

#endif /* Barrier_hpp */
