#ifndef SPSC_RING_BUFFER
#define SPSC_RING_BUFFER

#include <iostream>
#include <atomic>
#include <vector>

template <class T>
class spsc_ring_buffer {
public:
    //конструктор с параметром - размером буфера
    explicit spsc_ring_buffer(size_t size);
    //довавить элемент в очередь, вернуть успешно ли
    bool enqueue(T e);
    //достать элемент из очереди, вернут успешно ли
    bool dequeue(T& e);
private:
    
    const std::size_t _size;
    struct node_t{
        node_t(T el):data(el){};
        T data;
        //чтобы попали в разные кэш-линии
        char pad[128];
    };
    std::atomic<std::size_t> _head;
    //чтобы попали в разные кэш-линии
    char pad[128];
    
    std::atomic<std::size_t> _tail;
    
    std::vector<node_t> _data;
};

template <class T>
spsc_ring_buffer<T>::spsc_ring_buffer(size_t size):_size(size+1), _head(0),_tail(0),_data(size+1, node_t(0)){}

template <class T>
bool spsc_ring_buffer<T>::enqueue(T e) {
    size_t curr_head = _head.load(std::memory_order_acquire);
    size_t curr_tail = _tail.load(std::memory_order_relaxed);
    
    if(curr_head == (curr_tail+1)%_size){
        return 0;
    }
    
    _data[curr_tail] = node_t(e);
    curr_tail = (curr_tail + 1)%_size;
    
    _tail.store(curr_tail, std::memory_order_release);
    return 1;
}

template <class T>
bool spsc_ring_buffer<T>::dequeue(T& e) {
    size_t curr_head = _head.load(std::memory_order_relaxed);
    size_t curr_tail = _tail.load(std::memory_order_acquire);
    
    if(curr_head == curr_tail){
        return 0;
    }
    
    e = _data[curr_head].data;
    curr_head = (curr_head + 1)%_size;
    
    _head.store(curr_head, std::memory_order_release);
    return 1;
}

#endif
