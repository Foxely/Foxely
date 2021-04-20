/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include <string>
#include <iostream>
#include "object.hpp"
#include "Map.hpp"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

static inline uint32_t hashBits(uint64_t hash)
{
    // From v8's ComputeLongHash() which in turn cites:
    // Thomas Wang, Integer Hash Functions.
    // http://www.concentric.net/~Ttwang/tech/inthash.htm
    hash = ~hash + (hash << 18);  // hash = (hash << 18) - hash - 1;
    hash = hash ^ (hash >> 31);
    hash = hash * 21;  // hash = (hash + (hash << 2)) + (hash << 4);
    hash = hash ^ (hash >> 11);
    hash = hash + (hash << 6);
    hash = hash ^ (hash >> 22);
    return (uint32_t)(hash & 0x3fffffff);
}

// Generates a hash code for [num].
static inline uint32_t hashNumber(double num)
{
    // Hash the raw bits of the value.
    DoubleBits bits;
    bits.num = num;
    return hashBits(bits.bits64);
}

static inline uint32_t hashInt(int num)
{
    // Hash the raw bits of the value.
    return hashBits(num);
}

// Generates a hash code for [object].
static uint32_t hashObject(Object* pObject)
{
    switch (pObject->type)
    {
        case OBJ_CLASS:
        // Classes just use their name.
        return hashObject(((ObjectClass *) pObject)->name);
        
        // Allow bare (non-closure) functions so that we can use a map to find
        // existing constants in a function's constant table. This is only used
        // internally. Since user code never sees a non-closure function, they
        // cannot use them as map keys.
        case OBJ_CLOSURE:
        {
            ObjectClosure* pClosure = (ObjectClosure *) pObject;
            return hashNumber(pClosure->function->arity) ^ hashNumber(pClosure->function->chunk.m_vCode.size());
        }

        case OBJ_STRING:
            return ((ObjectString *)pObject)->hash;

        default:
            FOX_ASSERT(false, "Only immutable objects can be hashed.");
        return 0;
    }
}

// Generates a hash code for [value], which must be one of the built-in
// immutable types: null, bool, class, num, range, or string.
static uint32_t hashValue(Value value)
{
  // TODO: We'll probably want to randomize this at some point.

    switch (value.type)
    {
        case VAL_BOOL:      return (Fox_AsBool(value) ? 1 : 0);
        case VAL_NIL:       return 1;
        // case VAL_INT:       return hashInt(Fox_AsInt(value));
        case VAL_NUMBER:    return hashNumber(Fox_AsNumber(value));
        case VAL_OBJ:       return hashObject(Fox_AsObject(value));
        default:            return -1;
    }
    
    return 0;
}

MapTable::MapTable()
{
    m_iCount = 0;
    m_iCapacity = 0;

	for (int i = 0; i <= m_iCapacity; i++) {
		MapEntry ent;
		ent.m_oKey = Fox_Nil;
		ent.m_oValue = Fox_Nil;
		m_vEntries.push_back(ent);
    }
}

MapEntry& MapTable::FindEntry(Value oKey)
{
    uint32_t index = hashValue(oKey) & m_iCapacity;
    MapEntry* tombstone = NULL;
    MapEntry oTemp = MapEntry();

    if (index == -1) return oTemp;
    if (m_iCapacity == 0) return oTemp;

    for (;;)
    {
        MapEntry& entry = m_vEntries[index];

        if (Fox_IsNil(entry.m_oKey)) {
            if (Fox_IsNil(entry.m_oValue))
                return tombstone != NULL ? *tombstone : entry;
            else if (tombstone == NULL)
                    tombstone = &entry;
        } else if (ValuesEqual(entry.m_oKey, oKey))
            return entry;

        index = (index + 1) & m_iCapacity;
    }
}

