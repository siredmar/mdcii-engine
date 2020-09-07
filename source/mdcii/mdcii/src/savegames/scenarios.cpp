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

#include <algorithm>
#include <filesystem>

#include "cod/codhelpers.hpp"
#include "cod/text_cod.hpp"
#include "common/stringhelpers.hpp"
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
        // TODO: filter Einfuehrungsspiel hardcoded
        if (!IsSubstring(fs::path(s).filename(), "rungsspiel"))
        {
            if ("." + fileEnding == fs::path(s).extension())
            {
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
                        game->set_missiontext(gam.GetMissionText(0));
                    }
                    else
                    {
                        auto missionNumber = std::stoi(multiMatch.at(1));
                        // Abort this mission for now if if does not belong to a valid campaign
                        auto campaignNumber = gam.GetSceneCampaign();
                        if (missionNumber > 0)
                        {
                            int protoCampaignIndex = CampagneIndex(RemoveDigits(fs::path(s).stem()));
                            campaignNumber = gamelist.campaign(protoCampaignIndex).number();
                        }
                        if (campaignNumber == -1)
                        {
                            std::cout << "[INFO] Skipping, because no valid scene campaign: " << filename << std::endl;
                            continue;
                        }

                        int index = CampagneIndex(RemoveDigits(fs::path(s).stem()));
                        // The campaign has not been found yet, lets create one
                        GamesPb::Campaign* campaign;
                        if (index == -1)
                        {
                            auto newCampaign = gamelist.add_campaign();
                            newCampaign->set_name(RemoveDigits(fs::path(s).stem()));
                            newCampaign->set_number(campaignNumber);
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
                        auto TextCodIndex = campaignNumber * 5 + missionNumber;
                        game->set_name(TextCod::Instance()->GetValue("KAMPAGNE", TextCodIndex));
                        game->set_stars(gam.GetSceneRanking());
                        game->set_path(s.c_str());
                        game->set_missionnumber(missionNumber);
                        std::string missionTextRaw = gam.GetMissionText(0);
                        game->set_missiontext(removeTrailingCarriageReturnNewline(missionTextRaw));
                        campaign->set_stars(gam.GetSceneRanking());
                    }
                }
                else
                {
                    GamesPb::SingleGame* game;
                    auto missionNumber = gam.GetMissionNumber();
                    if (missionNumber != -1)
                    {
                        game = gamelist.add_originalsingle();
                    }
                    else
                    {
                        game = gamelist.add_addonsingle();
                    }
                    game->set_missionnumber(missionNumber);
                    game->set_path(s.c_str());
                    game->set_stars(gam.GetSceneRanking());
                    game->set_name(RemoveDigits(fs::path(s).stem()));
                    game->set_missiontext(gam.GetMissionText(0));
                }
            }
        }
    }
    for (int i = 0; i < gamelist.campaign_size(); i++)
    {
        SortCampaignMissions(gamelist.mutable_campaign(i));
    }
    SortOriginalMissions();
    SortCampaigns();
}

int Scenarios::CampagneIndex(const std::string& name)
{
    int index = -1;
    for (int i = 0; i < gamelist.campaign_size(); i++)
    {
        if (gamelist.campaign(i).name() == name)
        {
            return i;
        }
    }
    return -1;
}

void Scenarios::SortCampaignMissions(GamesPb::Campaign* campaign)
{
    std::sort(campaign->mutable_game()->begin(),
        campaign->mutable_game()->end(),
        [](const GamesPb::SingleGame& a, const GamesPb::SingleGame& b) {
            return a.missionnumber() < b.missionnumber();
        });
}

void Scenarios::SortOriginalMissions()
{
    std::sort(gamelist.mutable_originalsingle()->begin(),
        gamelist.mutable_originalsingle()->end(),
        [](const GamesPb::SingleGame& a, const GamesPb::SingleGame& b) {
            return a.missionnumber() < b.missionnumber();
        });
}

void Scenarios::SortCampaigns()
{
    std::sort(gamelist.mutable_campaign()->begin(),
        gamelist.mutable_campaign()->end(),
        [](const GamesPb::Campaign& a, const GamesPb::Campaign& b) {
            return a.number() < b.number();
        });
}

GamesPb::Games Scenarios::Get() const
{
    return gamelist;
}