/* This file is part of FineFramework project */
#ifndef FFW_SWAP_WRAPPER
#define FFW_SWAP_WRAPPER
#include "config.h"
#include <memory>

namespace ffw{
    template<class T> class SwapWrapper {
    public:
        std::unique_ptr<T> ptr;
        
        template <typename... Args>
        SwapWrapper(Args&&... args):
            ptr(new T( std::forward<Args>(args)... )) {
            
        }
        
        SwapWrapper(SwapWrapper&& other):ptr(nullptr) {
            swap(other);
        }
        
        SwapWrapper& operator = (SwapWrapper&& other) {
            if(this != &other){
                swap(other);
            }
            return *this;
        }
        
        void swap(SwapWrapper<T>& other){
            std::swap(ptr, other.ptr);
        }
        
        T& operator * () {
            return *ptr;
        }
        
        const T& operator * () const {
            return *ptr;
        }
        
        T* operator -> () {
            return ptr.get();
        }
        
        const T* operator -> () const {
            return ptr.get();
        }
    };
};

template<class T>
void swap(ffw::SwapWrapper<T>& first, ffw::SwapWrapper<T>& second){
    first.swap(second);
}

#endif