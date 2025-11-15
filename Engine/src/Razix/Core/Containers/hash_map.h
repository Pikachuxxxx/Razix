#ifndef _RZ_HASH_MAP_H_
#define _RZ_HASH_MAP_H_

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/std/type_traits.h"
#include "Razix/Core/std/utility.h"

#include "Razix/Core/Containers/hash_functors.h"

// just for the std::type_index hash specialization
#include <typeindex>

// TODO: Implement this https://www.youtube.com/watch?v=ncHmEUmJZf4 from CppCon 2017
// TODO: Add move/copy asserts to types used as Key/Value in the hash map, this will help the users

#define RZ_INITIAL_HASH_MAP_CAPACITY 16
#define RZ_HASH_MAP_GROWTH_FACTOR    2

namespace Razix {

    //--------------------------------------------------
    // For comparing keys
    //--------------------------------------------------

    template<typename T>
    struct rz_equal_to
    {
        bool operator()(const T& a, const T& b) const
        {
            return a == b;
        }
    };

    RAZIX_API extern i32 rz_strcmp(const char* s1, const char* s2);

    // Specialization for C-strings
    template<>
    struct rz_equal_to<const char*>
    {
        bool operator()(const char* a, const char* b) const
        {
            return rz_strcmp(a, b) == 0;
        }
    };

    //--------------------------------------------------
    // Hash Map Entry
    //--------------------------------------------------

    template<typename K, typename V>
    struct RZPair
    {
        const K& first;
        V&       second;

        //// This templated constructor uses "perfect forwarding" to handle
        //// copying, moving, or binding references automatically.
        //// This allows RZPair to hold either values or references.
        //template<typename U1, typename U2>
        //RZPair(U1&& f, U2&& s)
        //    : first(std::forward<U1>(f)), second(std::forward<U2>(s))
        //{
        //}

        RZPair& operator=(const RZPair& other)
        {
            second = other.second;
            return *this;
        }

        template<typename OtherPair>
        RZPair& operator=(const OtherPair& other)
        {
            second = other.second;
            return *this;
        }
    };

    template<typename K, typename V>
    struct RZInitalizerPair
    {
        K first;
        V second;
    };

    //--------------------------------------------------
    // Hash Map Iterator
    //--------------------------------------------------

    template<typename Key, typename Value, typename Hash, typename Equal>
    class RZHashMap;

    template<typename Key, typename Value>
    class RZHashMapIterator
    {
    public:
        using pair_type = RZPair<Key&, Value&>;
        using size_type = sz;

        RZHashMapIterator();
        ~RZHashMapIterator();

        RZHashMapIterator(const RZHashMapIterator& other);
        RZHashMapIterator& operator=(const RZHashMapIterator& other);

        RZHashMapIterator(RZHashMapIterator&& other) noexcept;
        RZHashMapIterator& operator=(RZHashMapIterator&& other) noexcept;

        RZHashMapIterator& operator++();
        RZHashMapIterator  operator++(int);

        bool operator==(const RZHashMapIterator& other) const;
        bool operator!=(const RZHashMapIterator& other) const;

        pair_type* operator->()
        {
            new (&m_ProxyStorage) pair_type{m_Keys[m_Index], m_Values[m_Index]};
            return reinterpret_cast<pair_type*>(&m_ProxyStorage);
        }

        const pair_type* operator->() const
        {
            new (&m_ProxyStorage) pair_type{m_Keys[m_Index], m_Values[m_Index]};
            return reinterpret_cast<const pair_type*>(&m_ProxyStorage);
        }

        pair_type& operator*()
        {
            new (&m_ProxyStorage) pair_type{m_Keys[m_Index], m_Values[m_Index]};
            return *reinterpret_cast<pair_type*>(&m_ProxyStorage);
        }

        const pair_type& operator*() const
        {
            new (&m_ProxyStorage) pair_type{m_Keys[m_Index], m_Values[m_Index]};
            return *reinterpret_cast<const pair_type*>(&m_ProxyStorage);
        }

        Key&         key();
        const Key&   key() const;
        Value&       value();
        const Value& value() const;
        Key&         first();
        const Key&   first() const;
        Value&       second();
        const Value& second() const;

    private:
        template<typename K, typename V, typename H, typename E>
        friend class RZHashMap;

