//
//  RW-mutex.hpp
//  Stripted hash set
//
//  Created by No_love_no_problem on 16.04.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef RW_mutex_hpp
#define RW_mutex_hpp

#include <mutex>

class RW_mutex {
public:
    RW_mutex():readers(0){};
    
    void write_lock();
    void read_lock();
    
    void write_unlock();
    void read_unlock();
private:
    std::mutex turnstile;
    std::mutex ring;
    std::mutex lightswitch;
    std::size_t readers;
};

#endif /* RW_mutex_hpp */
