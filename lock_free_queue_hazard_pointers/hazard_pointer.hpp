//
//  hazard_pointer.hpp
//  Lock-free Queue
//
//  Created by No_love_no_problem on 16.05.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef hazard_pointer_hpp
#define hazard_pointer_hpp

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <cassert>

template<class T>
class hazard_pointer {
public:
    hazard_pointer(size_t maxhp=50):MAX_HAZARD_POINTERS(maxhp), _pointers(maxhp, std::this_thread::get_id()), data_to_reclaim(nullptr){};
    ~hazard_pointer();
    
    //возвращает ХП для потока, в котором вызывается
    std::atomic<T*>& get_hp();
    
    //проверяет указывает ли на этот объект какой-то hp
    bool find_hp(T* p);
    
    //поместить в список на удаление
    void delete_later(T* e);
    
    //удалить все ненужное
    void delete_data();
private:
    //максимальное число указателей опасности
    const size_t MAX_HAZARD_POINTERS;
    
    //сам указатель опасности
    struct hp{
        hp(std::thread::id tid):_pointer(nullptr), _id(tid){};
        std::atomic<std::thread::id> _id;
        std::atomic<T*> _pointer;
    };
    //структура для хранения данных на удаление
    struct to_delete{
        T* data;
        to_delete* next;
        to_delete(T* pointer):data(pointer), next(nullptr){};
    };
    
    //список вершин на удаление
    std::atomic<to_delete*> data_to_reclaim;
    
    //здесь хранятся hazard pointers для всех потоков
    std::vector<hp> _pointers;
    
};

template <class T>
hazard_pointer<T>::~hazard_pointer(){
    to_delete* current=data_to_reclaim.exchange(nullptr);
    while(current){
        to_delete* next = current->next;
        delete current;
        current = next;
    }
}


template<class T>
std::atomic<T*>& hazard_pointer<T>::get_hp(){
    size_t i = 0;
    //пытаемся захватить указатель
    while(!(_pointers[i]._id.load() == std::this_thread::get_id())){
        std::thread::id old_id;
        if(_pointers[i]._id.compare_exchange_strong(old_id, std::this_thread::get_id())){
            break;
        }
        i++;
        //значит слишком мало указателей
        assert(i != MAX_HAZARD_POINTERS);
    }
    return _pointers[i]._pointer;
}


template<class T>
bool hazard_pointer<T>::find_hp(T *p){
    for (int i = 0; i < MAX_HAZARD_POINTERS; i++) {
        if (_pointers[i]._pointer.load() == p) {
            return true;
        }
    }
    return false;
}

template <class T>
void hazard_pointer<T>::delete_later(T *e) {
    //просто добавляем в список на удаление
    to_delete* node = new to_delete(e);
    node->next = data_to_reclaim.load();
    while(!data_to_reclaim.compare_exchange_weak(node->next,node));
}

template <class T>
void hazard_pointer<T>::delete_data() {
    //присматриваем список на удаление, предварительно захватив его себе
    to_delete* current=data_to_reclaim.exchange(nullptr);
    while(current){
        to_delete* next = current->next;
        if(find_hp(current->data)){
            delete current;
        } else{
            current->next = data_to_reclaim.load();
            while(!data_to_reclaim.compare_exchange_weak(current->next,current));
        }
        current = next;
    }
}

#endif /* hazard_pointer_hpp */
