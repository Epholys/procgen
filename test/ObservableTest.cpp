#include <gtest/gtest.h>

#include "Observer.h"
#include "Observable.h"

using namespace std;

struct A : public Observable
{
    int n { -1 };
    A(int nn)
        : n(nn)
        {}
    
    void increment()
        {
            ++n;
            notify();
        }
    bool empty() const
        {
            return observers_.empty();
        }
};

struct B : public Observable
{
    int n { -1 };

    B(int nn)
        : n(nn)
        {}
    void increment()
        {
            ++n;
            notify();
        }
    bool empty() const
        {
            return observers_.empty();
        }
};
    
struct C : public Observer<A>
{
    C(const shared_ptr<A>& a)
        : Observer<A>(a)
        {
            Observer<A>::add_callback([this](){update();});
        }
        
    void update()
        {
            n = Observer<A>::get_target()->n;
        }

    int n { -1 };
};

struct D : public Observer<A>, public Observer<B>
{
    D(const shared_ptr<A>& a, const shared_ptr<B>& b )
        : Observer<A>(a)
        , Observer<B>(b)

        {
            Observer<A>::add_callback([this](){update_n();});
            Observer<B>::add_callback([this](){update_m();});
        }
        
    void update_n()
        {
            n = Observer<A>::get_target()->n;
        }
    void update_m()
        {
            m = Observer<B>::get_target()->n;
        }

    int n { -1 };
    int m { -1 };
};

TEST(ObservableTest, one_to_one)
{
    auto a = std::make_shared<A>(0);
    {
        C c (a);
        a->increment();
        ASSERT_EQ(c.n, 1);
    }
    ASSERT_TRUE(a->empty());
}

TEST(ObservableTest, multiple)
{
    auto a1 = std::make_shared<A>(1);
    auto a2 = std::make_shared<A>(2);
    auto b3 = std::make_shared<B>(3);
    {
        C c1_1 (a1);
        C c1_2 (a1);
        C c2 (a2);
        D d13 (a1, b3); 
        D d23 (a2, b3); 

        a1->increment();
        ASSERT_EQ(c1_1.n, 2);
        ASSERT_EQ(c1_2.n, 2);
        ASSERT_EQ(d13.n, 2);

        a2->increment();
        ASSERT_EQ(c2.n, 3);
        ASSERT_EQ(d23.n, 3);

        b3->increment();
        ASSERT_EQ(d13.m, 4);
        ASSERT_EQ(d23.m, 4);
    }
    ASSERT_TRUE(a1->empty());
    ASSERT_TRUE(a2->empty());
    ASSERT_TRUE(b3->empty());
}

TEST(ObservableTest, set_target)
{
    auto a1 = std::make_shared<A>(0);
    auto a2 = std::make_shared<A>(0);
    C c (a1);

    c.set_target(a2);
    c.add_callback([&c](){c.update();});
    a1->increment();
    a2->increment();

    ASSERT_TRUE(a1->empty());
    ASSERT_FALSE(a2->empty());
    ASSERT_EQ(1, c.n);
}
