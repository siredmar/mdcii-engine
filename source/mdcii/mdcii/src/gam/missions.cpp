// #include <cstring>

// #include "gam/missions.hpp"


// Missions::Missions(uint8_t* data, uint32_t length, const std::string& name)
//   : name(name)
// {
//   for (int i = 0; i < 4; i++)
//   {
//     MissionData m;
//     int missionLength = length / 4;
//     memset((char*)&m, 0, missionLength);
//     memcpy((char*)&m, data + (i * missionLength), missionLength);
//     missions.push_back(m);
//   }
// }

#include <cstring>
#include <ios>
#include <iostream>
#include <memory>
#include <variant>

#include "gam/missions.hpp"


Mission4::Mission4(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
{
  Mission4Data m;
  // Multiple missions in one chunk laying adjacent in memory.
  int numMissions = length / sizeof(Mission4Data);
  for (int i = 0; i < numMissions; i++)
  {
    int missionLength = length / numMissions;
    memset((char*)&m, 0, missionLength);
    memcpy((char*)&m, data + (i * missionLength), missionLength);
    missions.push_back(m);
  }
}

Mission2::Mission2(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
{
  Mission2Data m;
  // Multiple missions in one chunk laying adjacent in memory.
  int numMissions = length / sizeof(Mission2Data);
  for (int i = 0; i < numMissions; i++)
  {
    int missionLength = length / numMissions;
    memset((char*)&m, 0, missionLength);
    memcpy((char*)&m, data + (i * missionLength), missionLength);
    missions.push_back(m);
  }
}