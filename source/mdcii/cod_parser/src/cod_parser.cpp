#include <iostream>

#include <boost/program_options.hpp>

#include "mdcii/cod/cod_parser.hpp"
#include "mdcii/cod/haeuser.hpp"
#include "mdcii/files/files.hpp"

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
  CodParser cod(cod_path, decrypt, true);
}
