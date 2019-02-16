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

    ASSERT_NEAR(oparams.get_starting_angle(), iparams.get_starting_angle(), 0.0001);
    ASSERT_NEAR(oparams.get_delta_angle(), iparams.get_delta_angle(), 0.0001);
    ASSERT_NEAR(oparams.get_step(), iparams.get_step(), 0.0001);
    ASSERT_EQ(oparams.get_n_iter(), iparams.get_n_iter());
}
