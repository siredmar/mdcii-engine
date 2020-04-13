#ifndef _SAVEGAMES_H_
#define _SAVEGAMES_H_

#include <experimental/optional>
#include <string>
#include <vector>

#include "files.hpp"

class Savegames
{
public:
  Savegames();
  int size() const;
  std::experimental::optional<std::string> getPath(int index) const;
  std::vector<std::string> getSavegames() const;

private:
  std::vector<std::string> savegames;
};

#endif // _SAVEGAMES_H_