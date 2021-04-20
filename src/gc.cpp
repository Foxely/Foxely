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

void Traceable::on_destroy()
{
}

GC::GC()
{
	bytesAllocated = 0;
	nextGC = 1024 * 1024;
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

std::uint32_t GC::add_callback(std::uint32_t id, std::function<void()> cb)
{
    auto& phase = m_vEvents[id];
    phase.push_back(cb);
    return phase.size() - 1;
}

void GC::remove(std::uint32_t id, std::uint32_t idx)
{
    auto& phase = m_vEvents[id];
    phase.erase(phase.begin() + idx);
}

void GC::emit(std::uint32_t id)
{
    auto& phase = m_vEvents[id];
    for (auto& cb : phase) {
        cb();
    }
}

void GC::Prepare()
{
	m_vBeCollected.clear();
	for (ObjectSet::iterator it = m_oHeap.begin(); it != m_oHeap.end(); ++it)
	{
		Traceable* p = *it;
		// total++;
		if (p && p->mMarked) {
			p->mMarked = false;
			// ++live;
		}
		else {
			p->on_destroy();
			m_vBeCollected.push_back(it);
		}
	}
}

/**
 * Sweep phase.
 */
void GC::Sweep()
{
	// unsigned int live = 0;
	// unsigned int dead = 0;
	// unsigned int total = 0;
	// std::vector<ObjectSet::iterator> erase;
	// for (ObjectSet::iterator it = m_oHeap.begin(); it != m_oHeap.end(); ++it)
	// {
	// 	Traceable* p = *it;
	// 	total++;
	// 	if (p && p->mMarked) {
	// 		p->mMarked = false;
	// 		++live;
	// 	}
	// 	else {
	// 		erase.push_back(it);
	// 	}
	// }
	// dead = erase.size();
	for (auto it : m_vBeCollected) {
		Traceable* p = *it;
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

// #ifdef DEBUG
	// if (m_pVm->IsLogGC())
		// Dump("After mark:");
// #endif
	Prepare();
	Sweep();
	nextGC = bytesAllocated * GC_HEAP_GROW_FACTOR;

// #ifdef DEBUG
// 	if (m_pVm->IsLogGC())
// 		Dump("After sweep:");
// #endif
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