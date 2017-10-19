//
//  Blockingqueue.hpp
//  ThreadPool
//
//  Created by No_love_no_problem on 15.03.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef Blockingqueue_hpp
#define Blockingqueue_hpp
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <atomic>

template <class T>
class Blockingqueue {
public:
    Blockingqueue():_shitdown(false){};
    Blockingqueue(const Blockingqueue&) = delete;
    void enqueue(const T & item);
    void pop(T& item);
    void shutdown();
    bool is_shut_down(){return _shitdown.load()};
private:
    std::queue<T> _data;
    std::mutex _mtx;
    std::mutex _mtx_sd;
    std::condition_variable _not_empty;
    std::condition_variable _not_full;
    bool _shitdown;
};
//кладем
template <class T>
inline void Blockingqueue<T>::enqueue(const T & item) {
    std::unique_lock<std::mutex> lock(_mtx);
    _data.push(item);
    _not_empty.notify_all();
}
//достаем
template <class T>
inline void Blockingqueue<T>::pop(T &item) {
    std::unique_lock<std::mutex> lock(_mtx);
    while (_data.empty()) {
        if (_shitdown.load()) {
            exit(0);
        }
        _not_empty.wait(lock);
    }
    item = _data.front();
    _data.pop();
    _not_full.notify_all();
}
//убиваем всех
template <class T>
inline void Blockingqueue<T>::shutdown() {
    _shitdown.store(true);
}

#endif /* Blockingqueue_hpp */
