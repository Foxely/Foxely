#pragma once


class Value;

#define MAP_MAX_LOAD (0.75)

// A union to let us reinterpret a double as raw bits and back.
typedef union
{
    uint64_t bits64;
    uint32_t bits32[2];
    double num;
} DoubleBits;

struct MapEntry
{
    Value m_oKey;
    Value m_oValue;

	bool operator==(const MapEntry& other) const
    {
        return m_oKey == other.m_oKey && m_oValue == other.m_oValue;
    }
};

class MapTable
{
public:
	MapTable();
    int m_iCount;
    int m_iCapacity;
	std::vector<MapEntry> m_vEntries;

	void AdjustCapacity(int capacity);
	MapEntry& FindEntry(Value oKey);
	bool Set(Value oKey, Value value);
	void AddAll(MapTable& from);
	bool Get(Value oKey, Value& value);
	bool Delete(Value oKey);
	void MarkMap();
	void RemoveWhite();
	void Print();
};