#pragma once

#include <Containers/Array.h>
#include <type_traits>
#include <ciso646>

template <typename T, bool Const=false>
class StdArray;

namespace detail {
    template <typename TArr>
    class StdArrayIterator {
        friend class StdArrayIterator<typename std::add_const<TArr>::type>;
        typedef std::decay_t<decltype(std::declval<TArr>()[0])> raw_value_type;
        friend class StdArray<raw_value_type, true>;
        friend class StdArray<raw_value_type, false>;
    public:
        typedef typename std::conditional<std::is_const_v<TArr>, std::add_const_t<raw_value_type>, raw_value_type>::type value_type;
        typedef decltype(std::declval<TArr>().Num()) size_type;
        typedef std::make_signed_t<size_type> difference_type;
        typedef std::random_access_iterator_tag iterator_category;
        typedef typename std::add_lvalue_reference<value_type>::type reference;
        typedef typename std::add_pointer<value_type>::type pointer;
        typedef std::add_const_t<value_type>* const_pointer;

        StdArrayIterator() : m_array(nullptr), m_index(0) {}
        StdArrayIterator (const StdArrayIterator&) = default;

        operator bool() const { return nullptr != m_array; }

        template <typename TArr2, typename=typename std::enable_if<not std::is_const_v<TArr2> and std::is_const_v<TArr>>::type>
        StdArrayIterator (const StdArrayIterator<TArr2>& other) :
            m_array(other.m_array),
            m_index(other.m_index)
        {
        }

        template <typename TArr2>
        StdArrayIterator (TArr2* other, typename std::enable_if<not std::is_const_v<TArr2> and std::is_const_v<TArr>, size_type>::type index) :
            m_array(other),
            m_index(index)
        {
        }

        StdArrayIterator(TArr* arr, size_type index) :
            m_array(arr), m_index(index)
        {
        }

        StdArrayIterator& operator= (const StdArrayIterator&) = default;
        template <typename TArr2> typename std::enable_if<not std::is_const_v<TArr2> and std::is_const_v<TArr>, StdArrayIterator>::type& operator= (const StdArrayIterator<TArr2>& other) {
            m_array = other.m_array;
            m_index = other.m_index;
            return *this;
        }

        value_type& operator*() { return (*m_array)[m_index]; }
        std::add_const_t<value_type>& operator*() const { return (*m_array)[m_index]; }
        pointer operator->() { return &(*m_array)[m_index]; }
        const_pointer operator->() const { return &(*m_array)[m_index]; }
        StdArrayIterator& operator++() { ++m_index; return *this; }
        StdArrayIterator& operator--() { --m_index; return *this; }
        StdArrayIterator operator++(int) { StdArrayIterator ret(*this); ++m_index; return ret; }
        StdArrayIterator operator--(int) { StdArrayIterator ret(*this); --m_index; return ret; }
        StdArrayIterator& operator+= (difference_type offs) { m_index += offs; return *this; }
        StdArrayIterator& operator-= (difference_type offs) { m_index -= offs; return *this; }
        difference_type operator- (const StdArrayIterator<TArr>& other) const { return this->m_index - other.m_index; }
        bool operator== (const StdArrayIterator& other) const { return m_array == other.m_array and m_index == other.m_index; }
        bool operator!= (const StdArrayIterator& other) const { return m_array != other.m_array or m_index != other.m_index; }

    private:
        TArr* m_array;
        size_type m_index;
    };

    template <typename TArr>
    inline StdArrayIterator<TArr> operator+ (StdArrayIterator<TArr> left, typename StdArrayIterator<TArr>::difference_type right) {
        left += right;
        return left;
    }

    template <typename TArr>
    inline StdArrayIterator<TArr> operator- (StdArrayIterator<TArr> left, typename StdArrayIterator<TArr>::difference_type right) {
        left -= right;
        return left;
    }
} //namespace detail

template <typename T, bool Const>
class StdArray {
public:
    typedef typename std::conditional<Const, const TArray<T>, TArray<T>>::type array_type;
    typedef decltype(std::declval<array_type>().Num()) size_type;
    typedef T value_type;
    typedef detail::StdArrayIterator<array_type> iterator;
    typedef detail::StdArrayIterator<typename std::add_const<array_type>::type> const_iterator;

    explicit StdArray (array_type* inner) : m_array(inner) {}
    ~StdArray() noexcept = default;

    value_type& operator[] (size_type index) { return (*m_array)[index]; }
    const value_type& operator[] (size_type index) const { return (*m_array)[index]; }

    array_type& array() { return *m_array; }
    typename std::add_const<array_type>::type array() const { return *m_array; }

    size_type size() const { return m_array->Num(); }
    bool empty() const { return 0 == this->size(); }

    iterator begin() { return iterator{m_array, 0}; }
    iterator end() { return iterator{m_array, this->size()}; }
    const_iterator begin() const { return const_iterator{m_array, 0}; }
    const_iterator end() const { return const_iterator{m_array, this->size()}; }
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend() const { return this->end(); }
    void clear() { m_array->Empty(); }

    iterator insert (const_iterator before, value_type&& item);
    iterator insert (const_iterator before, const value_type& item);
    void push_back (value_type&& item);
    void push_back (const value_type& item);
    template <bool C=Const> std::enable_if_t<C, iterator> erase (iterator first, iterator last);
    iterator erase (const_iterator first, const_iterator last);

private:
    array_type* m_array;
};

template <typename T, bool Const>
inline auto StdArray<T, Const>::insert (const_iterator before, value_type&& item) -> iterator {
    m_array->Insert(std::move(item), before.m_index);
    return iterator{m_array, before.m_index};
}

template <typename T, bool Const>
inline auto StdArray<T, Const>::insert (const_iterator before, const value_type& item) -> iterator {
    m_array->Insert(item, before.m_index);
    return iterator{m_array, before.m_index};
}

template <typename T, bool Const>
inline void StdArray<T, Const>::push_back (value_type&& item) {
    m_array->Emplace(std::move(item));
}

template <typename T, bool Const>
inline void StdArray<T, Const>::push_back (const value_type& item) {
    m_array->Add(item);
}

template <typename T, bool Const>
template <bool C>
inline auto StdArray<T, Const>::erase (iterator first, iterator last) -> std::enable_if_t<C, iterator> {
    return this->erase(const_iterator(first), const_iterator(last));
}

template <typename T, bool Const>
inline auto StdArray<T, Const>::erase (const_iterator first, const_iterator last) -> iterator {
    const size_type& dele_idx = first.m_index;
    const size_type dele_count = static_cast<size_type>(last - first);
    for (size_type z = 0; z < dele_count; ++z) {
        m_array->RemoveAt(dele_idx);
    }
    return this->begin() + dele_idx;
}
