#pragma once

#include <cstddef>
#include <vector>

template <typename T>
class DynamicArray
{
public:
    class iterator
    {
    public:
        using value_type = T;
        using pointer    = T*;
        using reference  = T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        iterator():                    v(nullptr), i(0) {}   
        iterator(DynamicArray<T>* v, int i): v(v),       i(i) {}
        // Default Copy/Move Are Fine.
        // Default Destructor fine.

        reference       operator*()             {return (*v)[i];}
        const reference operator*()       const {return (*v)[i];}
        pointer         operator->()            {return &((*v)[i]);}
        const pointer   operator->()      const {return &((*v)[i]);}
        reference       operator[](int m)       {return (*v)[i + m];}
        const reference operator[](int m) const {return (*v)[i + m];}
        

        iterator& operator++()       {++i;return *this;}
        iterator& operator--()       {--i;return *this;}
        iterator  operator++(int)    {iterator r(*this);++i;return r;}
        iterator  operator--(int)    {iterator r(*this);--i;return r;}

        iterator& operator+=(int n)  {i += n;return *this;}
        iterator& operator-=(int n)  {i -= n;return *this;}

        iterator operator+(int n)   const {iterator r(*this);return r += n;}
        iterator operator-(int n)   const {iterator r(*this);return r -= n;}

        difference_type operator-(iterator const& r) const {return i - r.i;}

        // Note: comparing iterator from different containers
        //       is undefined behavior so we don't need to check
        //       if they are the same container.
        bool operator<(iterator const& r)  const {return i <  r.i;}
        bool operator<=(iterator const& r) const {return i <= r.i;}
        bool operator>(iterator const& r)  const {return i >  r.i;}
        bool operator>=(iterator const& r) const {return i >= r.i;}
        bool operator!=(const iterator &r) const {return i != r.i;}
        bool operator==(const iterator &r) const {return i == r.i;}

    private:
        DynamicArray<T>* v;
        int        i;
    };

    DynamicArray()
    {
        m_lCapacity = 10;
	    m_pData = new T [m_lCapacity];
	    m_lSize = 0;
    }

    DynamicArray(int iCapacity) : m_lCapacity(iCapacity)
    {
	    m_pData = new T [m_lCapacity];
	    m_lSize = 0;
    }

    ~DynamicArray()
    {
	    delete[] m_pData;
    }
    
    void resize(size_t lNewCapacity)
    {
	    T* pNewData = new T [lNewCapacity];

        if (m_pData)
        {
            for (int i = 0; i < m_lCapacity; ++i)
		        pNewData[i] = m_pData[i];
	        delete [] m_pData;
        }
	    m_pData = pNewData;
	    m_lCapacity = lNewCapacity;
    }

    size_t push(T oNewData)
    {
        size_t lIndex = 0;
        if (!m_vAvailable.empty())
        {
            lIndex = m_vAvailable.back();
            m_pData[lIndex] = oNewData;
            m_vAvailable.pop_back();
            return lIndex;
        }
        if (m_lSize == m_lCapacity)
            resize(m_lCapacity * GrowPercent);

        lIndex = m_lSize;
	    m_pData[m_lSize++] = oNewData;
        return lIndex;
    }

    void pop()
    {
        if (m_lSize > 0)
	        m_lSize--;
    }

    void insert(size_t lIndex, T oNewData)
    {
	    if (lIndex >= m_lCapacity)
            resize(lIndex * GrowPercent);

	    m_pData[lIndex++] = oNewData;
    }

    void remove(T oData)
    {
        size_t lIndex = find(oData);
        if (lIndex != -1)
        {
            m_pData[lIndex] = 0;
            m_vAvailable.push_back(lIndex);
        }
    }

    size_t find(T oData)
    {
        for (size_t i = 0; i < m_lSize; ++i)
            if (m_pData[i] == oData)
                return i;
        return -1;
    }

    size_t size()
    {
        return m_lSize;
    }

    bool empty()
    {
        return m_lSize <= 0;
    }

    T& get(size_t lIndex)
    {
	    return m_pData[lIndex];
    }

    void clear()
    {
        m_lCapacity = 10;
        m_lSize = 0;
	    delete [] m_pData;
        m_pData = nullptr;
    }

    T& back()
    {
	    return m_pData[m_lSize - 1];
    }

    iterator begin()
    {
        return iterator(this, 0);
    }

    iterator end()
    {
        return iterator(this, m_lSize);
    }

    T& operator [](size_t lIndex)
    {
        if (lIndex >= m_lCapacity)
        {
            m_lSize = lIndex;
            resize(lIndex * GrowPercent);
        }

        // m_pData[m_lSize] = oNewData;
        // m_lSize++;
        return m_pData[lIndex];
    }

    T& operator [](int iIndex)
    {
        if (iIndex >= m_lCapacity)
        {
            m_lSize = iIndex;
            resize(iIndex * GrowPercent);
        }

        // m_pData[m_lSize] = oNewData;
        // m_lSize++;
        return m_pData[iIndex];
    }

    T& operator [](uint32_t lIndex)
    {
        if (lIndex >= m_lCapacity)
        {
            m_lSize = lIndex;
            resize(lIndex * GrowPercent);
        }

        // m_pData[m_lSize] = oNewData;
        // m_lSize++;
        return m_pData[lIndex];
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& rhs)
    {
        copy(rhs);
        return *this;
    }

    bool operator==(const DynamicArray<T>& rhs) const
    {
        if (m_lSize != rhs.m_lSize)
            return false;
        for (size_t i = 0; i < m_lSize; i++)
            if (!(m_pData[i] == rhs.m_pData[i]))
                return false;
        return true;
    }

    bool operator!=(const DynamicArray<T>& rhs) const
    {
        return !(*this == rhs);
    }

private:
    void copy(const DynamicArray<T>& other)
    {
	    T* pNewData = new T [other.m_lCapacity];

        if (other.m_pData)
        {
            for (int i = 0; i < other.m_lCapacity; ++i)
		        pNewData[i] = other.m_pData[i];
        }
        if (m_pData)
	        delete [] m_pData;
	    m_pData = pNewData;
	    m_lSize = other.m_lSize;
	    m_lCapacity = other.m_lCapacity;
    }

private:
    T* m_pData;
    size_t m_lSize;
    size_t m_lCapacity;
    float GrowPercent = 1.2f;
    std::vector<size_t> m_vAvailable;
};