        Key*      m_Keys;
        Value*    m_Values;
        bool*     m_Occupied;
        size_type m_Index;
        size_type m_Capacity;
        mutable unsigned char m_ProxyStorage[sizeof(pair_type)];

        RZHashMapIterator(Key* keys, Value* values, bool* occupied, size_type capacity, size_type start_index = 0);

        void advance_to_next_occupied();
    };

    //--------------------------------------------------
    // RZHashMapIterator implementation
    //--------------------------------------------------

    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value>::RZHashMapIterator()
        : m_Keys(NULL), m_Values(NULL), m_Occupied(NULL), m_Index(0), m_Capacity(0)
    {
    }

    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value>::~RZHashMapIterator()
    {
        // The hash map owns all memory
    }

    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value>::RZHashMapIterator(const RZHashMapIterator& other)
        : m_Keys(other.m_Keys), m_Values(other.m_Values), m_Occupied(other.m_Occupied), m_Index(other.m_Index), m_Capacity(other.m_Capacity)
    {
    }

    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value>& RZHashMapIterator<Key, Value>::operator=(const RZHashMapIterator& other)
    {
        if (this != &other) {
            m_Keys     = other.m_Keys;
            m_Values   = other.m_Values;
            m_Occupied = other.m_Occupied;
            m_Index    = other.m_Index;
            m_Capacity = other.m_Capacity;
        }
        return *this;
    }

    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value>::RZHashMapIterator(RZHashMapIterator&& other) noexcept
        : m_Keys(other.m_Keys), m_Values(other.m_Values), m_Occupied(other.m_Occupied), m_Index(other.m_Index), m_Capacity(other.m_Capacity)
    {
        other.m_Keys     = NULL;
        other.m_Values   = NULL;
        other.m_Occupied = NULL;
        other.m_Index    = 0;
        other.m_Capacity = 0;
    }

    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value>& RZHashMapIterator<Key, Value>::operator=(RZHashMapIterator&& other) noexcept
    {
        if (this != &other) {
            m_Keys     = other.m_Keys;
            m_Values   = other.m_Values;
            m_Occupied = other.m_Occupied;
            m_Index    = other.m_Index;
            m_Capacity = other.m_Capacity;

            other.m_Keys     = NULL;
            other.m_Values   = NULL;
            other.m_Occupied = NULL;
            other.m_Index    = 0;
            other.m_Capacity = 0;
        }
        return *this;
    }

    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value>& RZHashMapIterator<Key, Value>::operator++()
    {
        advance_to_next_occupied();
        return *this;
    }

    // postfix
    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value> RZHashMapIterator<Key, Value>::operator++(int)
    {
        RZHashMapIterator old = *this;
        advance_to_next_occupied();
        return old;
    }

    template<typename Key, typename Value>
    bool RZHashMapIterator<Key, Value>::operator==(const RZHashMapIterator<Key, Value>& other) const
    {
        return m_Index == other.m_Index && m_Keys == other.m_Keys;
    }

    template<typename Key, typename Value>
    bool RZHashMapIterator<Key, Value>::operator!=(const RZHashMapIterator<Key, Value>& other) const
    {
        return !(*this == other);
    }

    template<typename Key, typename Value>
    RZHashMapIterator<Key, Value>::RZHashMapIterator(Key* keys, Value* values, bool* occupied, size_type capacity, size_type start_index)
        : m_Keys(keys), m_Values(values), m_Occupied(occupied), m_Index(start_index), m_Capacity(capacity)
    {
        if (m_Index < m_Capacity && !m_Occupied[m_Index])
            advance_to_next_occupied();
    }

    template<typename Key, typename Value>
    void RZHashMapIterator<Key, Value>::advance_to_next_occupied()
    {
        m_Index++;

        while (m_Index < m_Capacity) {
            if (m_Occupied[m_Index]) {
                return;
            }
            m_Index++;
        }

        // matches end() iterator
        m_Index = m_Capacity;
    }

    template<typename Key, typename Value>
    Key& RZHashMapIterator<Key, Value>::key()
    {
        return m_Keys[m_Index];
    }

    template<typename Key, typename Value>
    const Key& RZHashMapIterator<Key, Value>::key() const
    {
        return m_Keys[m_Index];
    }

    template<typename Key, typename Value>
    Value& RZHashMapIterator<Key, Value>::value()
    {
        return m_Values[m_Index];
    }

    template<typename Key, typename Value>
    const Value& RZHashMapIterator<Key, Value>::value() const
    {
        return m_Values[m_Index];
    }

