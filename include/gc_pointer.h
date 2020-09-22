#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
*/
template <class T, int size = 0>
class Pointer{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;
    T *addr = nullptr;
    bool isArray = false; 
    unsigned arraySize = 0;
    static bool first;
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;
    Pointer(){
        Pointer(NULL);
    }
    Pointer(T*);
    Pointer(const Pointer &);
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();
    T* operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    // Return a reference to the object pointed to by this Pointer.
    T& operator*(){
        return *addr;
    }
    // Return the address being pointed to.
    T* operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    T& operator[](int i){ return addr[i];}
    // Conversion function to T *.
    operator T *() { return addr; }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t){
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    typename std::list<PtrDetails<T> >::iterator p = findPtrInfo(t);
    if(p == refContainer.end()){
        PtrDetails<T> newPtr(t, size);
        if(size > 0){
            isArray = true;
            arraySize = size;
        }
        refContainer.push_back(newPtr);
    }else{
        p->refcount++;
    }
    this->addr = t;
}
// Copy constructor.
template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &ob){
    typename std::list<PtrDetails<T> >::iterator p = findPtrInfo(ob.addr);
    p->refcount++;
    this->isArray = ob.isArray;
    this->arraySize = ob.arraySize;   
	this->addr = ob.addr;
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer(){
    typename std::list<PtrDetails<T> >::iterator p = findPtrInfo(addr);
    p->refcount--;
    if(p->refcount == 0)collect();
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){
    bool memFreed = false;
    typename std::list<PtrDetails<T> >::iterator p = refContainer.begin();
    do{
        for(; p != refContainer.end(); p++){
            if(p->refcount > 0)continue;
            if(p->memPtr){
                if(p->isArray){
                    delete[] p->memPtr;
                }
                else delete p->memPtr;
                refContainer.erase(p--);
            }
        }
    }while (p != refContainer.end());
    
    return memFreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T* Pointer<T, size>::operator=(T *t){
    typename std::list<PtrDetails<T> >::iterator p = findPtrInfo(addr);
    p->refcount--;
    if(p->refcount == 0)collect();
    p = findPtrInfo(t);
    if(p == refContainer.end()){
        PtrDetails<T> newPtr(t, size);
        refContainer.push_back(newPtr);
    }else{
        p->refcount++;
    }
    if(size > 0){
        isArray = true;
        arraySize = size;
    }
    this->addr = t;
    return addr;
}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){
    if(*this == rv)return *this;
    typename std::list<PtrDetails<T> >::iterator p = findPtrInfo(addr);
    p->refcount--;
    if(p->refcount == 0)collect();
    p = findPtrInfo(rv.addr);
    p->refcount++;
	this->addr = rv.addr;
    this->isArray = rv.isArray;
    this->arraySize = rv.arraySize;
    return *this;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
	return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++){
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}