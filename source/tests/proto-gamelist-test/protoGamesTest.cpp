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
        auto entry = list.add_gameentry();
        auto singleScenario = entry->mutable_singlegame();
        singleScenario->set_name("scenario " + std::to_string(i));
        singleScenario->set_path("/tmp/scen" + std::to_string(i) + ".szs");
        singleScenario->set_stars(i);
        singleScenario->set_multiplayer(i % 2);
    }
    ASSERT_EQ(list.gameentry_size(), 5);
}