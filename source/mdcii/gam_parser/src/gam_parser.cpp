
// This file is part of the MDCII Game Engine.
// Copyright (C) 2015  Benedikt Freisen
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <boost/program_options.hpp>
#include <iostream>

#include "mdcii/cod/cod_parser.hpp"
#include "mdcii/cod/haeuser.hpp"
#include "mdcii/files/files.hpp"
#include "mdcii/gam/gam_parser.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  std::string input_name;

  // clang-format off
  po::options_description desc("Options");
  desc.add_options()
    ("input,i", po::value<std::string>(&input_name), "Input file (*.bsh, *.szs, *.szm)")
    ("path,p", po::value<std::string>()->default_value("."), "Installation path to 1602 AD")
    ("help,h", "Print this help")
  ;
  // clang-format on

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (vm.count("input") != 1)
  {
    std::cout << "No valid input file given." << std::endl;
    exit(EXIT_FAILURE);
  }
  if (vm.count("path") != 1)
  {
    std::cout << "No valid installation path for 1602 AD given." << std::endl;
    exit(EXIT_FAILURE);
  }

  auto files = Files::CreateInstance(vm["path"].as<std::string>());
  Buildings::CreateInstance(std::make_shared<CodParser>(files->Instance()->FindPathForFile("haeuser.cod"), true, false));
  try
  {
    GamParser gamParser(vm["input"].as<std::string>(), false);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    exit(1);
  }
  catch (const std::string& ex)
  {
    std::cout << ex << std::endl;
  }
  catch (...)
  {
    std::cout << "unknown exception" << std::endl;
  }
}
