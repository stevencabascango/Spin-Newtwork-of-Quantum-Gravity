#pragma once
#include <memory>
#include <utility>
#include <cstddef>
#include <iterator>
#include <type_traits>
namespace snl {
template<class T> class SinglyLinkedList {
    struct Node { T value; std::unique_ptr<Node> next; explicit Node(const T& v):value(v){} };
    std::unique_ptr<Node> head_; Node* tail_=nullptr; std::size_t size_=0;
public:
    template<bool Const> class Iterator {
        using Pointer=typename std::conditional<Const,const Node*,Node*>::type;
        Pointer node_;
    public:
        using iterator_category=std::forward_iterator_tag;
        using value_type=T; using difference_type=std::ptrdiff_t;
        using reference=typename std::conditional<Const,const T&,T&>::type;
        using pointer=typename std::conditional<Const,const T*,T*>::type;
        explicit Iterator(Pointer n=nullptr):node_(n){}
        reference operator*() const {return node_->value;}
        pointer operator->() const {return &node_->value;}
        Iterator& operator++(){node_=node_->next.get();return *this;}
        Iterator operator++(int){auto old=*this;++*this;return old;}
        bool operator!=(const Iterator& o)const{return node_!=o.node_;}
        bool operator==(const Iterator& o)const{return node_==o.node_;}
    };
    SinglyLinkedList()=default;
    ~SinglyLinkedList(){clear();}
    SinglyLinkedList(const SinglyLinkedList& o){SinglyLinkedList tmp;for(const auto& v:o)tmp.pushBack(v);swap(tmp);}
    SinglyLinkedList(SinglyLinkedList&& o) noexcept {swap(o);}
    SinglyLinkedList& operator=(SinglyLinkedList o){swap(o);return *this;}
    void swap(SinglyLinkedList& o) noexcept {head_.swap(o.head_);std::swap(tail_,o.tail_);std::swap(size_,o.size_);}
    void pushBack(const T& v){auto n=std::make_unique<Node>(v);auto raw=n.get();if(tail_)tail_->next=std::move(n);else head_=std::move(n);tail_=raw;++size_;}
    void pushFront(const T& v){auto n=std::make_unique<Node>(v);if(!tail_)tail_=n.get();n->next=std::move(head_);head_=std::move(n);++size_;}
    T* find(const T& v){for(auto& item:*this)if(item==v)return &item;return nullptr;}
    const T* find(const T& v)const{for(const auto& item:*this)if(item==v)return &item;return nullptr;}
    bool contains(const T& v)const{return find(v)!=nullptr;}
    bool remove(const T& v){return removeIf([&](const T& item){return item==v;});}
    template<class Predicate> bool removeIf(Predicate pred){auto link=&head_;Node* previous=nullptr;while(*link){if(pred((*link)->value)){auto removed=std::move(*link);*link=std::move(removed->next);if(tail_==removed.get())tail_=previous;--size_;return true;}previous=link->get();link=&(*link)->next;}return false;}
    void clear() noexcept {while(head_){auto n=std::move(head_);head_=std::move(n->next);}tail_=nullptr;size_=0;}
    std::size_t size()const noexcept{return size_;}
    bool empty()const noexcept{return size_==0;}
    auto begin(){return Iterator<false>(head_.get());} auto end(){return Iterator<false>(nullptr);}
    auto begin()const{return Iterator<true>(head_.get());} auto end()const{return Iterator<true>(nullptr);}
};
}
