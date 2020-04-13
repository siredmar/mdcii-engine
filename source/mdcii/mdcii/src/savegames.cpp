#include "savegames.hpp"
#include "files.hpp"

Savegames::Savegames()
{
  auto files = Files::instance();
  auto savegamefolder = files->find_path_for_file("savegame");
  auto tree = files->get_directories_files(savegamefolder);
  for (auto& s : tree)
  {
    if (s.find(".gam") != std::string::npos)
    {
      savegames.push_back(s);
    }
  }
}

int Savegames::size() const
{
  return (int)savegames.size();
}

std::experimental::optional<std::string> Savegames::getPath(int index) const
{
  if (index < savegames.size())
  {
    return savegames[index];
  }
  return {};
}

std::vector<std::string> Savegames::getSavegames() const
{
  return savegames;
}