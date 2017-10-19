//
//  RW-mutex.cpp
//  Stripted hash set
//
//  Created by No_love_no_problem on 16.04.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#include "RW-mutex.hpp"

void RW_mutex::write_lock(){
    turnstile.lock();
    ring.lock();
    turnstile.unlock();
}

void RW_mutex::read_lock(){
    turnstile.lock();
    turnstile.unlock();
    lightswitch.lock();
    readers ++;
    if (readers == 1){
        ring.lock();
    }
    lightswitch.unlock();
}


void RW_mutex::write_unlock(){
    ring.unlock();
}

void RW_mutex::read_unlock(){
    lightswitch.lock();
    readers --;
    if (!readers){
        ring.unlock();
    }
    lightswitch.unlock();
}