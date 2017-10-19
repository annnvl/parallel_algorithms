//
//  Stripted_hash_set.hpp
//  Stripted hash set
//
//  Created by No_love_no_problem on 16.04.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef Stripted_hash_set_hpp
#define Stripted_hash_set_hpp

#include <vector>
#include <forward_list>
#include <atomic>
#include "RW-mutex.hpp"

template <typename T, class H = std::hash<T> >
class Striped_hash_set {
public:
    //конструктор с параметрами - число потоков, коэффициент увеличения и коэффициент заполняемости
    Striped_hash_set(std::size_t num_stripes = 1, int growth_factor = 2, int load_factor = 5);
    
    //добавляет элемент в множество, если его там нет
    void add(const T& e);
    //удаляет элемент из множества, если он там есть
    void remove(const T& e);
    //проверяет, лежит ли элемент в множестве
    bool contains(const T& e);
    
private:
    //расширение таблицы
    void _extend();
    
    //параметры таблицы
    std::size_t _num_stripes;
    int _growth_factor;
    int _load_factor;
    
    //текущие параметры таблицы
    std::atomic<std::size_t> _current_table_size;
    std::atomic<std::size_t> _number_of_elem;
    double _current_load_factor(){return _number_of_elem.load()/_current_table_size.load();};
    std::mutex _size_change_mtx;
    
    //таблица
    std::vector<std::forward_list<T>> _table;
    std::vector<RW_mutex> _mtx;
};

template <typename T, class H >
Striped_hash_set<T, H>::Striped_hash_set(std::size_t num_stripes, int growth_factor, int load_factor):_num_stripes(num_stripes), _growth_factor(growth_factor), _load_factor(load_factor), _number_of_elem(0), _current_table_size(2*num_stripes), _table(2*num_stripes), _mtx(num_stripes){}

template <typename T, class H >
inline void Striped_hash_set<T, H>::add(const T& e){
    if (_current_load_factor() >= _load_factor) {
        _extend();
    }
    
    size_t key = H()(e);
    _mtx[key%_num_stripes].write_lock();
    int i = key%_current_table_size.load();
    auto elem = std::find(_table[i].begin(), _table[i].end(), e);
    if (elem ==  _table[i].end()) {
        _table[i].push_front(e);
    }
    
    _mtx[key%_num_stripes].write_unlock();
    
    _size_change_mtx.lock();
    _number_of_elem++;
    _size_change_mtx.unlock();

}

template <typename T, class H>
inline void Striped_hash_set<T, H>::remove(const T& e){
    size_t key = H()(e);
    _mtx[key%_num_stripes].write_lock();
    size_t i = key%_current_table_size.load();
    _table[i].remove(e);
    _mtx[key%_num_stripes].write_unlock();
    
    _size_change_mtx.lock();
    _number_of_elem--;
    _size_change_mtx.unlock();
}

template <typename T, class H >
inline bool Striped_hash_set<T, H>::contains(const T& e){
    size_t key = H()(e);
    _mtx[key%_num_stripes].read_lock();
    size_t i = key%_current_table_size.load();
    auto elem = std::find(_table[i].begin(), _table[i].end(), e);
    _mtx[key%_num_stripes].read_unlock();
    if (elem ==  _table[i].end()) {
        return false;
    }
    return true;
}

template <typename T, class H >
inline void Striped_hash_set<T, H>::_extend(){
    size_t oldsize = _current_table_size.load();
    _mtx[0].write_lock();
    if (oldsize!=_current_table_size.load()) {
        return;
    }
    
    for (int i = 1; i < _num_stripes; i++) {
        _mtx[i].write_lock();
    }
    
    _current_table_size.store(_current_table_size.load()*_growth_factor);
    
    std::vector<std::forward_list<T>> newdata(_current_table_size);
    for (int i = 0; i < oldsize; i++) {
        for (auto j = _table[i].begin(); j != _table[i].end(); ++j) {
            newdata[H()(*j)%_current_table_size.load()].push_front(*j);
        }
    }
    std::swap(_table, newdata);
    
    for (int i = 0; i < _num_stripes; i++) {
        _mtx[i].write_unlock();
    }
}


#endif /* Stripted_hash_set_hpp */
