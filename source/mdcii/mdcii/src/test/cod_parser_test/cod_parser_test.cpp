#include <iostream>

#include <boost/program_options.hpp>

#include "../../cod_parser.hpp"
#include "../../files.hpp"
#include "../../haeuser.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  std::string cod_path;
  bool decrypt;
  po::options_description desc("Zulässige Optionen");
  desc.add_options()("cod,c", po::value<std::string>(&cod_path)->default_value("test.cod"), "Path to .cod file");
  desc.add_options()("decrypt,d", po::value<bool>(&decrypt)->default_value(true), "decrypt true/false");
  desc.add_options()("help,h", "Gibt diesen Hilfetext aus");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    exit(EXIT_SUCCESS);
  }
  // auto files = Files::create_instance(".", false);
  // if (files->instance()->check_file(cod_path) == true)
  {
    std::shared_ptr<Cod_Parser> cod = std::make_shared<Cod_Parser>(cod_path, decrypt, true);
    Haeuser haeuser(cod);
    for (int i = 0; i < 2500; i++)
    {
      auto h = haeuser.get_haus(i);
      if (h)
      {
        // std::cout << h.value()->Id-20000 << "," << h.value()->Gfx << std::endl;
        if (h.value()->Size.w * h.value()->Size.h != h.value()->Rotate)
        {
          std::cout << h.value()->Id - 20000 << ": " << h.value()->Size.w << ", " << h.value()->Size.h << "!=" << h.value()->Rotate << std::endl;
        }
      }
    }
  }
}
