// This class defines an element that is stored
// in the garbage collection information list.
//
template <class T>
class PtrDetails{
  public:
    unsigned refcount;
    T *memPtr;
    bool isArray = false;
    unsigned arraySize;

    PtrDetails(T* newPtr, unsigned size = 0){
        refcount = 1;
        if(size > 0){
            isArray = true;
            arraySize = size;
        }
        memPtr = newPtr;
    }
};
// Overloading operator== allows two class objects to be compared.
template <class T>
bool operator==(const PtrDetails<T> &ob1, const PtrDetails<T> &ob2){
    if(ob1.memPtr == ob2.memPtr && ob1.arraySize == ob2.arraySize)return true;
    return false;
}