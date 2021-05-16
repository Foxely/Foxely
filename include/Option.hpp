

#ifndef OPTION_HPP_
#define OPTION_HPP_

#include <functional>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace fox
{
    struct nullopt_t {
        constexpr explicit nullopt_t(int) {}
    };
    constexpr nullopt_t nullopt{42};
    namespace base {
        template <typename T>
        class OptionBase {
        public:
            constexpr OptionBase() noexcept = default;

            OptionBase(OptionBase&&) = delete;

            ~OptionBase()
            {
                reset();
            }

            bool is_just() const
            {
                return m_bIsJust;
            }

            template <typename... Args>
            void set_val(Args&&... args)
            {
                new (&m_pValue) T(std::forward<Args>(args)...);
                m_bIsJust = true;
            }

            T* get_val()
            {
                if (m_bIsJust)
                    return reinterpret_cast<T*>(&m_pValue);
                return nullptr;
            }

            const T* get_val() const
            {
                if (m_bIsJust)
                    return reinterpret_cast<const T*>(&m_pValue);
                return nullptr;
            }

            void reset()
            {
                if (m_bIsJust) {
                    get_val()->~T();
                    m_bIsJust = false;
                }
            }

        private:
            bool m_bIsJust = false;
            alignas(T) unsigned char m_pValue[sizeof(T)];
        };

        template <typename T>
        class OptionBase<T&> {
        public:
            constexpr OptionBase() noexcept = default;

            ~OptionBase()
            {
                reset();
            }

            bool is_just() const
            {
                return m_bIsJust;
            }

            void set_val(std::add_lvalue_reference_t<T> ref)
            {
                m_pValue = std::addressof(ref);
                m_bIsJust = true;
            }

            std::add_pointer_t<T> get_val()
            {
                return m_pValue;
            }

            std::add_const_t<std::add_pointer_t<T>> get_val() const
            {
                return m_pValue;
            }

            void reset()
            {
                if (m_bIsJust) {
                    m_pValue = nullptr;
                    m_bIsJust = false;
                }
            }

        private:
            bool m_bIsJust = false;
            std::add_pointer_t<T> m_pValue = nullptr;
        };
    }

    template <typename T>
    class Option;

    template <typename T>
    constexpr Option<T> null();

    template <typename T>
    constexpr Option<T> just(T&& value);

    template <typename T>
    class Option : public base::OptionBase<T>
    {
    public:
        constexpr Option() noexcept : base::OptionBase<T>()
        {
        }

        ~Option() = default;

        template <typename U>
        Option(Option<U>&& other) : base::OptionBase<T>()
        {
            if (other.is_just()) {
                base::OptionBase<T>::set_val(std::forward<U>(other.value()));
            }
        }

        T& value()
        {
            if (is_null())
                throw std::runtime_error("Option::value() is a null value");

            return *base::OptionBase<T>::get_val();
        }

        const T& value() const
        {
            if (is_null())
                throw std::runtime_error("Option::value() is a null value");

            return *base::OptionBase<T>::get_val();
        }

        Option<T&> as()
        {
            Option<T&> opt;

            if (is_just())
                opt.set(*base::OptionBase<T>::get_val());
            return opt;
        }

        Option<const T&> as_ref() const
        {
            Option<const T&> opt;

            if (is_just())
                opt.set(*base::OptionBase<T>::get_val());
            return opt;
        }

        T value_or(T oValue)
        {
            if (is_just())
                return std::forward<T>(value());
            return std::forward<T>(oValue);
        }

        T value_or_null()
        {
            if (is_just())
                return std::forward<T>(value());
            return {};
        }

        bool is_just() const
        {
            return base::OptionBase<T>::is_just();
        }

        bool is_null() const
        {
            return !is_just();
        }

        template <typename... Args>
        void set(Args&&... args)
        {
            base::OptionBase<T>::set_val(std::forward<Args>(args)...);
        }

        template <typename F>
        decltype(auto) convert(F&& func)
        {
            if (is_just())
                return just(func(value()));
            return null<decltype(func(value()))>();
        }

        explicit operator bool() const
        {
            return is_just();
        }

        template <typename U>
        Option<T>& operator=(Option<U>&& other)
        {
            if (other)
                set(other.value());
            else
                base::OptionBase<T>::reset();
            return *this;
        }

        Option<T>& operator=(const T& value)
        {
            set(value);
            return *this;
        }

        Option<T>& operator=(const nullopt_t& value)
        {
            base::OptionBase<T>::reset();
            return *this;
        }

        template <typename U>
        bool operator==(const Option<U>& other) const
        {
            if (is_null() || other.is_null())
                return is_null() && other.is_null();

            const T& lhs = value();
            const U& rhs = value();

            return lhs == rhs;
        }

        bool operator==(const T& rhs) const
        {
            if (is_null())
                return false;
            const T& lhs = value();
            return lhs == rhs;
        }

        bool operator==(const nullopt_t& value)
        {
            return is_null();
        }

        std::add_pointer_t<T> operator->()
        {
            return base::OptionBase<T>::get_val();
        }

        std::add_const_t<std::add_pointer_t<T>> operator->() const
        {
            return base::OptionBase<T>::get_val();
        }

        const T& operator *() const&
        {
            return *base::OptionBase<T>::get_val();
        }

        T& operator *() &
        {
            return *base::OptionBase<T>::get_val();
        }

        T&& operator *() &&
        {
            return *base::OptionBase<T>::get_val();
        }
    };

    template <typename T>
    constexpr Option<T> null()
    {
        return {};
    }

    template <typename T>
    constexpr Option<T> just(T&& value)
    {
        Option<T> opt;

        opt.set(std::forward<T>(value));
        return opt;
    }

    template <typename T>
    std::ostream& operator<<(::std::ostream& os, const Option<T>& opt)
    {
        if (opt) {
            return os << "just(" << *opt.get_val() << ")";
        } else {
            return os << "null";
        }
    }
} // namespace fox

namespace std {

template <typename T>
struct hash<fox::Option<T>> {
    using base_type = std::remove_cv_t<std::remove_reference_t<T>>;

    std::size_t operator()(const fox::Option<T>& opt) const
    {
        std::size_t hash = 17;
        hash = hash * 31 + std::hash<bool>()(opt.is_just());
        if (opt)
            hash = hash * 31 + std::hash<base_type>()(opt.value());
        return hash;
    }
};

}

#endif