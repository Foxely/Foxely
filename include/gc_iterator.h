// To be thrown when we have errors
class OutOfRangeExc
{ 

};
// An iterator-like class for cycling through arrays
template <class T>
class Iter{
    T *ptr;
    T *end;
    T *begin;
    unsigned length;
  public:
    Iter(){
        ptr = end = begin = nullptr;
        length = 0;
    }
    Iter(T *p, T *first, T *last){
        ptr = p;
        end = last;
        begin = first;
        length = last - first;
    }
    // Return length of sequence to which this
    unsigned size() { return length; }
    // Return value pointed to by ptr.
    T& operator*(){
        if ((ptr >= end) || (ptr < begin))
            throw OutOfRangeExc();
        return *ptr;
    }
    // Return address contained in ptr.
    T *operator->(){
        if ((ptr >= end) || (ptr < begin))
            throw OutOfRangeExc();
        return ptr;
    }
    // Prefix ++.
    Iter operator++(){
        ptr++;
        return *this;
    }
    // Prefix --.
    Iter operator--(){
        ptr--;
        return *this;
    }
    // Postfix ++.
    Iter operator++(int notused){
        T *tmp = ptr;
        ptr++;
        return Iter<T>(tmp, begin, end);
    }
    // Postfix --.
    Iter operator--(int notused){
        T *tmp = ptr;
        ptr--;
        return Iter<T>(tmp, begin, end);
    }
    // Return a reference to the object at the specified index
    T& operator[](int i){
        if ((i < 0) || (i >= (end - begin)))
            throw OutOfRangeExc();
        return ptr[i];
    }
    // Define the relational operators.
    bool operator==(Iter op2){
        return ptr == op2.ptr;
    }
    bool operator!=(Iter op2){
        return ptr != op2.ptr;
    }
    bool operator<(Iter op2){
        return ptr < op2.ptr;
    }
    bool operator<=(Iter op2){
        return ptr <= op2.ptr;
    }
    bool operator>(Iter op2){
        return ptr > op2.ptr;
    }
    bool operator>=(Iter op2){
        return ptr >= op2.ptr;
    }
    // Subtract an integer from an Iter.
    Iter operator-(int n){
        ptr -= n;
        return *this;
    }
    // Add an integer to an Iter.
    Iter operator+(int n){
        ptr += n;
        return *this;
    }
    // Return number of elements between two Iters.
    int operator-(Iter<T> &itr2){
        return ptr - itr2.ptr;
    }
};
