#pragma once
#include "tools/include/compact_map.h"


namespace tools
{
    template<Sortable Key, typename Value>
    inline size_t compact_map<Key, Value>::find_index(const Key& key) const
    {
        auto it = std::lower_bound(_keys.begin(), _keys.end(), key);

        if (it != _keys.end() && *it == key)
        {
            return std::distance(_keys.begin(), it);
        }
        return npos;
    }

    template<Sortable Key, typename Value>
    inline void compact_map<Key, Value>::insert(const Key& key, const Value& value)
    {
        auto it = std::lower_bound(_keys.begin(), _keys.end(), key);

        size_t index = std::distance(_keys.begin(), it);

        if (it != _keys.end() && *it == key)
        {
            _values[index] = value;
        }
        else
        {
            _keys.insert(it, key);
            _values.insert(_values.begin() + index, value);
        }
    }




    template<Sortable Key, typename Value>
    Value& compact_map<Key, Value>::operator[](const Key& key)
    {
        auto it = std::lower_bound(_keys.begin(), _keys.end(), key);

        size_t index = std::distance(_keys.begin(), it);

        if (it != _keys.end() && *it == key)
        {
            return _values[index];
        }
        else
        {
            _keys.insert(it, key);
            _values.insert(_values.begin() + index, Value{});
            return _values[index];
        }
    }
    template<Sortable Key, typename Value>
    inline const Value& compact_map<Key, Value>::operator[](const Key& key) const
    {
        size_t index = find_index(key);

        assert(index != npos && "Key not found in const operator[]");

        return _values[index];
    }

    template<Sortable Key, typename Value>
    inline void compact_map<Key, Value>::remove(const Key& key)
    {
        size_t index = find_index(key);

        if (index != npos)
        {
            _keys.erase(_keys.begin() + index);
            _values.erase(_values.begin() + index);
        }
    }

    template<Sortable Key, typename Value>
    inline void compact_map<Key, Value>::clear()
    {
        _keys.clear();
        _values.clear();
    }

    template<Sortable Key, typename Value>
    inline size_t compact_map<Key, Value>::size() const
    {
        return _keys.size();
    }

    template<Sortable Key, typename Value>
    inline bool compact_map<Key, Value>::empty() const
    {
        return _keys.empty();
    }

    template<Sortable Key, typename Value>
    inline const std::vector<Key>& compact_map<Key, Value>::keys() const
    {
        return _keys;
    }

    template<Sortable Key, typename Value>
    inline const std::vector<Value>& compact_map<Key, Value>::values() const
    {
        return _values;
    }

    template<Sortable Key, typename Value>
    inline compact_map<Key, Value>::iterator compact_map<Key, Value>::begin()
    {
        return iterator(_keys, _values, 0);
    }

    template<Sortable Key, typename Value>
    inline compact_map<Key, Value>::iterator compact_map<Key, Value>::end()
    {
        return iterator(_keys, _values, _keys.size());
    }


    template<Sortable Key, typename Value>
    inline compact_map<Key, Value>::iterator::iterator(std::vector<Key>& keys, std::vector<Value>& values, size_t index)
        : _keys(keys), _values(values), _index(index) {
    }

    template<Sortable Key, typename Value>
    inline compact_map<Key, Value>::iterator::reference compact_map<Key, Value>::iterator::operator*() const
    {
        return { _keys[_index], _values[_index] };
    }

    template<Sortable Key, typename Value>
    inline compact_map<Key, Value>::iterator::self& compact_map<Key, Value>::iterator::operator++()
    {
        _index++;
        return *this;
    }

    template<Sortable Key, typename Value>
    inline compact_map<Key, Value>::iterator::self compact_map<Key, Value>::iterator::operator++(int)
    {
        auto tmp = *this;
        (*this)++;
        return tmp;
    }

    template<Sortable Key, typename Value>
    inline bool compact_map<Key, Value>::iterator::operator==(const self& other) const
    {
        return _index == other._index;
    }

    template<Sortable Key, typename Value>
    inline bool compact_map<Key, Value>::iterator::operator!=(const self& other) const
    {
        return !(*this == other);
    }









}