    template<typename Key, typename Value>
    Key& RZHashMapIterator<Key, Value>::first()
    {
        return key();
    }

    template<typename Key, typename Value>
    const Key& RZHashMapIterator<Key, Value>::first() const
    {
        return key();
    }

    template<typename Key, typename Value>
    Value& RZHashMapIterator<Key, Value>::second()
    {
        return value();
    }

    template<typename Key, typename Value>
    const Value& RZHashMapIterator<Key, Value>::second() const
    {
        return value();
    }

    template<typename Key, typename Value>
    bool operator==(const RZHashMapIterator<Key, Value>& lhs, const RZHashMapIterator<Key, Value>& rhs)
    {
        return lhs.m_Index == rhs.m_Index && lhs.m_Keys == rhs.m_Keys;
    }

    template<typename Key, typename Value>
    bool operator!=(const RZHashMapIterator<Key, Value>& lhs, const RZHashMapIterator<Key, Value>& rhs)
    {
        return !(lhs == rhs);
    }

    //--------------------------------------------------
    // Hash Map (Simple quadratic probing with FNV-1a hashing)
    //--------------------------------------------------

    template<typename Key, typename Value, typename Hash = rz_hash<Key>, typename Equal = rz_equal_to<Key>>
    class RZHashMap
    {
    public:
        using key_type        = Key;
        using value_type      = Value;
        using size_type       = sz;
        using reference       = Value&;
        using const_reference = const Value&;
        using iterator        = RZHashMapIterator<Key, Value>;
        using const_iterator  = const RZHashMapIterator<Key, Value>;

        RZHashMap();
        explicit RZHashMap(size_type initial_capacity);
        ~RZHashMap();

        RZHashMap(const RZHashMap& other);
        RZHashMap& operator=(const RZHashMap& other);

        RZHashMap(RZHashMap&& other) noexcept;
        RZHashMap& operator=(RZHashMap&& other) noexcept;

        RZHashMap(std::initializer_list<RZInitalizerPair<Key, Value>> init)
            : RZHashMap(16)
        {
            for (const auto& pair: init) {
                insert(pair.first, pair.second);
            }
        }

        RZHashMap& operator=(std::initializer_list<RZInitalizerPair<Key, Value>> init)
        {
            clear();
            for (const auto& pair: init) {
                insert(pair.first, pair.second);
            }
            return *this;
        }

        void reserve(size_type new_capacity);
        template<typename... Args>
        void            emplace(const Key& key, Args&&... args);
        void            insert(const Key& key, const Value& value);
        void            insert(const Key& key, Value&& value);
        iterator        find(const Key& key);
        const iterator  find(const Key& key) const;
        const iterator  cfind(const Key& key) const;
        bool            contains(const Key& key) const;
        bool            remove(const Key& key);
        void            clear();
        reference       operator[](const Key& key);
        const_reference operator[](const Key& key) const;
        reference       at(const Key& key);
        const_reference at(const Key& key) const;
        size_type       size() const;
        size_type       capacity() const;
        bool            empty() const;
        float           load_factor() const;
        iterator        begin();
        iterator        end();
        const iterator  begin() const;
        const iterator  end() const;
        bool            erase(const iterator& it);

    private:
        Key*      m_Keys;
        Value*    m_Values;
        bool*     m_Occupied;
        uint64_t* m_Hashes;
        size_type m_Length;
        size_type m_Capacity;
        Hash      m_Hash;
        Equal     m_Equal;

        static size_type quadratic_probe(size_type index, size_type probe_count, size_type capacity);
        void             insert_entry(const Key& key, const Value& value);
        bool             expand();
        size_type        find_entry(const Key& key) const;

        friend class RZHashMapIterator<Key, Value>;
    };

    //--------------------------------------------------
    // RZHashMap implementation
    //--------------------------------------------------

