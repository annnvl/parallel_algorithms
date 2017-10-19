//
//  main.cpp
//  ThreadPool
//
//  Created by No_love_no_problem on 15.03.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>
#include "Blockingqueue.hpp"

void produce(Blockingqueue<int>& tasks){
    for (int i = 1; i < 32000; i++) {
        tasks.enqueue(i);
    }
    tasks.shutdown();
}

void consume(Blockingqueue<int>& tasks, std::mutex& writemtx, int num){
    srand((unsigned int)time(NULL));
    while (true) {
        int mod;
        tasks.pop(mod);
        writemtx.lock();
        std::cout << mod << " " << rand()%mod << " " << num <<"\n";
        writemtx.unlock();
    }
}
int main() {
    Blockingqueue<int> tasks(100);
    std::mutex writemtx;
    std::thread producer(produce, std::ref(tasks));
    std::vector<std::thread> consumers;
    for (int i = 0; i < 10; i++) {
        consumers.emplace_back(consume, std::ref(tasks), std::ref(writemtx), i);
    }
    for (int i = 0; i < 10; i++) {
        consumers[i].join();
    }
    producer.join();
    return 0;
}
