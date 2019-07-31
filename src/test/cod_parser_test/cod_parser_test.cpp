#include <iostream>

#include <boost/program_options.hpp>

#include "../../cod_parser.hpp"
#include "../../haeuser.hpp"
#include "../../files.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  std::string cod_path;

  po::options_description desc("Zulässige Optionen");
  desc.add_options()("cod,c", po::value<std::string>(&cod_path)->default_value("test.cod"), "Path to .cod file");
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
    Cod_Parser cod(cod_path, true);
    Haeuser haeuser(&cod);
    auto h = haeuser.get_haus(1254);
    if(h)
    {
      std::cout << h.value().Gfx << std::endl;
    }
  }
}
