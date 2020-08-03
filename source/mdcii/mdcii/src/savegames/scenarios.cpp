// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
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

#include <filesystem>

#include "cod/codhelpers.hpp"
#include "cod/text_cod.hpp"
#include "files/files.hpp"
#include "gam/gam_parser.hpp"
#include "savegames/scenarios.hpp"

#include "gamelist.pb.h"

namespace fs = std::filesystem;

Scenarios::Scenarios(const std::string& basepath, const std::string& fileEnding)
{
    auto files = Files::Instance();
    auto tree = files->GetDirectoryFiles(files->FindPathForFile(basepath));
    for (auto& s : tree)
    {
        if ("." + fileEnding == fs::path(s).extension())
        {
            if (IsSubstring(s, "ngsspiel4"))
            {
                std::cout << "h";
            }
            GamParser gam(s, true);
            std::string filename = fs::path(s).filename();
            auto multiMatch = RegexSearch("([0-9]+)?[.](" + fileEnding + ")", filename);
            // Check if previous regex detects that the selected file is a mission from a campaign by checking the number.
            // e.g. Mission4.szs. The '4' says that the file is within a campaign and its number 4 (counted from 0).
            // Files with no number mean single mission.
            if (!IsEmpty(multiMatch.at(1)))
            {
                if (gam.GetSceneGameID() == SceneGameIDType::Endless)
                {
                    auto game = gamelist.add_endless();
                    game->set_path(s.c_str());
                    game->set_stars(gam.GetSceneRanking());
                    game->set_name(RemoveDigits(fs::path(s).stem()));
                }
                else
                {
                    // Abort this mission for now if if does not belong to a valid campaign
                    auto campaignNumber = gam.GetSceneCampaign();
                    if (campaignNumber == -1)
                    {
                        continue;
                    }

                    int index = -1;
                    for (int i = 0; i < gamelist.campaign_size(); i++)
                    {
                        if (gamelist.campaign(i).name() == fs::path(s).stem())
                        {
                            index = i;
                            break;
                        }
                    }
                    // The campaign has not been found yet, lets create one
                    GamesPb::Campaign* campaign;
                    if (index == -1)
                    {
                        auto newCampaign = gamelist.add_campaign();
                        newCampaign->set_name(RemoveDigits(fs::path(s).stem()));
                        campaign = newCampaign;
                    }
                    else
                    {
                        campaign = gamelist.mutable_campaign(index);
                    }
                    auto game = campaign->add_game();
                    // TODO: 5 is currently hardcoded
                    // There are five possible missions referred from text.cod.
                    // The campaign number is read from the szenario file.
                    // The mission number within the campaign is the number in the filename.
                    // The calculated index for the mission name is: campaign number * 5 + file number
                    auto TextCodIndex
                        = campaignNumber * 5 + std::stoi(multiMatch.at(1));
                    game->set_name(TextCod::Instance()->GetValue("KAMPAGNE", TextCodIndex));
                    game->set_stars(gam.GetSceneRanking());
                    game->set_path(s.c_str());
                }
            }
            else
            {
                auto singleMatch = RegexSearch("(.*)[.](" + fileEnding + ")", filename);
                auto game = gamelist.add_single();
                game->set_path(s.c_str());
                game->set_stars(gam.GetSceneRanking());
                game->set_name(RemoveDigits(fs::path(s).stem()));
            }
        }
    }
}

// GamesPb::MultiGame Savegames::CreateMultigame(const std::string& basename, const std::vector<std::string>& list)
// {
//     GamesPb::MultiGame ret;
//     ret.set_name(basename.c_str());
//     for (auto& s : list)
//     {
//         if (s.find(basename) != std::string::npos)
//         {
//             auto g = ret.add_games();
//             auto gam = GamParser(s, true);
//             g->set_name();
//         }
//     }
//     return ret;
// }

// unsigned int Scenarios::size() const
// {
//     return savegames.size();
// }

// std::experimental::optional<std::string> Savegames::GetPath(unsigned int index) const
// {
//     if (index < savegames.size())
//     {
//         return std::get<0>(savegames[index]);
//     }
//     return {};
// }

// std::experimental::optional<std::string> Savegames::GetName(unsigned int index) const
// {
//     if (index < savegames.size())
//     {
//         return std::get<1>(savegames[index]);
//     }
//     return {};
// }

// std::experimental::optional<int> Savegames::GetRanking(unsigned int index) const
// {
//     if (index < savegames.size())
//     {
//         return std::get<2>(savegames[index]);
//     }
//     return {};
// }

GamesPb::Games Scenarios::Get() const
{
    return gamelist;
}