#include <iostream>
#include "version.hpp"
#include "bsh_leser.hpp"
#include "files.hpp"

Anno_version Version::Detect_game_version()
{
  auto files = Files::instance();
  Bsh_leser bsh(files->instance()->find_path_for_file("sgfx/stadtfld.bsh"));
  if (bsh.anzahl() == 5748)
  {
    std::cout << "[INFO] Vanilla Anno 1602 Installation erkannt" << std::endl;
    return Anno_version::VANILLA;
  }
  else // sgfx.anzahl == 5964
  {
    std::cout << "[INFO] NINA Anno 1602 Installation erkannt" << std::endl;
    return Anno_version::NINA;
  }
}
