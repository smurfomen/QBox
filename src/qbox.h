#ifndef QBOX_H
#define QBOX_H

#include <functional>
#include <memory>

using namespace std;
class QBoxException: public logic_error {
public:
    explicit
    QBoxException(const char * msg)  _GLIBCXX_TXN_SAFE
        : logic_error(msg) { }

    explicit
    QBoxException(const std::string & msg) _GLIBCXX_TXN_SAFE
        : logic_error(msg) { }

    virtual ~QBoxException() _GLIBCXX_USE_NOEXCEPT { }
};

template <typename T>
class QBox {
    std::shared_ptr<T> val = nullptr;
public:
    using shared_base = typename std::shared_ptr<T>;
    using owner_base = typename std::shared_ptr<T>;
    using not_null_base = typename std::shared_ptr<T>;

    QBox(T * v)
        : val(v) {
    }

    QBox & operator=(T * v)
    {
        val = std::shared_ptr<T>(v);
        return *this;
    }

    /* shared reference on T */
    class shared : public shared_base {
    public:
        using base = typename QBox<T>::shared_base;

        shared(T * t)
            : base(t) { }

        shared(QBox & r)
            : base(r.val) { }

        shared(QBox && r)
            : base(std::move(r.val))  { }
    };

    /* garantees what reference is unique or throw exception */
    class owner : public owner_base {
    public:
        using base = typename QBox<T>::owner_base;

        owner(T * t)
            : base(t) { }

        owner(QBox & r)
            : base(nullptr)
        {
            if(!r.val.unique())
                throw QBoxException("QBox: non unique reference in owner container");

            this->swap(r.val);
        }

        owner(QBox && r)
            : base(nullptr)
        {
            if(!r.val.unique())
                throw QBoxException("QBox: non unique reference in owner container");

            this->swap(r.val);
        }
    };

    /* garantees what reference is not null or throw exception */
    class not_null : public not_null_base {
    public:
        using base = typename QBox<T>::not_null_base;

        not_null(T * t)
            : base (t)
        {
            if(this->get() == nullptr)
                throw QBoxException("QBox: null reference in not null container");
        }

        not_null(QBox & r)
            : base (r.val)
        {
            if(this->get() == nullptr)
                throw QBoxException("QBox: null reference in not null container");
        }

        not_null(QBox && r)
            : base (std::move(r.val))
        {
            if(this->get() == nullptr)
                throw QBoxException("QBox: null reference in not null container");
        }
    };
};



#endif // QBOX_H
