//
//  main.cpp
//  Barrier
//
//  Created by No_love_no_problem on 04.04.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#include <iostream>
#include <vector>
#include <thread>
#include "Barrier.hpp"

int main() {
    std::vector<std::thread> threads;
    Barrier bar(10);
    for (int i = 0; i < 10; i++) {
        threads.push_back(std::thread([&bar]() -> void {
            int j = 0;
            while (j < 100) {
                std::cout << j;
                j++;
                bar.enter();
            }
        }));
    }
    for (int i = 0; i < 10; i++) {
        threads[i].join();
    }
}
