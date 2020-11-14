#include <iostream>
#include <memory>
#include <algorithm>
#include <setjmp.h>
#include "Parser.h"
#include "vm.hpp"
#include "gc.hpp"

Traceable::Traceable()
{
	mMarked = false;
}

Traceable::~Traceable() { }


void Traceable::mark()
{
	if (!mMarked)
	{
		mMarked = true;
		markChildren();
	}
}

void Traceable::markChildren() { }

GC::GC(VM* pVm)
{
	bytesAllocated = 0;
	nextGC = 1024 * 20;
	m_pVm = pVm;
}

/**
 * Initializes address of the main frame.
 */
GC::~GC()
{
	m_oRoots.clear();
	Collect();
}

void GC::Dump(const char *label)
{
	print("\n------------------------------------------------");
	print(label);

	print("\n{");

	for (const auto &it : m_oHeap)
	{
		print(it, ": {.marked = ", it->mMarked, "}, ");
	}

	print("}\n");
}

template <typename... T>
void GC::print(const T &... t)
{
	(void)std::initializer_list<int>{(std::cout << t << "", 0)...};
	std::cout << "\n";
}

template <typename T>
void GC::PrintVector(std::vector<T> const &input)
{
	print("\n{");
	for (int i = 0; i < input.size(); i++)
	{
		print("  ", input.at(i), ", ");
	}
	print("}\n");
}

/**
 * Mark phase.
 */
void GC::Mark()
{
	for (ObjectSet::iterator it = m_oRoots.begin(); it != m_oRoots.end(); ++it)
	{
    	(*it)->mark();
	}
}

/**
 * Sweep phase.
 */
void GC::Sweep()
{
	unsigned int live = 0;
	unsigned int dead = 0;
	unsigned int total = 0;
	std::vector<ObjectSet::iterator> erase;
	for (ObjectSet::iterator it = m_oHeap.begin(); it != m_oHeap.end(); ++it)
	{
		Traceable* p = *it;
		total++;
		if (p->mMarked) {
			p->mMarked = false;
			++live;
		}
		else {
			erase.push_back(it);
		}
	}
	dead = erase.size();
	for (std::vector<ObjectSet::iterator>::iterator it = erase.begin(); it != erase.end(); ++it) {
		Traceable* p = **it;
		m_oHeap.erase(*it);
		delete p;
	}
	// if (verbose) {
	// 	cout << "GC: " << live << " objects live after sweep" << endl;
	// 	cout << "GC: " << dead << " objects dead after sweep" << endl;
  	// }
}

/**
 * Mark-Sweep GC.
 */
void GC::Collect()
{
	Mark();

#ifdef DEBUG_LOG_GC
	Dump("After mark:");
#endif

	Sweep();
	nextGC = bytesAllocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
	Dump("After sweep:");
#endif
}

void GC::AddObject(Traceable* o)
{
  	m_oHeap.insert(o);
}

void GC::RemoveObject(Traceable* o)
{
	m_oHeap.erase(o);
}

void GC::AddRoot(Traceable* root)
{
	m_oRoots.insert(root);
}

void GC::RemoveRoot(Traceable* root)
{
	m_oRoots.erase(root);
}

void GC::ClearRoots()
{
	m_oRoots.clear();
}
