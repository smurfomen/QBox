#include <QCoreApplication>
#include <qbox.h>
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
void not_compile(const QBox<A> & lvalue_ref) {

}


/* correct compiled becouse shared have QRef & constructor
 * QRef<A> ref = new A();
 * compile(ref);            */
void compile(QBox<A> & lvalue_ref) {
    QBox<A>::shared s = lvalue_ref; //
    qDebug()<< "non const lvalue reference QRef";
}


/* compile(QRef<A>(new A()));
 * compile(new A());            */
void compile(QBox<A> && rvalue_ref) {
    QBox<A>::shared s = rvalue_ref; // correct compilled
//    QRef<A>::owner o = rvalue_ref; // throw exception std::invalid_argument("non unique reference") becouse exists any shared
    qDebug()<< "rvalue reference QRef";
}

void non_null(QBox<A>::not_null n_ref)
{
    qDebug() << "non null" << n_ref->digit;
}

/* use case:
 * QRef<A> ref = new A();
 * shared(ref); - make shared reference to use in this function
 * own(ref); - make owned reference to use in this function and delete this after execution */
void shared(QBox<A>::shared s_ref)
{
    qDebug() <<"shared" << s_ref->digit;
}

/* use case:
 * own(new A()) and after own execute created A object will be deleted */
void own(QBox<A>::owner u_ref)
{    
    qDebug() << "owner" << u_ref->digit;
}


/* use case:
 * test(new A()); - create owner QRef and using this how own this */
void test(QBox<A>::owner f)
{
    qDebug()<<"test"<<f->digit;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QBox<A> ref = new A();
    shared(ref);
    try {
        QBox<A>::shared s = ref;
        own(ref);
    } catch (QBoxException & e) {
        qDebug()<< e.what(); // not unique reference
    }

    own(ref);
    own(QBox<A>(new A()));
    shared(QBox<A>(new A()));
    non_null(QBox<A>(new A()));

    compile(ref);

    // equial:
    /* 1 */ compile(new A());
    /* 2 */ compile(QBox<A>(new A()));

    test(new A());

    try {
        non_null(nullptr);
    } catch(QBoxException & e) {
        qDebug()<<e.what(); // null in not null container
    }

    return a.exec();
}
