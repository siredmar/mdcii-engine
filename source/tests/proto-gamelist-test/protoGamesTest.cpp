#include <gmock/gmock.h>

#include <mdcii/cache/cacheprotobuf.hpp>

#include "gamelist.pb.h"

class ProtoGamelistTest : public testing::Test
{
public:
};

TEST_F(ProtoGamelistTest, SimpleSingleGamesList)
{
    auto list = GamesPb::Games();
    for (int i = 0; i < 5; i++)
    {
        auto singleScenario = list.add_single();
        singleScenario->set_name("scenario " + std::to_string(i));
        singleScenario->set_path("/tmp/scen" + std::to_string(i) + ".szs");
        singleScenario->set_stars(i);
        singleScenario->set_flags(i % 2);
    }
    ASSERT_EQ(list.single_size(), 5);
}