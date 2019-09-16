#include <sstream>
#include <gtest/gtest.h>
#include "cereal/archives/json.hpp"
#include "DrawingParameters.h"
#include "Observer.h"

using namespace drawing;

TEST(DrawingParametersTest, complete_ctor)
{
    ext::sf::Vector2d expected_pos {100, 100};
    double expected_angle {1};
    double expected_delta {1};
    double expected_step {10};
    int expected_iter {3};
    DrawingParameters params (expected_pos, expected_angle, expected_delta, expected_step, expected_iter);

    ASSERT_EQ(expected_pos, params.get_starting_position());
    ASSERT_FLOAT_EQ(expected_angle, params.get_starting_angle());
    ASSERT_FLOAT_EQ(expected_delta, params.get_delta_angle());
    ASSERT_FLOAT_EQ(expected_step, params.get_step());
    ASSERT_EQ(expected_iter, params.get_n_iter());    
}

// Other constructors are defaulted, we assume the implementation is correct.

//---------------------------------------------------------------------------------------

class Obs : public Observer<DrawingParameters>
{
public:
    using O = Observer<DrawingParameters>;

    explicit Obs(std::shared_ptr<DrawingParameters> params)
        : O{params}
        {add_callback( [this](){ ++notify_count_; });}

    operator bool() const
        {
            return notify_count_ == 1;
        }

    int get_notify_count()
        {
            return notify_count_;
        }

private:
    int notify_count_ {0};
};


TEST(DrawingParametersTest, set_starting_position)
{
    DrawingParameters d;
    auto params_ptr = std::make_shared<DrawingParameters>(d);
    Obs params_obs (params_ptr);

    ext::sf::Vector2d expected (10, 10);
    params_ptr->set_starting_position(expected);

    ASSERT_EQ(0, params_obs.get_notify_count());
    ASSERT_EQ(expected, params_ptr->get_starting_position());
}

TEST(DrawingParametersTest, set_starting_angle)
{
    auto params_ptr = std::make_shared<DrawingParameters>(DrawingParameters());
    Obs params_obs (params_ptr);

    double expected {3.141};
    params_ptr->set_starting_angle(expected);

    ASSERT_TRUE(params_obs);
    ASSERT_FLOAT_EQ(3.141, params_ptr->get_starting_angle());
}

TEST(DrawingParametersTest, set_delta_angle)
{
    auto params_ptr = std::make_shared<DrawingParameters>(DrawingParameters());
    Obs params_obs (params_ptr);

    double expected {3.141};
    params_ptr->set_delta_angle(expected);

    ASSERT_TRUE(params_obs);
    ASSERT_FLOAT_EQ(expected, params_ptr->get_delta_angle());
}

TEST(DrawingParametersTest, set_step)
{
    auto params_ptr = std::make_shared<DrawingParameters>(DrawingParameters());
    Obs params_obs (params_ptr);

    int expected {42};
    params_ptr->set_step(expected);

    ASSERT_TRUE(params_obs);
    ASSERT_FLOAT_EQ(expected, params_ptr->get_step());
}

TEST(DrawingParametersTest, set_n_iter)
{
    auto params_ptr = std::make_shared<DrawingParameters>(DrawingParameters());
    Obs params_obs (params_ptr);

    int expected {5};
    params_ptr->set_n_iter(expected);

    ASSERT_TRUE(params_obs);
    ASSERT_EQ(expected, params_ptr->get_n_iter());
}

//---------------------------------------------------------------------------------------

TEST(DrawingParametersTest, serialization)
{
    DrawingParameters oparams { {100,100}, 1, 1, 10, 3};
    DrawingParameters iparams;
    
    std::stringstream ss;
    {
        cereal::JSONOutputArchive oarchive (ss);
        oarchive(oparams);
    }
    {
        cereal::JSONInputArchive iarchive (ss);
        iarchive(iparams);
    }

    // The lines below are not necessary: starting_position and step are not saved.
    // ASSERT_EQ(oparams.get_starting_position(), iparams.get_starting_position());
    // ASSERT_FLOAT_EQ(oparams.get_delta_angle(), iparams.get_delta_angle());

    ASSERT_FLOAT_EQ(oparams.get_starting_angle(), iparams.get_starting_angle());
    ASSERT_FLOAT_EQ(oparams.get_delta_angle(), iparams.get_delta_angle());
    ASSERT_EQ(oparams.get_n_iter(), iparams.get_n_iter());
}
