/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include <string>
#include <iostream>
#include "object.hpp"
#include "Table.hpp"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

Table::Table()
{
    m_iCount = 0;
    m_iCapacity = 3;

	for (int i = 0; i <= m_iCapacity; i++) {
		Entry ent;
		ent.m_pKey = NULL;
		ent.m_oValue = Fox_Nil;
		m_vEntries.push_back(ent);
    }
}

Entry& Table::FindEntry(ref<ObjectString> pKey)
{
    uint32_t index = pKey->hash & m_iCapacity;
    Entry* tombstone = NULL;

    for (;;) {
        Entry& entry = m_vEntries[index];

        if (entry.m_pKey == NULL) {
            if (Fox_IsNil(entry.m_oValue))
                return tombstone != NULL ? *tombstone : entry;
            else
                if (tombstone == NULL) tombstone = &entry;
        } else if (entry.m_pKey->hash == pKey->hash && entry.m_pKey->string == pKey->string)
            return entry;

        index = (index + 1) & m_iCapacity;
    }
}

void Table::AdjustCapacity(int capacity)
{
	Table oTemp;
	oTemp.m_iCapacity = capacity;
    for (int i = 0; i <= capacity; i++) {
		Entry ent;
		ent.m_pKey = NULL;
		ent.m_oValue = Fox_Nil;
		oTemp.m_vEntries.push_back(ent);
    }
    
    m_iCount = 0;
    for (int i = 0; i <= m_iCapacity; i++) {
        Entry& entry = m_vEntries[i];
        if (entry.m_pKey == NULL)
            continue;
        Entry& dest = oTemp.FindEntry(entry.m_pKey);
        dest.m_pKey = entry.m_pKey;
        dest.m_oValue = entry.m_oValue;
        m_iCount++;
    }
	m_vEntries.clear();
    m_vEntries = oTemp.m_vEntries;
    m_iCapacity = capacity;
}

bool Table::Set(ref<ObjectString> key, Value value)
{
    if (m_iCount + 1 > (m_iCapacity + 1) * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(m_iCapacity + 1) - 1;
        AdjustCapacity(capacity);
    }

    Entry& entry = FindEntry(key);
    bool is_new_key = entry.m_pKey == NULL;
    if (is_new_key && Fox_IsNil(entry.m_oValue))
        m_iCount++;

    entry.m_pKey = key;
    entry.m_oValue = value;
    return is_new_key;
}


void Table::AddAll(Table& from)
{
    for (int i = 0; i <= from.m_iCapacity; i++) {
        Entry& entry = from.m_vEntries[i];
        if (entry.m_pKey != NULL) {
            Set(entry.m_pKey, entry.m_oValue);
        }
    }
}

void Table::Print()
{
    for (int i = 0; i <= m_iCapacity; i++) {
        Entry& entry = m_vEntries[i];
        if (entry.m_pKey != NULL) {
            std::cout << entry.m_pKey->string << std::endl;
        }
    }
}

bool Table::Get(ref<ObjectString> key, Value& value)
{
    if (m_iCount == 0)
        return false;

    Entry& entry = FindEntry(key);
    if (entry.m_pKey == NULL)
        return false;

    value = entry.m_oValue;
    return true;
}

bool Table::Delete(ref<ObjectString> key)
{
    if (m_iCount == 0)
        return false;

    Entry& entry = FindEntry(key);
    if (entry.m_pKey == NULL)
        return false;
    entry.m_pKey = NULL;
    entry.m_oValue = Fox_Bool(true);
    return true;
}

ref<ObjectString> Table::FindString(const char *chars, int length, uint32_t hash)
{
    if (m_iCount == 0)
        return NULL;
    uint32_t index = hash & m_iCapacity;

    for (;;) {
        Entry& entry = m_vEntries[index];

        if (entry.m_pKey == NULL) {
            if (Fox_IsNil(entry.m_oValue))
                return NULL;
        } else if (entry.m_pKey->string == std::string(chars, length) && entry.m_pKey->hash == hash) {
            return entry.m_pKey;
        }
        index = (index + 1) & m_iCapacity;
    }
}

ref<ObjectString> Table::FindString(const std::string& string, uint32_t hash)
{
    if (m_iCount == 0)
        return NULL;
    uint32_t index = hash & m_iCapacity;

    for (;;) {
        Entry& entry = m_vEntries[index];

        if (entry.m_pKey == NULL) {
            if (Fox_IsNil(entry.m_oValue))
                return NULL;
        } else if (entry.m_pKey->string == string && entry.m_pKey->hash == hash) {
            return entry.m_pKey;
        }
        index = (index + 1) & m_iCapacity;
    }
}

void Table::MarkTable()
{
    // for (int i = 0; i <= m_iCapacity; i++)
	// {
    //     Entry& entry = m_vEntries[i];
    //     MarkObject(entry.m_pKey);
    //     MarkValue(entry.m_oValue);
    // }
}

void Table::RemoveWhite()
{
    // for (int i = 0; i <= m_iCapacity; i++)
	// {
    //     Entry& entry = m_vEntries[i];
    //     if (entry.m_pKey != NULL && !entry.m_pKey->mMarked) {
    //         Delete(entry.m_pKey);
    //     }
    // }
}
