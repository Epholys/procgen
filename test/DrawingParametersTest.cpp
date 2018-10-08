#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include "cereal/archives/json.hpp"
#include "DrawingParameters.h"

TEST(DrawingParametersTest, serialization)
{
    drawing::DrawingParameters oparams { {100,100}, 1, 1, 10, 3};
    drawing::DrawingParameters iparams;
    
    std::stringstream ss;
    {
        cereal::JSONOutputArchive oarchive (ss);
        oarchive(oparams);
    }
    {
        cereal::JSONInputArchive iarchive (ss);
        iarchive(iparams);
    }

    ASSERT_NEAR(oparams.starting_angle, iparams.starting_angle, 0.0001);
    ASSERT_NEAR(oparams.delta_angle, iparams.delta_angle, 0.0001);
    ASSERT_NEAR(oparams.step, iparams.step, 0.0001);
    ASSERT_EQ(oparams.n_iter, iparams.n_iter);
}
