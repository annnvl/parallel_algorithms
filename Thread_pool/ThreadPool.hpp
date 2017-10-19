//
//  ThreadPool.hpp
//  ThreadPool
//
//  Created by No_love_no_problem on 29.03.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef ThreadPool_hpp
#define ThreadPool_hpp

#include "Blockingqueue.hpp"
#include <future>
#include <functional>
#include <vector>
#include <thread>

template <class T>
class ThreadPool {
public:
    //конструктор с числом потоков по умолчанию
    ThreadPool();
    //конструктор с явно заданным числом потоков
    ThreadPool(std::size_t num_threads);
    //поместить запрос на выполнение ф-ции в пул потоков
    std::future<T> submit(std::function<T()> func);
    //убить всех
    void shut_down();
    
    ~ThreadPool();
private:
    std::size_t _default_num_workers();
    std::size_t _num_workers;
    std::vector<std::thread> _workers;
    Blockingqueue<std::pair<std::promise<T>, std::function<T()>>> _tasksqueue;
};

template <class T>
void ThreadPool<T>::shut_down() {
    if(!_tasksqueue.is_shut_down()){
        _tasksqueue.shutdown();
        for (auto t:_workers) {
            t.join();
        }
    }
}

template <class T>
ThreadPool<T>::ThreadPool():ThreadPool(_default_num_workers()) {}

template <class T>
ThreadPool<T>::ThreadPool(std::size_t num_threads) : _tasksqueue(), _num_workers(num_threads){
    for (int i = 0; i < _num_workers; i++) {
            _workers.push_back(std::thread([this]() -> void {
                while (true) {
                    std::pair<std::promise<T>, std::function<T()>> obj;
                    _tasksqueue.pop(obj);
                    obj.first.set_value(obj.second());
                }
        }));
    }
}

template <class T>
std::future<T> ThreadPool<T>::submit(std::function<T()> func) {
    std::promise<T> mypromise;
    std::future<T> myfuture = mypromise.get_future();
    _tasksqueue.enqueue(std::make_pair(move(mypromise), func));
    return myfuture;
}

template <class T>
ThreadPool<T>::~ThreadPool() {
    shut_down();
}

template <class T>
std::size_t ThreadPool<T>::_default_num_workers() {
    std::size_t n = (size_t)std::thread::hardware_concurrency;
    return n ? n : 2;
}

#endif /* ThreadPool_hpp */
