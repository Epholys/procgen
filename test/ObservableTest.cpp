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
    
struct A_Observer : public Observer<A>
{
    A_Observer(const shared_ptr<A>& a)
        : Observer<A>(a)
        {
            add_callback([this](){update();});
        }
        
    void update()
        {
            n = get_target()->n;
        }

    int n { -1 };
};

struct A_B_Observer : public Observer<A>, public Observer<B>
{
    A_B_Observer(const shared_ptr<A>& a, const shared_ptr<B>& b )
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

TEST(ObservableTest, observable_ctors)
{
    auto a1 = std::make_shared<A>(0);
    A_Observer a_obs (a1);
    auto a2 (*a1);
    A a3 (0);
    a3 = *a1;

    ASSERT_FALSE(a1->empty());
    ASSERT_TRUE(a2.empty());
    ASSERT_TRUE(a3.empty());
}

TEST(ObservableTest, one_to_one)
{
    auto a = std::make_shared<A>(0);
    {
        A_Observer a_obs (a);
        a->increment();
        ASSERT_EQ(a_obs.n, 1);
    }
    ASSERT_TRUE(a->empty());
}

TEST(ObservableTest, multiple)
{
    auto a1 = std::make_shared<A>(1);
    auto a2 = std::make_shared<A>(2);
    auto b = std::make_shared<B>(3);
    {
        A_Observer a1_obs_1 (a1);
        A_Observer a1_obs_2 (a1);
        A_Observer a2_obs (a2);
        A_B_Observer a1_b_obs (a1, b); 
        A_B_Observer a2_b_obs (a2, b); 

        a1->increment();
        ASSERT_EQ(a1_obs_1.n, 2);
        ASSERT_EQ(a1_obs_2.n, 2);
        ASSERT_EQ(a1_b_obs.n, 2);

        a2->increment();
        ASSERT_EQ(a2_obs.n, 3);
        ASSERT_EQ(a2_b_obs.n, 3);

        b->increment();
        ASSERT_EQ(a1_b_obs.m, 4);
        ASSERT_EQ(a2_b_obs.m, 4);
    }
    ASSERT_TRUE(a1->empty());
    ASSERT_TRUE(a2->empty());
    ASSERT_TRUE(b->empty());
}

TEST(ObservableTest, set_target)
{
    auto a1 = std::make_shared<A>(0);
    auto a2 = std::make_shared<A>(0);
    A_Observer a1_obs (a1);

    a1_obs.set_target(a2);
    a1_obs.add_callback([&a1_obs](){a1_obs.update();});
    a1->increment();
    a2->increment();

    ASSERT_TRUE(a1->empty());
    ASSERT_FALSE(a2->empty());
    ASSERT_EQ(1, a1_obs.n);
}
