#pragma once

#include "Interfaces/OnlineExternalUIInterface.h"
#include <Containers/Map.h>
#include <utility>

namespace detail {
    template <typename R, typename... Args> class UnrealFunctor;

    template <typename R> class UnrealFunctor<R> {
        DECLARE_DELEGATE_RetVal(R, DelegateType)
    public:
        typedef DelegateType type;
    };
    template <> class UnrealFunctor<void> {
        DECLARE_DELEGATE(DelegateType)
    public:
        typedef DelegateType type;
    };

    template <typename R, typename P1> class UnrealFunctor<R, P1> {
        DECLARE_DELEGATE_RetVal_OneParam(R, DelegateType, P1)
    public:
        typedef DelegateType type;
    };
    template <typename P1> class UnrealFunctor<void, P1> {
        DECLARE_DELEGATE_OneParam(DelegateType, P1)
    public:
        typedef DelegateType type;
    };

    template <typename R, typename P1, typename P2> class UnrealFunctor<R, P1, P2> {
        DECLARE_DELEGATE_RetVal_TwoParams(R, DelegateType, P1, P2)
    public:
        typedef DelegateType type;
    };
    template <typename P1, typename P2> class UnrealFunctor<void, P1, P2> {
        DECLARE_DELEGATE_TwoParams(DelegateType, P1, P2)
    public:
        typedef DelegateType type;
    };

    template <typename R, typename P1, typename P2, typename P3> class UnrealFunctor<R, P1, P2, P3> {
        DECLARE_DELEGATE_RetVal_ThreeParams(R, DelegateType, P1, P2, P3)
    public:
        typedef DelegateType type;
    };
    template <typename P1, typename P2, typename P3> class UnrealFunctor<void, P1, P2, P3> {
        DECLARE_DELEGATE_ThreeParams(DelegateType, P1, P2, P3)
    public:
        typedef DelegateType type;
    };
} //namespace detail

template <typename Ret, typename... Args>
class DelegateChain {
public:
    typedef Ret return_type;
    typedef typename detail::UnrealFunctor<return_type, Args...>::type delegate_type;
    typedef decltype(std::declval<TMap<int, delegate_type>>().Num()) size_type;

    DelegateChain() = default;
    ~DelegateChain() noexcept = default;

    template <typename... Args2> return_type evaluate (Args2&&... args) const;
    size_type size() const { return m_delegates.Num(); }
    size_type push_back (delegate_type&& delegate);
    bool erase (size_type index);

private:
    TMap<size_type, delegate_type> m_delegates;
    size_type m_next_index{};
};

template <typename Ret, typename... Args>
template <typename... Args2>
inline auto DelegateChain<Ret, Args...>::evaluate (Args2&&... args) const -> return_type {
    for (const TPair<size_type, delegate_type>& p : m_delegates) {
        const auto& delegate = p.Value;
        const Ret ret = delegate.Execute(std::forward<Args2>(args)...);
        if (ret) {
            return ret;
        }
    }
    return {};
}

template <typename Ret, typename... Args>
inline auto DelegateChain<Ret, Args...>::push_back (delegate_type&& delegate) -> size_type {
    if (delegate.IsBound()) {
        const size_type index = m_next_index++;
        m_delegates.Add(index, std::move(delegate));
        return index;
    }
    else {
        return static_cast<size_type>(-1);
    }
}

template <typename Ret, typename... Args>
inline bool DelegateChain<Ret, Args...>::erase (size_type index) {
    delegate_type* const item = m_delegates.Find(index);
    if (item) {
        m_delegates.Remove(index);
        return true;
    }
    return false;
}
