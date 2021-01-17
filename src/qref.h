#ifndef QREFERENCEWRAPPER_H
#define QREFERENCEWRAPPER_H

#include <functional>
#include <memory>

using namespace std;
class QRefException: public logic_error {
public:
    explicit
    QRefException(const char * msg)  _GLIBCXX_TXN_SAFE
        : logic_error(msg) { }

    explicit
    QRefException(const std::string & msg) _GLIBCXX_TXN_SAFE
        : logic_error(msg) { }

    virtual ~QRefException() _GLIBCXX_USE_NOEXCEPT { }
};

template <typename T>
class QRef {
    std::shared_ptr<T> val = nullptr;
public:
    using shared_base = typename std::shared_ptr<T>;
    using owner_base = typename std::shared_ptr<T>;
    using not_null_base = typename std::shared_ptr<T>;

    QRef(T * v)
        : val(v) {
    }

    QRef & operator=(T * v)
    {
        val = std::shared_ptr<T>(v);
        return *this;
    }

    /* shared reference on T */
    class shared : public shared_base {
    public:
        using base = typename QRef<T>::shared_base;

        shared(T * t)
            : base(t) { }

        shared(QRef & r)
            : base(r.val) { }

        shared(QRef && r) noexcept
            : base(std::move(r.val))  { }
    };

    /* garantees what reference is unique or throw exception */
    class owner : public owner_base {
    public:
        using base = typename QRef<T>::owner_base;

        owner(T * t)
            : base(t) { }

        owner(QRef & r)
            : base(nullptr)
        {
            if(!r.val.unique())
                throw QRefException("QRef: non unique reference in owner container");

            this->swap(r.val);
        }

        owner(QRef && r)
            : base(nullptr)
        {
            if(!r.val.unique())
                throw QRefException("QRef: non unique reference in owner container");

            this->swap(r.val);
        }
    };

    /* garantees what reference is not null or throw exception */
    class not_null : public not_null_base {
    public:
        using base = typename QRef<T>::not_null_base;

        not_null(T * t)
            : base (t)
        {
            if(this->get() == nullptr)
                throw QRefException("QRef: null reference in not null container");
        }

        not_null(QRef & r)
            : base (r.val)
        {
            if(this->get() == nullptr)
                throw QRefException("QRef: null reference in not null container");
        }

        not_null(QRef && r)
            : base (std::move(r.val))
        {
            if(this->get() == nullptr)
                throw QRefException("QRef: null reference in not null container");
        }
    };
};



#endif // QREFERENCEWRAPPER_H
