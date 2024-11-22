#ifndef RZSTL_CONST_MAP_H
#define RZSTL_CONST_MAP_H

#include "RZSTL/config.h"

namespace Razix {
    namespace rzstl {

#if defined WIN32 || defined _WIN64
        typedef unsigned __int64 size_t;
        typedef __int64          ptrdiff_t;
        typedef __int64          intptr_t;
#endif
        // [Source] : https://github.com/mapbox/eternal/tree/master

        namespace Internal {    // Same logic as EASTL for internal implementations
            template<typename T>
            constexpr void swap(T& a, T& b) noexcept
            {
                T tmp{a};
                a = b;
                b = tmp;
            }

            template<typename Key>
            class compare_key
            {
            public:
                const Key key;

                constexpr compare_key(const Key& key_) noexcept
                    : key(key_)
                {
                }

                template<typename Element>
                constexpr bool operator<(const Element& rhs) const noexcept
                {
                    return key < rhs->first;
                }
            };

            template<typename Key, typename Value>
            class element
            {
            public:
                using key_type         = Key;
                using mapped_type      = Value;
                using value_type       = std::pair<key_type, mapped_type>;
                using compare_key_type = compare_key<key_type>;

                constexpr element(const key_type& key, const mapped_type& value) noexcept
                    : pair(key, value)
                {
                }

                constexpr bool operator<(const element& rhs) const noexcept
                {
                    return pair.first < rhs.pair.first;
                }

                constexpr bool operator<(const compare_key_type& rhs) const noexcept
                {
                    return pair.first < rhs.key;
                }

                constexpr const auto& operator*() const noexcept
                {
                    return pair;
                }

                constexpr const auto* operator->() const noexcept
                {
                    return &pair;
                }

                constexpr void swap(element& rhs) noexcept
                {
                    swap(pair.first, rhs.pair.first);
                    swap(pair.second, rhs.pair.second);
                }

            private:
                value_type pair;
            };

            template<typename Element>
            class iterator
            {
            public:
                constexpr iterator(const Element* pos_) noexcept
                    : pos(pos_)
                {
                }

                constexpr bool operator==(const iterator& rhs) const noexcept
                {
                    return pos == rhs.pos;
                }

                constexpr bool operator!=(const iterator& rhs) const noexcept
                {
                    return pos != rhs.pos;
                }

                constexpr iterator& operator++() noexcept
                {
                    ++pos;
                    return *this;
                }

                constexpr iterator& operator+=(std::size_t i) noexcept
                {
                    pos += i;
                    return *this;
                }

                constexpr iterator operator+(std::size_t i) const noexcept
                {
                    return pos + i;
                }

                constexpr iterator& operator--() noexcept
                {
                    --pos;
                    return *this;
                }

                constexpr iterator& operator-=(std::size_t i) noexcept
                {
                    pos -= i;
                    return *this;
                }

                constexpr std::size_t operator-(const iterator& rhs) const noexcept
                {
                    return pos - rhs.pos;
                }

                constexpr const auto& operator*() const noexcept
                {
                    return **pos;
                }

                constexpr const auto* operator->() const noexcept
                {
                    return &**pos;
                }

            private:
                const Element* pos;
            };

            template<typename Compare, typename Iterator, typename Key>
            constexpr auto bound(Iterator left, Iterator right, const Key& key) noexcept
            {
                std::size_t count = right - left;
                while (count > 0) {
                    const std::size_t step = count / 2;
                    right                  = left + step;
                    if (Compare()(*right, key)) {
                        left = ++right;
                        count -= step + 1;
                    } else {
                        count = step;
                    }
                }
                return left;
            }

            struct less
            {
                template<typename A, typename B>
                constexpr bool operator()(const A& a, const B& b) const noexcept
                {
                    return a < b;
                }
            };

            struct greater_equal
            {
                template<typename A, typename B>
                constexpr bool operator()(const A& a, const B& b) const noexcept
                {
                    return !(b < a);
                }
            };

            template<typename Element, size_t N>
            class constexpr_map
            {
            private:
                static_assert(N > 0, "constexpr_map is empty");

                Element data_[N];

                template<typename T, std::size_t... I>
                constexpr constexpr_map(const T (&data)[N], std::index_sequence<I...>) noexcept
                    : data_{{data[I].first, data[I].second}...}
                {
                    static_assert(sizeof...(I) == N, "index_sequence has identical length");
                    // Yes, this is a bubblesort. It's usually evaluated at compile-time, it's fast for data
                    // that is already sorted (like static maps), it has a small code size, and it's stable.
                    for (auto left = data_, right = data_ + N - 1; data_ < right; right = left, left = data_) {
                        for (auto it = data_; it < right; ++it) {
                            if (it[1] < it[0]) {
                                it[0].swap(it[1]);
                                left = it;
                            }
                        }
                    }
                }

                using compare_key_type = typename Element::compare_key_type;

            public:
                template<typename T>
                constexpr constexpr_map(const T (&data)[N]) noexcept
                    : constexpr_map(data, eastl::make_index_sequence<N>())
                {
                }

                using key_type        = typename Element::key_type;
                using mapped_type     = typename Element::mapped_type;
                using value_type      = typename Element::value_type;
                using size_type       = size_t;
                using difference_type = ptrdiff_t;
                using const_reference = const value_type&;
                using const_pointer   = const value_type*;
                using const_iterator  = iterator<Element>;

                constexpr bool unique() const noexcept
                {
                    for (auto right = data_ + N - 1, it = data_; it < right; ++it) {
                        if (!(it[0] < it[1])) {
                            return false;
                        }
                    }
                    return true;
                }

                constexpr const mapped_type& at(const key_type& key) const noexcept
                {
                    return find(key)->second;
                }

                constexpr std::size_t size() const noexcept
                {
                    return N;
                }

                constexpr const_iterator begin() const noexcept
                {
                    return data_;
                }

                constexpr const_iterator cbegin() const noexcept
                {
                    return begin();
                }

                constexpr const_iterator end() const noexcept
                {
                    return data_ + N;
                }

                constexpr const_iterator cend() const noexcept
                {
                    return end();
                }

                constexpr const_iterator lower_bound(const key_type& key) const noexcept
                {
                    return bound<less>(data_, data_ + N, compare_key_type{key});
                }

                constexpr const_iterator upper_bound(const key_type& key) const noexcept
                {
                    return bound<greater_equal>(data_, data_ + N, compare_key_type{key});
                }

                constexpr eastl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const noexcept
                {
                    const compare_key_type compare_key{key};
                    auto                   first = bound<less>(data_, data_ + N, compare_key);
                    return {first, bound<greater_equal>(first, data_ + N, compare_key)};
                }

                constexpr size_t count(const key_type& key) const noexcept
                {
                    const auto range = equal_range(key);
                    return range.second - range.first;
                }

                constexpr const_iterator find(const key_type& key) const noexcept
                {
                    const compare_key_type compare_key{key};
                    auto                   it = bound<less>(data_, data_ + N, compare_key);
                    if (it != data_ + N && greater_equal()(*it, compare_key)) {
                        return it;
                    } else {
                        return end();
                    }
                }

                constexpr bool contains(const key_type& key) const noexcept
                {
                    return find(key) != end();
                }
            };
        }    // namespace Internal

        template<typename Key, typename Value, size_t N>
        static constexpr auto constexpr_map(const eastl::pair<const Key, const Value> (&items)[N]) noexcept
        {
            return Internal::constexpr_map<Internal::element<Key, Value>, N>(items);
        }
    }    // namespace rzstl
}    // namespace Razix
#endif
