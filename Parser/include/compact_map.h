#pragma once

namespace smallVecMap
{
    template<typename T>
    concept Sortable = requires(T a, T b) {
        { a < b } -> std::convertible_to<bool>;
    };

    template<Sortable Key, typename Value>
    class compact_map {
    public:
        compact_map() = default;

        // Find index using binary search
        size_t find_index(const Key& key) const;

        // Insert or update key-value pair
        void insert(const Key& key, const Value& value);

        // operator[] inserts default if not found
        Value& operator[](const Key& key);

        const Value& operator[](const Key& key) const;

        void remove(const Key& key);

        void clear();

        size_t size() const;

        bool empty() const;

        const std::vector<Key>& keys() const;

        const std::vector<Value>& values() const;

        struct iterator 
        {
            using self = iterator;
            using value_type = std::pair<const Key&, Value&>;
            using reference = value_type;
            using pointer = void;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::random_access_iterator_tag;

            iterator(std::vector<Key>& keys, std::vector<Value>& values, size_t index);

            reference operator*() const;

            self& operator++();

            self operator++(int);

            bool operator==(const self& other) const;

            bool operator!=(const self& other) const;

        private:
            std::vector<Key>& _keys;
            std::vector<Value>& _values;
            size_t _index;
        };

        iterator begin();

        iterator end();

        static constexpr size_t npos = static_cast<size_t>(-1);

    private:
        std::vector<Key> _keys;
        std::vector<Value> _values;
    };

}

#include "compact_map.inl"