    template<typename Key, typename Value, typename Hash, typename Equal>
    RZHashMap<Key, Value, Hash, Equal>::RZHashMap()
        : m_Keys(NULL), m_Values(NULL), m_Occupied(NULL), m_Hashes(NULL), m_Length(0), m_Capacity(RZ_INITIAL_HASH_MAP_CAPACITY)
    {
        m_Keys     = static_cast<Key*>(rz_malloc_aligned(m_Capacity * sizeof(Key)));
        m_Values   = static_cast<Value*>(rz_malloc_aligned(m_Capacity * sizeof(Value)));
        m_Occupied = static_cast<bool*>(rz_malloc_aligned(m_Capacity * sizeof(bool)));
        m_Hashes   = static_cast<uint64_t*>(rz_malloc_aligned(m_Capacity * sizeof(uint64_t)));

        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Failed to allocate memory for hash map");

        // Initialize arrays to zero/false
        memset(m_Occupied, 0, m_Capacity * sizeof(bool));
        memset(m_Hashes, 0, m_Capacity * sizeof(uint64_t));
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    RZHashMap<Key, Value, Hash, Equal>::RZHashMap(size_type initial_capacity)
        : m_Keys(NULL), m_Values(NULL), m_Occupied(NULL), m_Hashes(NULL), m_Length(0), m_Capacity(initial_capacity)
    {
        if (initial_capacity == 0) {
            m_Capacity = RZ_INITIAL_HASH_MAP_CAPACITY;
        }

        m_Keys     = static_cast<Key*>(rz_malloc_aligned(m_Capacity * sizeof(Key)));
        m_Values   = static_cast<Value*>(rz_malloc_aligned(m_Capacity * sizeof(Value)));
        m_Occupied = static_cast<bool*>(rz_malloc_aligned(m_Capacity * sizeof(bool)));
        m_Hashes   = static_cast<uint64_t*>(rz_malloc_aligned(m_Capacity * sizeof(uint64_t)));

        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Failed to allocate memory for hash map");

        // Initialize arrays to zero/false
        memset(m_Occupied, 0, m_Capacity * sizeof(bool));
        memset(m_Hashes, 0, m_Capacity * sizeof(uint64_t));
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    RZHashMap<Key, Value, Hash, Equal>::~RZHashMap()
    {
        clear();

        if (m_Keys) {
            rz_free(m_Keys);
            m_Keys = NULL;
        }
        if (m_Values) {
            rz_free(m_Values);
            m_Values = NULL;
        }
        if (m_Occupied) {
            rz_free(m_Occupied);
            m_Occupied = NULL;
        }
        if (m_Hashes) {
            rz_free(m_Hashes);
            m_Hashes = NULL;
        }

        m_Length   = 0;
        m_Capacity = 0;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    RZHashMap<Key, Value, Hash, Equal>::RZHashMap(const RZHashMap& other)
        : m_Length(0), m_Capacity(other.m_Capacity)
    {
        m_Keys     = static_cast<Key*>(rz_malloc_aligned(m_Capacity * sizeof(Key)));
        m_Values   = static_cast<Value*>(rz_malloc_aligned(m_Capacity * sizeof(Value)));
        m_Occupied = static_cast<bool*>(rz_malloc_aligned(m_Capacity * sizeof(bool)));
        m_Hashes   = static_cast<uint64_t*>(rz_malloc_aligned(m_Capacity * sizeof(uint64_t)));

        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Hash map is not initialized");

        memset(m_Occupied, 0, m_Capacity * sizeof(bool));
        memset(m_Hashes, 0, m_Capacity * sizeof(uint64_t));

        // Copy all entries
        for (size_type i = 0; i < other.m_Capacity; ++i) {
            if (other.m_Occupied[i]) {
                insert_entry(other.m_Keys[i], Value(other.m_Values[i]));
            }
        }
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    RZHashMap<Key, Value, Hash, Equal>& RZHashMap<Key, Value, Hash, Equal>::operator=(const RZHashMap& other)
    {
        if (this != &other) {
            clear();

            if (m_Capacity != other.m_Capacity) {
                rz_free(m_Keys);
                rz_free(m_Values);
                rz_free(m_Occupied);
                rz_free(m_Hashes);

                m_Capacity = other.m_Capacity;
                m_Keys     = static_cast<Key*>(rz_malloc_aligned(m_Capacity * sizeof(Key)));
                m_Values   = static_cast<Value*>(rz_malloc_aligned(m_Capacity * sizeof(Value)));
                m_Occupied = static_cast<bool*>(rz_malloc_aligned(m_Capacity * sizeof(bool)));
                m_Hashes   = static_cast<uint64_t*>(rz_malloc_aligned(m_Capacity * sizeof(uint64_t)));

                RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
                    "[RZHashMap] Hash map is not initialized");

                memset(m_Occupied, 0, m_Capacity * sizeof(bool));
                memset(m_Hashes, 0, m_Capacity * sizeof(uint64_t));
            }

            // Copy all entries
            for (size_type i = 0; i < other.m_Capacity; ++i) {
                if (other.m_Occupied[i]) {
                    insert_entry(other.m_Keys[i], Value(other.m_Values[i]));
                }
            }
        }
        return *this;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    RZHashMap<Key, Value, Hash, Equal>::RZHashMap(RZHashMap&& other) noexcept
        : m_Keys(other.m_Keys), m_Values(other.m_Values), m_Occupied(other.m_Occupied), m_Hashes(other.m_Hashes), m_Length(other.m_Length), m_Capacity(other.m_Capacity)
    {
        other.m_Keys     = NULL;
        other.m_Values   = NULL;
        other.m_Occupied = NULL;
        other.m_Hashes   = NULL;
        other.m_Length   = 0;
        other.m_Capacity = 0;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    RZHashMap<Key, Value, Hash, Equal>& RZHashMap<Key, Value, Hash, Equal>::operator=(RZHashMap&& other) noexcept
    {
        if (this != &other) {
            clear();

            rz_free(m_Keys);
            rz_free(m_Values);
            rz_free(m_Occupied);
            rz_free(m_Hashes);

            m_Keys     = other.m_Keys;
            m_Values   = other.m_Values;
            m_Occupied = other.m_Occupied;
            m_Hashes   = other.m_Hashes;
            m_Length   = other.m_Length;
            m_Capacity = other.m_Capacity;

            RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
                "[RZHashMap] Hash map is not initialized");

            other.m_Keys     = NULL;
            other.m_Values   = NULL;
            other.m_Occupied = NULL;
            other.m_Hashes   = NULL;
            other.m_Length   = 0;
            other.m_Capacity = 0;
        }
        return *this;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    void RZHashMap<Key, Value, Hash, Equal>::reserve(size_type new_capacity)
    {
        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Hash map is not initialized");

        // Only expand if new_capacity is larger than current capacity
        if (new_capacity <= m_Capacity)
            return;

        size_type actual_capacity = new_capacity;
        while (actual_capacity < m_Length * 2) {
            actual_capacity *= RZ_HASH_MAP_GROWTH_FACTOR;
        }

        // Allocate new arrays
        Key*      new_keys     = static_cast<Key*>(rz_malloc_aligned(actual_capacity * sizeof(Key)));
        Value*    new_values   = static_cast<Value*>(rz_malloc_aligned(actual_capacity * sizeof(Value)));
        bool*     new_occupied = static_cast<bool*>(rz_malloc_aligned(actual_capacity * sizeof(bool)));
        uint64_t* new_hashes   = static_cast<uint64_t*>(rz_malloc_aligned(actual_capacity * sizeof(uint64_t)));

        RAZIX_CORE_ASSERT(new_keys && new_values && new_occupied && new_hashes,
            "[RZHashMap] Failed to allocate memory during reserve");

        memset(new_occupied, 0x0, actual_capacity * sizeof(bool));
        memset(new_hashes, 0x0, actual_capacity * sizeof(uint64_t));

        // Save old data
        Key*      old_keys     = m_Keys;
        Value*    old_values   = m_Values;
        bool*     old_occupied = m_Occupied;
        uint64_t* old_hashes   = m_Hashes;
        size_type old_capacity = m_Capacity;

        // Swap to new arrays
        m_Keys     = new_keys;
        m_Values   = new_values;
        m_Occupied = new_occupied;
        m_Hashes   = new_hashes;
        m_Capacity = actual_capacity;
        m_Length   = 0;

        // Re-hash all entries into new arrays
        for (size_type i = 0; i < old_capacity; ++i) {
            if (old_occupied[i]) {
                insert_entry(old_keys[i], Value(old_values[i]));
            }
        }

        // Free old arrays
        rz_free(old_keys);
        rz_free(old_values);
        rz_free(old_occupied);
        rz_free(old_hashes);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    template<typename... Args>
    void RZHashMap<Key, Value, Hash, Equal>::emplace(const Key& key, Args&&... args)
    {
        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Hash map is not initialized");

        // If length will exceed half of current capacity, expand it.
        if (m_Length > m_Capacity / 2)
            expand();

        insert_entry(key, Value(std::forward<Args>(args)...));
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    void RZHashMap<Key, Value, Hash, Equal>::insert(const Key& key, const Value& value)
    {
        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Hash map is not initialized");

        // If length will exceed half of current capacity, expand it.
        if ((m_Length + 1) * 2 > m_Capacity) {
            expand();
        }

        insert_entry(key, value);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    void RZHashMap<Key, Value, Hash, Equal>::insert(const Key& key, Value&& value)
    {
        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Hash map is not initialized");

        // If length will exceed half of current capacity, expand it.
        if (m_Length > m_Capacity / 2)
            expand();

        insert_entry(key, value);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::iterator RZHashMap<Key, Value, Hash, Equal>::find(const Key& key)
    {
        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Hash map is not initialized");

        size_type idx = find_entry(key);
        if (idx == m_Capacity)
            return end();

        return iterator(m_Keys, m_Values, m_Occupied, m_Capacity, idx);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    const typename RZHashMap<Key, Value, Hash, Equal>::iterator RZHashMap<Key, Value, Hash, Equal>::find(const Key& key) const
    {
        return cfind(key);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    const typename RZHashMap<Key, Value, Hash, Equal>::iterator RZHashMap<Key, Value, Hash, Equal>::cfind(const Key& key) const
    {
        auto& self = const_cast<RZHashMap&>(*this);
        return self.find(key);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    bool RZHashMap<Key, Value, Hash, Equal>::contains(const Key& key) const
    {
        return find_entry(key) != m_Capacity;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    bool RZHashMap<Key, Value, Hash, Equal>::remove(const Key& key)
    {
        size_type idx = find_entry(key);
        if (idx == m_Capacity)
            return false;

        m_Keys[idx].~Key();
        m_Values[idx].~Value();

        m_Occupied[idx] = false;
        m_Length--;
        return true;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    void RZHashMap<Key, Value, Hash, Equal>::clear()
    {
        for (size_type i = 0; i < m_Capacity; ++i) {
            if (m_Occupied[i]) {
                m_Keys[i].~Key();
                m_Values[i].~Value();
            }
        }

        memset(m_Occupied, 0, m_Capacity * sizeof(bool));
        m_Length = 0;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::reference RZHashMap<Key, Value, Hash, Equal>::operator[](const Key& key)
    {
        return at(key);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::const_reference RZHashMap<Key, Value, Hash, Equal>::operator[](const Key& key) const
    {
        return at(key);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::reference RZHashMap<Key, Value, Hash, Equal>::at(const Key& key)
    {
        RAZIX_CORE_ASSERT(m_Keys && m_Values && m_Occupied && m_Hashes,
            "[RZHashMap] Hash map is not initialized");

        size_type idx = find_entry(key);
        if (idx != m_Capacity)
            return m_Values[idx];

        // Key not found, insert with default value
        if (m_Length > m_Capacity / 2) {
            expand();
        }
        insert_entry(key, Value());
        idx = find_entry(key);

        RAZIX_CORE_ASSERT(idx != m_Capacity, "[RZHashMap] Inserted key not found after insertion");
        return m_Values[idx];
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::const_reference RZHashMap<Key, Value, Hash, Equal>::at(const Key& key) const
    {
        auto& self = const_cast<RZHashMap&>(*this);
        return self.at(key);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::size_type RZHashMap<Key, Value, Hash, Equal>::size() const
    {
        return m_Length;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::size_type RZHashMap<Key, Value, Hash, Equal>::capacity() const
    {
        return m_Capacity;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    bool RZHashMap<Key, Value, Hash, Equal>::empty() const
    {
        return m_Length == 0;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    float RZHashMap<Key, Value, Hash, Equal>::load_factor() const
    {
        return static_cast<float>(m_Length) / static_cast<float>(m_Capacity);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::iterator RZHashMap<Key, Value, Hash, Equal>::begin()
    {
        return iterator(m_Keys, m_Values, m_Occupied, m_Capacity, 0);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::iterator RZHashMap<Key, Value, Hash, Equal>::end()
    {
        return iterator(m_Keys, m_Values, m_Occupied, m_Capacity, m_Capacity);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    const typename RZHashMap<Key, Value, Hash, Equal>::const_iterator RZHashMap<Key, Value, Hash, Equal>::begin() const
    {
        // Returns a read-only iterator
        return const_iterator(m_Keys, m_Values, m_Occupied, m_Capacity, 0);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    const typename RZHashMap<Key, Value, Hash, Equal>::const_iterator RZHashMap<Key, Value, Hash, Equal>::end() const
    {
        return const_iterator(m_Keys, m_Values, m_Occupied, m_Capacity, m_Capacity);
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    bool RZHashMap<Key, Value, Hash, Equal>::erase(const iterator& it)
    {
        return remove(it.key());
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::size_type RZHashMap<Key, Value, Hash, Equal>::quadratic_probe(size_type index, size_type probe_count, size_type capacity)
    {
        return (index + probe_count * probe_count) % capacity;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    void RZHashMap<Key, Value, Hash, Equal>::insert_entry(const Key& key, const Value& value)
    {
        u64       hash_value  = m_Hash(key);    // invoke operator () for Razix::hash<T>
        size_type index       = hash_value % m_Capacity;
        size_type probe_count = 0;

        while (probe_count < m_Capacity) {
            if (!m_Occupied[index]) {
                // Empty slot found
                new (&m_Keys[index]) Key(key);
                new (&m_Values[index]) Value(value);
                m_Occupied[index] = true;
                m_Hashes[index]   = hash_value;
                m_Length++;
                return;
            }

            if (m_Hashes[index] == hash_value && m_Equal(m_Keys[index], key)) {
                // Key already exists, update value
                m_Values[index] = value;
                return;
            }
            probe_count++;
            index = quadratic_probe(index, probe_count, m_Capacity);
        }
        RAZIX_CORE_ERROR("[RZHashMap] Hash map is full, couldn't insert entry!");
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    bool RZHashMap<Key, Value, Hash, Equal>::expand()
    {
        size_type new_capacity = m_Capacity * RZ_HASH_MAP_GROWTH_FACTOR;

        if (new_capacity < m_Capacity) {
            RAZIX_CORE_ERROR("[RZHashMap] Capacity overflow!");
            return false;
        }

        Key*      new_keys     = static_cast<Key*>(rz_malloc_aligned(new_capacity * sizeof(Key)));
        Value*    new_values   = static_cast<Value*>(rz_malloc_aligned(new_capacity * sizeof(Value)));
        bool*     new_occupied = static_cast<bool*>(rz_malloc_aligned(new_capacity * sizeof(bool)));
        uint64_t* new_hashes   = static_cast<uint64_t*>(rz_malloc_aligned(new_capacity * sizeof(uint64_t)));

        RAZIX_CORE_ASSERT(new_keys && new_values && new_occupied && new_hashes,
            "[RZHashMap] Failed to allocate memory while expanding hash map");

        memset(new_occupied, 0x0, new_capacity * sizeof(bool));
        memset(new_hashes, 0x0, new_capacity * sizeof(uint64_t));

        Key*      old_keys     = m_Keys;
        Value*    old_values   = m_Values;
        bool*     old_occupied = m_Occupied;
        uint64_t* old_hashes   = m_Hashes;
        size_type old_capacity = m_Capacity;

        // Swap to new arrays
        m_Keys     = new_keys;
        m_Values   = new_values;
        m_Occupied = new_occupied;
        m_Hashes   = new_hashes;
        m_Capacity = new_capacity;
        m_Length   = 0;

        // Re-hash all entries into new arrays
        for (size_type i = 0; i < old_capacity; ++i) {
            if (old_occupied[i]) {
                insert_entry(rz_move(old_keys[i]), rz_move(Value(old_values[i])));
            }
        }

        // Free old arrays
        rz_free(old_keys);
        rz_free(old_values);
        rz_free(old_occupied);
        rz_free(old_hashes);

        return true;
    }

    template<typename Key, typename Value, typename Hash, typename Equal>
    typename RZHashMap<Key, Value, Hash, Equal>::size_type RZHashMap<Key, Value, Hash, Equal>::find_entry(const Key& key) const
    {
        u64       hash_value  = m_Hash(key);    // invoke operator () for Razix::hash<T>
        size_type index       = hash_value % m_Capacity;
        size_type probe_count = 0;
        while (probe_count < m_Capacity) {
            if (m_Occupied[index] && m_Hashes[index] == hash_value && m_Equal(m_Keys[index], key))
                return index;

            probe_count++;
            index = quadratic_probe(index, probe_count, m_Capacity);
        }
        return m_Capacity;
    }

}    // namespace Razix

#endif    // _RZ_HASH_MAP_H_
