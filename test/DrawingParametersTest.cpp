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

TEST(DrawingParametersTest, set_starting_position)
{
    DrawingParameters parameters;

    ext::sf::Vector2d expected (10, 10);
    parameters.set_starting_position(expected);

    ASSERT_EQ(expected, parameters.get_starting_position());
    ASSERT_FALSE(parameters.poll_modification());
}

TEST(DrawingParametersTest, set_starting_angle)
{
    DrawingParameters parameters;

    double expected {3.141};
    parameters.set_starting_angle(expected);

    ASSERT_FLOAT_EQ(3.141, parameters.get_starting_angle());
    ASSERT_TRUE(parameters.poll_modification());
}

TEST(DrawingParametersTest, set_delta_angle)
{
    DrawingParameters parameters;

    double expected {3.141};
    parameters.set_delta_angle(expected);

    ASSERT_FLOAT_EQ(expected, parameters.get_delta_angle());
    ASSERT_TRUE(parameters.poll_modification());
}

TEST(DrawingParametersTest, set_step)
{
    DrawingParameters parameters;

    int expected {42};
    parameters.set_step(expected);

    ASSERT_FLOAT_EQ(expected, parameters.get_step());
    ASSERT_FALSE(parameters.poll_modification());
}

TEST(DrawingParametersTest, set_n_iter)
{
    DrawingParameters parameters;

    int expected {5};
    parameters.set_n_iter(expected);

    ASSERT_EQ(expected, parameters.get_n_iter());
    ASSERT_TRUE(parameters.poll_modification());
}

//---------------------------------------------------------------------------------------

TEST(DrawingParametersTest, revert_n_iter)
{
    constexpr int base_iter = 5;
    DrawingParameters params ({0, 0}, 0, 0, 10, /*n_iter*/ base_iter);
    constexpr int new_iter = 7;

    params.set_n_iter(new_iter);
    params.revert();

    ASSERT_EQ(base_iter, params.get_n_iter());
}


TEST(DrawingParametersTest, validate_n_iter)
{
    DrawingParameters params ({0, 0}, 0, 0, 10, /*n_iter*/ 5);
    const int new_iter = 7;

    params.set_n_iter(new_iter);
    params.validate();
    params.revert();

    ASSERT_EQ(new_iter, params.get_n_iter());
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
