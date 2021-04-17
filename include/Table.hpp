

#ifndef FOX_TABLE_HPP_
#define FOX_TABLE_HPP_

class ObjectString;

#define TABLE_MAX_LOAD (0.75)

struct Entry
{
    ref<ObjectString> m_pKey;
    Value m_oValue;

	Entry() : m_oValue()
	{
	}

	bool operator==(const Entry& other) const
    {
        return m_pKey == other.m_pKey && m_oValue == other.m_oValue;
    }
};

class Table
{
public:
	Table();
    int m_iCount;
    int m_iCapacity;
	std::vector<Entry> m_vEntries;

	void AdjustCapacity(int capacity);
	Entry& FindEntry(ref<ObjectString> pKey);
	bool Set(ref<ObjectString> key, Value value);
	void AddAll(Table& from);
	bool Get(ref<ObjectString> key, Value& value);
	bool Delete(ref<ObjectString> key);
	ref<ObjectString> FindString(const char *chars, int length, uint32_t hash);
	ref<ObjectString> FindString(const std::string& string, uint32_t hash);
	void MarkTable();
	void RemoveWhite();
	void Print();
};

#endif