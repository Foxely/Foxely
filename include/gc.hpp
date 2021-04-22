#ifndef FOX_GC_HPP_
#define FOX_GC_HPP_

#include <set>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#define GC_HEAP_GROW_FACTOR 2

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
	virtual void on_destroy();

	// Mark the object and all its children as live
  	void mark();

	// Overridden by derived classes to call mark()
	// on objects referenced by this object. The default
	// implemention does nothing.
	virtual void markChildren();
};

const std::uint32_t GC_OnMark = 10;
class GC
{
	typedef std::set<Traceable*> ObjectSet;
	ObjectSet m_oRoots;
    std::unordered_map<std::uint32_t, std::vector<std::function<void()>>> m_vEvents;
    std::vector<ObjectSet::iterator> m_vBeCollected;
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
	void Prepare();

public:
	int bytesAllocated;
	int nextGC;


	GC();
	~GC();
	void Dump(const char *label);
	void Collect();
	void AddObject(Traceable* o);
	void RemoveObject(Traceable* o);
	void AddRoot(Traceable* obj);
	void RemoveRoot(Traceable* obj);
	void ClearRoots();
	
    
    std::uint32_t add_callback(std::uint32_t id, std::function<void()> pSys);
    void remove(std::uint32_t id, std::uint32_t idx);
    void emit(std::uint32_t id);

	template <typename T, typename... Args>
	T* New(Args&&... args);
	template <class T>
    T* New();
	template <class T>
	T* NewArray(size_t count);
};

template <typename T, typename... Args>
inline T* GC::New(Args&&... args)
{
	T* pObject = new T(std::forward<Args>(args)...);

	bytesAllocated += sizeof(T);

	if (bytesAllocated > nextGC)
    {
		ClearRoots();
    	emit(GC_OnMark);
    	Collect();
    }
	AddObject(pObject);
    return pObject;
}

template <class T>
inline T* GC::New()
{
    T* pObject = new T();

	if (!pObject)
		return nullptr;

	bytesAllocated += sizeof(T);

	if (bytesAllocated > nextGC)
    {
		ClearRoots();
    	emit(GC_OnMark);
    	Collect();
    }
	AddObject(pObject);
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
    	emit(GC_OnMark);
    	Collect();
    }
    return pObject;
}

#endif