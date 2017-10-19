//
//  main.cpp
//  ThreadPool
//
//  Created by No_love_no_problem on 15.03.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#include <iostream>
#include "ThreadPool.hpp"

int f(){
    return 7;
}

int main() {
    ThreadPool<int> tp;
    std::future<int> k = tp.submit(f);
    std::cout << k.get();
    return 0;
}