void MapTable::AdjustCapacity(int capacity)
{
	MapTable oTemp;
	oTemp.m_iCapacity = capacity;
    for (int i = 0; i <= capacity; i++) {
		MapEntry ent;
		ent.m_oKey = Fox_Nil;
		ent.m_oValue = Fox_Nil;
		oTemp.m_vEntries.push_back(ent);
    }
    
    m_iCount = 0;
    for (int i = 0; i <= m_iCapacity; i++) {
        MapEntry& entry = m_vEntries[i];
        if (Fox_IsNil(entry.m_oKey))
            continue;
        MapEntry& dest = oTemp.FindEntry(entry.m_oKey);
        dest.m_oKey = entry.m_oKey;
        dest.m_oValue = entry.m_oValue;
        m_iCount++;
    }

    for (int i = 0; i <= m_iCapacity; i++) {
		MapEntry& entry = m_vEntries[i];
        if (Fox_IsNil(entry.m_oKey))
            continue;
        MapEntry& dest = oTemp.FindEntry(entry.m_oKey);
        dest.m_oKey = entry.m_oKey;
        dest.m_oValue = entry.m_oValue;
        m_iCount++;
    }

	m_vEntries.clear();
    m_vEntries = oTemp.m_vEntries;
    m_iCapacity = capacity;
}

bool MapTable::Set(Value oKey, Value value)
{
    if (m_iCount + 1 > (m_iCapacity + 1) * MAP_MAX_LOAD) {
        int capacity = GROW_CAPACITY(m_iCapacity + 1) - 1;
        AdjustCapacity(capacity);
    }

    MapEntry& entry = FindEntry(oKey);
    bool is_new_key = Fox_IsNil(entry.m_oKey);
    if (is_new_key && Fox_IsNil(entry.m_oValue))
        m_iCount++;

    entry.m_oKey = oKey;
    entry.m_oValue = value;
    return is_new_key;
}


void MapTable::AddAll(MapTable& from)
{
    for (int i = 0; i <= from.m_iCapacity; i++) {
        MapEntry& entry = from.m_vEntries[i];
        if (!Fox_IsNil(entry.m_oKey)) {
            Set(entry.m_oKey, entry.m_oValue);
        }
    }
}

void MapTable::Print()
{
    std::cout << "Map Table {" << std::endl;
    for (int i = 0; i <= m_iCapacity; i++) {
        MapEntry& entry = m_vEntries[i];
        std::cout << "key: ";
        PrintValue(entry.m_oKey);
        std::cout << " ";
        std::cout << "value: ";
        PrintValue(entry.m_oValue);
        std::cout << std::endl;
    }
    std::cout << "}"  << std::endl;
}

bool MapTable::Get(Value oKey, Value& value)
{
    if (m_iCount == 0)
        return false;

    MapEntry& entry = FindEntry(oKey);
    if (Fox_IsNil(entry.m_oKey))
        return false;

    value = entry.m_oValue;
    return true;
}

bool MapTable::Delete(Value key)
{
    if (m_iCount == 0)
        return false;

    MapEntry& entry = FindEntry(key);
    if (Fox_IsNil(entry.m_oKey))
        return false;
    entry.m_oKey = Fox_Nil;
    entry.m_oValue = Fox_Nil;
    m_iCount--;
    return true;
}

void MapTable::MarkMap()
{
    // for (int i = 0; i <= m_iCapacity; i++)
	// {
    //     MapEntry& entry = m_vEntries[i];
    //     MarkObject(entry.m_oKey);
    //     MarkValue(entry.m_oValue);
    // }
}

void MapTable::RemoveWhite()
{
    // for (int i = 0; i <= m_iCapacity; i++)
	// {
    //     MapEntry& entry = m_vEntries[i];
    //     if (!Fox_IsNil(entry.m_oKey) && !Fox_AsObject(entry.m_oKey)->mMarked) {
    //         Delete(entry.m_oKey);
    //     }
    // }
}

int MapTable::Size()
{
    return m_iCount;
}