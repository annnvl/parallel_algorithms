//
//  lock_free_queue.hpp
//  Lock-free Queue
//
//  Created by No_love_no_problem on 07.05.16.
//  Copyright © 2016 Anna Vlasova. All rights reserved.
//

#ifndef lock_free_queue_hpp
#define lock_free_queue_hpp

#include <iostream>
#include "hazard_pointer.hpp"

template <class T>
class lock_free_queue {
public:
    lock_free_queue():head(nullptr), tail(nullptr){};
    ~lock_free_queue();
    void enqueue(T item);
    bool dequeue(T& item);
private:
    
    struct node{
        T* data;
        node* next;
        node(T const& _data): data(_data), next(nullptr){};
    };
    
    std::atomic<node*> head;
    std::atomic<node*> tail;
    
    hazard_pointer<node> HP;
};

template <class T>
lock_free_queue<T>::~lock_free_queue(){
    node* current = head.exchange(nullptr);
    while(current){
        node* next = current->next;
        delete current;
        current = next;
    }
}

template <class T>
void lock_free_queue<T>::enqueue(T item) {
    node* new_node = new node(item);
    new_node->next = head.load();
    while(!head.compare_exchange_weak(new_node->next,new_node));
}


template <class T>
bool lock_free_queue<T>::dequeue(T& item){
    //получаем hazard pointer для данного потока
    std::atomic<T*>& hp = HP.get_hp();
    
    node* old_head=head.load();
    do{
        T* temp;
        do{
            //установка hazard pointer на удаляемый узел
            temp=old_head;
            hp.store(old_head);
            old_head=head.load();
        } while(old_head!=temp);
    }while(old_head && !head.compare_exchange_strong(old_head,old_head->next)); //в данном случае ложный отказ будет стоить слишком много лишних операций
    
    if(old_head){
        item = old_head->data;
        //проверяем, можем ли мы удалить узел
        if(find_hp(old_head)){
            delete_later(old_head);
        } else{
            delete old_head;
        }
        HP.delete_data();
        //на всякий случай, если узел не был удален, чтобы мы не мешали никому его удалить
        hp.store(nullptr);
        return 1;
    }
    return 0;
};

#endif /* lock_free_queue_hpp */
