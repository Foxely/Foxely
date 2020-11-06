#pragma once

#include <set>
#include <string>
#include <utility>

#define GC_HEAP_GROW_FACTOR 2

class VM;

/**
 * The `Traceable` struct is used as a base class
 * for any object which should be managed by GC.
 */
class Traceable
{
public:
	bool mMarked;

	Traceable();
	virtual ~Traceable();

	// Mark the object and all its children as live
  	void mark();

	// Overridden by derived classes to call mark()
	// on objects referenced by this object. The default
	// implemention does nothing.
	virtual void markChildren();
};

/**
 * Traceable Node structure.
 *
 * Contains the name of the node, and the
 * pointers to left and right sub-nodes,
 * forming a tree.
 */
// class Node : public Traceable
// {
// public:
// 	std::string m_name;

// 	Node *m_left;
// 	Node *m_right;

// 	Node()
// 	{
// 		m_name = " ";
// 		m_left = NULL;
// 		m_right = NULL;
// 	}

// 	virtual void markChildren() override
// 	{
// 		if (m_left)
// 			m_left->mark();
// 		if (m_right)
// 			m_right->mark();
// 	}
// };

class GC
{
	typedef std::set<Traceable*> ObjectSet;
	ObjectSet m_oRoots;
	/**
	* Objects tracing information: allocation pointer to header.
	*/
	ObjectSet m_oHeap;

	template <typename... T>
	void print(const T &... t);
	template <typename T>
	void PrintVector(std::vector<T> const &input);
	void Mark();
	void Sweep();

public:
	int bytesAllocated;
	int nextGC;
	VM* m_pVm;


	GC(VM* pVm);
	~GC();
	void Dump(const char *label);
	void Collect();
	void AddObject(Traceable* o);
	void RemoveObject(Traceable* o);
	void AddRoot(Traceable* obj);
	void RemoveRoot(Traceable* obj);
	void ClearRoots();

	template <typename T, typename... Args>
	T* New(Args&&... args);
	template <class T> T* New();

	template <class T>
	T* NewArray(size_t count);
};


template <typename T, typename... Args>
inline T* GC::New(Args&&... args)
{
	T* pObject = new T(std::forward<Args>(args)...);
	AddObject(pObject);

	bytesAllocated += sizeof(T);

	if (bytesAllocated > nextGC)
    {
        std::cerr << "Collect Garbage" << std::endl;
		ClearRoots();
		m_pVm->AddToRoots();
    	Collect();
    }
    return pObject;
}

template <class T> inline T* GC::New()
{
    T* pObject = new T();
	AddObject(pObject);

	bytesAllocated += sizeof(T);

	if (bytesAllocated > nextGC)
    {
        std::cerr << "Collect Garbage" << std::endl;
		ClearRoots();
		m_pVm->AddToRoots();
    	Collect();
    }
    return pObject;
}

// template <class T> void Delete(Allocator& allocator, T& object)
// {
//     object.~T(); 
//     allocator.Deallocate(&object);
// }

template <typename T>
inline T* GC::NewArray(size_t count)
{
    T* pObject = new T[count];
	for (int i = 0; i < count; i++)
		AddObject(pObject[i]);

	bytesAllocated += sizeof(T) * count;

	if (bytesAllocated > nextGC)
    {
		ClearRoots();
		m_pVm->AddToRoots();
    	Collect();
    }
    return pObject;
}