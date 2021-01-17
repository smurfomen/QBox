#include <QCoreApplication>
#include <qref.h>
#include <QDebug>
struct A
{
    int digit = 10;

    A() {
        qDebug()<<"created";
    }
    ~A() {
        qDebug()<<"deleted";
    }
};


/* compiled, but is not useful becouse inner ref types do not have (const QRef &) constructor
 * QRef<A>::shared s = lvalue_ref;
 * QRef<A>::owner o = lvalue_ref;
 * QRef<A>::not_null nn = lvalue_ref; */
void not_compile(const QRef<A> & lvalue_ref) {

}


/* correct compiled becouse shared have QRef & constructor
 * QRef<A> ref = new A();
 * compile(ref);            */
void compile(QRef<A> & lvalue_ref) {
    QRef<A>::shared s = lvalue_ref; //
    qDebug()<< "non const lvalue reference QRef";
}


/* compile(QRef<A>(new A()));
 * compile(new A());            */
void compile(QRef<A> && rvalue_ref) {
    QRef<A>::shared s = rvalue_ref; // correct compilled
//    QRef<A>::owner o = rvalue_ref; // throw exception std::invalid_argument("non unique reference") becouse exists any shared
    qDebug()<< "rvalue reference QRef";
}

void non_null(QRef<A>::not_null n_ref)
{
    qDebug() << "non null" << n_ref->digit;
}

/* use case:
 * QRef<A> ref = new A();
 * shared(ref); - make shared reference to use in this function
 * own(ref); - make owned reference to use in this function and delete this after execution */
void shared(QRef<A>::shared s_ref)
{
    qDebug() <<"shared" << s_ref->digit;
}

/* use case:
 * own(new A()) and after own execute created A object will be deleted */
void own(QRef<A>::owner u_ref)
{    
    qDebug() << "owner" << u_ref->digit;
}


/* use case:
 * test(new A()); - create owner QRef and using this how own this */
void test(QRef<A>::owner f)
{
    qDebug()<<"test"<<f->digit;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QRef<A> ref = new A();
    shared(ref);
    try {
        QRef<A>::shared s = ref;
        own(ref);
    } catch (QRefException & e) {
        qDebug()<< e.what(); // not unique reference
    }

    own(ref);
    own(QRef<A>(new A()));
    shared(QRef<A>(new A()));
    non_null(QRef<A>(new A()));

    compile(ref);

    // equial:
    /* 1 */ compile(new A());
    /* 2 */ compile(QRef<A>(new A()));

    test(new A());

    try {
        non_null(nullptr);
    } catch(QRefException & e) {
        qDebug()<<e.what(); // null in not null container
    }

    return a.exec();
}
