#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <utility>
namespace snl {
struct Fnv1a64 {
    static std::uint64_t bytes(const unsigned char* p,std::size_t n){std::uint64_t h=14695981039346656037ull;for(std::size_t i=0;i<n;++i){h^=p[i];h*=1099511628211ull;}return h;}
    std::uint64_t operator()(const std::string& s)const{return bytes(reinterpret_cast<const unsigned char*>(s.data()),s.size());}
    template<class K> std::uint64_t operator()(K key)const {
        static_assert(std::is_integral<K>::value && !std::is_same<K,bool>::value,"FNV supports integral keys or std::string");
        using U=typename std::make_unsigned<K>::type;U v=static_cast<U>(key);unsigned char b[sizeof(K)];
        for(std::size_t i=0;i<sizeof(K);++i){b[i]=static_cast<unsigned char>(v&255);v>>=8;}return bytes(b,sizeof(K));
    }
};
template<class K,class V,class Hash=Fnv1a64> class HashTable {
    struct Node {K key;std::unique_ptr<V> value;std::unique_ptr<Node> next;Node(const K& k,const V& v):key(k),value(std::make_unique<V>(v)){} };
    std::vector<std::unique_ptr<Node>> buckets_;std::size_t size_=0;Hash hash_;
    std::size_t index(const K& k)const{return hash_(k)%buckets_.size();}
public:
    explicit HashTable(std::size_t capacity=16,Hash h=Hash{}):buckets_(capacity?capacity:1),hash_(std::move(h)){}
    ~HashTable(){clear();}
    HashTable(const HashTable& o):HashTable(o.buckets_.size(),o.hash_){for(const auto& b:o.buckets_)for(auto n=b.get();n;n=n->next.get())insert(n->key,*n->value);}
    HashTable(HashTable&& o):HashTable(1){swap(o);}
    HashTable& operator=(HashTable o){swap(o);return *this;}
    void swap(HashTable& o) noexcept(noexcept(std::swap(hash_,o.hash_))){buckets_.swap(o.buckets_);std::swap(size_,o.size_);std::swap(hash_,o.hash_);}
    V* find(const K& k){for(auto n=buckets_[index(k)].get();n;n=n->next.get())if(n->key==k)return n->value.get();return nullptr;}
    const V* find(const K& k)const{for(auto n=buckets_[index(k)].get();n;n=n->next.get())if(n->key==k)return n->value.get();return nullptr;}
    bool contains(const K& k)const{return find(k)!=nullptr;}
    bool insert(const K& k,const V& v){if(contains(k))return false;auto n=std::make_unique<Node>(k,v);if((size_+1)>buckets_.size()*0.75)rehash(buckets_.size()*2);auto i=index(k);n->next=std::move(buckets_[i]);buckets_[i]=std::move(n);++size_;return true;}
    bool remove(const K& k){auto link=&buckets_[index(k)];while(*link){if((*link)->key==k){auto n=std::move(*link);*link=std::move(n->next);--size_;return true;}link=&(*link)->next;}return false;}
    // Rebuild transactionally: failed allocations leave the original table intact.
    void rehash(std::size_t capacity){if(!capacity)throw std::invalid_argument("Zero bucket count");HashTable tmp(capacity,hash_);for(const auto& b:buckets_)for(auto n=b.get();n;n=n->next.get())tmp.insert(n->key,*n->value);swap(tmp);}
    template<class F> void forEach(F f){for(auto& b:buckets_)for(auto n=b.get();n;n=n->next.get())f(n->key,*n->value);}
    template<class F> void forEach(F f)const{for(const auto& b:buckets_)for(auto n=b.get();n;n=n->next.get())f(n->key,static_cast<const V&>(*n->value));}
    void clear() noexcept {for(auto& b:buckets_)while(b){auto n=std::move(b);b=std::move(n->next);}size_=0;}
    std::size_t size()const noexcept{return size_;}std::size_t bucketCount()const noexcept{return buckets_.size();}
    std::size_t capacity()const noexcept{return bucketCount();}
    double loadFactor()const noexcept{return double(size_)/buckets_.size();}
    std::size_t bucketSize(std::size_t i)const{std::size_t n=0;for(auto b=buckets_.at(i).get();b;b=b->next.get())++n;return n;}
};
}
