#include <gmock/gmock.h>

#include <filesystem>

#include <mdcii/cache/cacheprotobuf.hpp>

#include "cod.pb.h"

namespace fs = std::filesystem;

class CacheProtobufTest : public testing::Test
{
public:
};

TEST_F(CacheProtobufTest, CheckSomeResults)
{
  std::string path = fs::u8path("mdcii-tests/cacheprotobuftest.json");
  std::string fullPath = std::string(path.c_str());

  cod_pb::Object obj;
  obj.set_name("Test");
  auto variables = obj.mutable_variables();
  auto var = variables->add_variable();
  var->set_name("Variable1");
  var->set_value_string("value1");
  CacheProtobuf<cod_pb::Object> cache(fullPath, true);
  ASSERT_EQ(cache.Exists(), false);
  cache.Serialize(obj);
  ASSERT_EQ(cache.Exists(), true);
  auto deserialized = cache.Deserialize();
  ASSERT_EQ(deserialized.name(), "Test");
  ASSERT_EQ(deserialized.variables().variable(0).name(), "Variable1");
  ASSERT_EQ(deserialized.variables().variable(0).value_string(), "value1");
}