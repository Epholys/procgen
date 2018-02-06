#include "Observer.h"
#include <iostream>

struct A : Observable
{
    int n { 0 };
    void notif() { notify(); }
};

struct B : Observable
{
    int m { 0 };
    void notif() { notify(); }
};

struct C : public Observer<A>, public Observer<B>
{
    using lsys = Observer<A>;
    using map = Observer<B>;
    
    C(const std::shared_ptr<A>& a,
      const std::shared_ptr<B>& b);
    virtual ~C();

    virtual void handle_notification() const;
};
    
C::C(const std::shared_ptr<A>& a,
     const std::shared_ptr<B>& b)
    : lsys(a)
    , map(b)
{
    lsys::add_callback([this](){handle_notification();});
    std::cout << "C added callback A // id=" << lsys::id_.first << std::endl;
    map::add_callback([this](){handle_notification();});
    std::cout << "C added callback B // id=" << map::id_.first << std::endl;
}

C::~C()
{
}

void C::handle_notification() const
{
    std::cout << "***Cid:*** " << lsys::id_.first << std::endl;
    std::cout << "***Cid:*** " << map::id_.first << std::endl;
}


struct D : public Observer<A>
{
    using lsys = Observer<A>;
    
    D(const std::shared_ptr<A>& a);
    virtual ~D();

    virtual void handle_notification() const;
};
    
D::D(const std::shared_ptr<A>& a)
    : lsys(a)
{
    lsys::add_callback([this](){handle_notification();});
    std::cout << "D added callback A // id=" << lsys::id_.first << std::endl;
}

D::~D()
{
}

void D::handle_notification() const
{
    std::cout << "***Did:*** " << id_.first << std::endl;
}

int main()
{
    std::shared_ptr<A> a = std::make_shared<A>();
    std::shared_ptr<B> b = std::make_shared<B>();
    C c (a, b);
    D d (a);

    std::cout << "a.n: " << a->n << std::endl;
    std::cout << "b.m: " << b->m << std::endl;

    a->notif();

    b->notif();

    return 0;
}
