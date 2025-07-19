#include <set>
#include <iostream>
#include <format>
#include <fstream>

#include "Engine/Engine.h"
#include "Engine/MapInfo.h"
#include "Engine/Tables/NPCTable.h"
#include "Engine/Tables/HouseTable.h"
#include "Engine/GameResourceManager.h"

using namespace std;

struct HousedNPC {
    unsigned int npcId;
    HouseData house;
    NPCData NPC;
};

const std::map<HouseType, std::string> houseTypeNames = {
    { HouseType::HOUSE_TYPE_INVALID, "Invalid" },
    { HouseType::HOUSE_TYPE_WEAPON_SHOP, "Weapon shop" },
    { HouseType::HOUSE_TYPE_ARMOR_SHOP, "Armor shop" },
    { HouseType::HOUSE_TYPE_MAGIC_SHOP, "Magic shop" },
    { HouseType::HOUSE_TYPE_ALCHEMY_SHOP, "Alchemy shop" },
    { HouseType::HOUSE_TYPE_FIRE_GUILD, "Fire guild" },
    { HouseType::HOUSE_TYPE_AIR_GUILD, "Air guild" },
    { HouseType::HOUSE_TYPE_WATER_GUILD, "Water guild" },
    { HouseType::HOUSE_TYPE_EARTH_GUILD, "Earth guild" },
    { HouseType::HOUSE_TYPE_SPIRIT_GUILD, "Spirit guild" },
    { HouseType::HOUSE_TYPE_MIND_GUILD, "Mind guild" },
    { HouseType::HOUSE_TYPE_BODY_GUILD, "Body guild" },
    { HouseType::HOUSE_TYPE_LIGHT_GUILD, "Light guild" },
    { HouseType::HOUSE_TYPE_DARK_GUILD, "Dark guild" },
    { HouseType::HOUSE_TYPE_ELEMENTAL_GUILD, "Elemental guild" },
    { HouseType::HOUSE_TYPE_SELF_GUILD, "Self guild" },
    { HouseType::HOUSE_TYPE_MIRRORED_PATH_GUILD, "Mirrored path guild" },
    { HouseType::HOUSE_TYPE_TOWN_HALL, "Town hall" },
    { HouseType::HOUSE_TYPE_MERCENARY_GUILD, "Mercenary guild" },
    { HouseType::HOUSE_TYPE_TOWN_HALL_MM6, "Town hall mm6" },
    { HouseType::HOUSE_TYPE_THRONE_ROOM, "Throne room" },
    { HouseType::HOUSE_TYPE_TAVERN, "Tavern" },
    { HouseType::HOUSE_TYPE_BANK, "Bank" },
    { HouseType::HOUSE_TYPE_TEMPLE, "Temple" },
    { HouseType::HOUSE_TYPE_CASTLE, "Castle" },
    { HouseType::HOUSE_TYPE_DUNGEON, "Dungeon" },
    { HouseType::HOUSE_TYPE_SEER, "Seer" },
    { HouseType::HOUSE_TYPE_STABLE, "Stable" },
    { HouseType::HOUSE_TYPE_BOAT, "Boat" },
    { HouseType::HOUSE_TYPE_HOUSE, "House" },
    { HouseType::HOUSE_TYPE_TRAINING_GROUND, "Training ground" },
    { HouseType::HOUSE_TYPE_JAIL, "Jail" },
    { HouseType::HOUSE_TYPE_CIRCUS, "Circus" },
    { HouseType::HOUSE_TYPE_GENERAL_STORE, "General store" },
    { HouseType::HOUSE_TYPE_SHADOW_GUILD, "Shadow guild" },
    { HouseType::HOUSE_TYPE_ADVENTURERS_INN, "Adventurers inn" }
};

const std::map<NpcProfession, std::string> NPCProfessionNames = {
    {NpcProfession::NoProfession, "No profession"},
    {NpcProfession::Smith, "Smith"},
    {NpcProfession::Armorer, "Armorer"},
    {NpcProfession::Alchemist, "Alchemist"},
    {NpcProfession::Scholar, "Scholar"},
    {NpcProfession::Guide, "Guide"},
    {NpcProfession::Tracker, "Tracker"},
    {NpcProfession::Pathfinder, "Pathfinder"},
    {NpcProfession::Sailor, "Sailor"},
    {NpcProfession::Navigator, "Navigator"},
    {NpcProfession::Healer, "Healer"},
    {NpcProfession::ExpertHealer, "Expert healer"},
    {NpcProfession::MasterHealer, "Master healer"},
    {NpcProfession::Teacher, "Teacher"},
    {NpcProfession::Instructor, "Instructor"},
    {NpcProfession::Armsmaster, "Armsmaster"},
    {NpcProfession::Weaponsmaster, "Weaponsmaster"},
    {NpcProfession::Apprentice, "Apprentice"},
    {NpcProfession::Mystic, "Mystic"},
    {NpcProfession::Spellmaster, "Spellmaster"},
    {NpcProfession::Trader, "Trader"},
    {NpcProfession::Merchant, "Merchant"},
    {NpcProfession::Scout, "Scout"},
    {NpcProfession::Herbalist, "Herbalist"},
    {NpcProfession::Apothecary, "Apothecary"},
    {NpcProfession::Tinker, "Tinker"},
    {NpcProfession::Locksmith, "Locksmith"},
    {NpcProfession::Fool, "Fool"},
    {NpcProfession::ChimneySweep, "Chimney sweep"},
    {NpcProfession::Porter, "Porter"},
    {NpcProfession::QuarterMaster, "Quarter master"},
    {NpcProfession::Factor, "Factor"},
    {NpcProfession::Banker, "Banker"},
    {NpcProfession::Cook, "Cook"},
    {NpcProfession::Chef, "Chef"},
    {NpcProfession::Horseman, "Horseman"},
    {NpcProfession::Bard, "Bard"},
    {NpcProfession::Enchanter, "Enchanter"},
    {NpcProfession::Cartographer, "Cartographer"},
    {NpcProfession::WindMaster, "Wind master"},
    {NpcProfession::WaterMaster, "Water master"},
    {NpcProfession::GateMaster, "Gate master"},
    {NpcProfession::Acolyte, "Acolyte"},
    {NpcProfession::Piper, "Piper"},
    {NpcProfession::Explorer, "Explorer"},
    {NpcProfession::Pirate, "Pirate"},
    {NpcProfession::Squire, "Squire"},
    {NpcProfession::Psychic, "Psychic"},
    {NpcProfession::Gypsy, "Gypsy"},
    {NpcProfession::Diplomat, "Diplomat"},
    {NpcProfession::Duper, "Duper"},
    {NpcProfession::Burglar, "Burglar"},
    {NpcProfession::FallenWizard, "Fallen wizard"},
    {NpcProfession::Acolyte2, "Acolyte2"},
    {NpcProfession::Initiate, "Initiate"},
    {NpcProfession::Prelate, "Prelate"},
    {NpcProfession::Monk, "Monk"},
    {NpcProfession::Sage, "Sage"},
    {NpcProfession::Hunter, "Hunter"}
};

string none_if_empty(string mes, bool is_empty) {
    if (is_empty) {
        return mes + ": none";
    } else {
        return mes;
    }
}

void compileLLMContext() {
    ofstream log_file("LLMData/log.txt");

    for (MapId i : pMapStats->pInfos.indices()) {
        // Code to load the local event map, copied from Eninge.cpp / loadMapEventsAndStrings
        string mapName = pMapStats->pInfos[i].fileName;
        string mapNameWithoutExt = mapName.substr(0, mapName.rfind('.'));
        const auto evtProg = EvtProgram::load(engine->_gameResourceManager->getEventsFile(fmt::format("{}.evt", mapNameWithoutExt)));

        // Find dungeons on map
        // See case EVENT_MoveToMap in executeOneEvent: the map name is in the "str" parameter
        auto dungeonEventMap = evtProg.getEventMap(EvtOpcode::EVENT_MoveToMap);
        set<MapId> dungeonIds;
        for (auto [evtId, ir] : dungeonEventMap) {
            if (ir.data.move_map_descr.house_id != HOUSE_INVALID || ir.data.move_map_descr.exit_pic_id) {
                MapId mid = pMapStats->GetMapInfo(ir.str);
                if (mid != MAP_INVALID) dungeonIds.insert(mid);
            }
        }

        // Find the NPCs that live in the houses on the map
        // For a house event, the houseTable[houseId].uType variable tells us which type of house it is
        auto houseEventMap = evtProg.getEventMap(EvtOpcode::EVENT_SpeakInHouse);
        set<HouseId> houseIds;
        set<HouseType> houseTypes;
        for (auto [id, ir] : houseEventMap) {
            houseIds.insert(ir.data.house_id);
            houseTypes.insert(houseTable[ir.data.house_id].uType);
        }

        vector<HousedNPC> housedNPCs;
        for (unsigned int i = 0; i < pNPCStats->uNumNewNPCs; ++i) {
            NPCData *npc = &pNPCStats->pNPCData[i];
            if (npc->Location2D != HOUSE_INVALID && houseIds.contains(npc->Location2D)) {
                housedNPCs.push_back({i, houseTable[npc->Location2D], *npc});
            }
        }

        // Write output about map
        auto mapInfo = &pMapStats->pInfos[i];

        // Debug output
        log_file << "*****************************************************************" << endl;
        log_file << format("MAP {} ({})", mapInfo->name, (int)i) << endl;
        log_file << "Type: " << (isMapIndoor(i) ? "indoor" : "outdoor") << endl;
        log_file << "Env: " << location_type[mapInfo->uEAXEnv] << endl;
        log_file << "Difficulty: " << (int)mapInfo->mapTreasureLevel << endl;
        
        // TODO Apparently, the NPCs do not have professions set at this point, they are all "No profession".
        log_file << none_if_empty("NPCs who live on this map", housedNPCs.empty()) << endl;
        for (auto x : housedNPCs) {
            log_file << format("NPC {} ({}), profession {}, lives in house {} ({})", x.NPC.name, x.npcId, NPCProfessionNames.at(x.NPC.profession), x.house.name, (int)x.NPC.Location2D) << endl;
        }

        log_file << none_if_empty("Houses and shops on this map", houseTypes.empty()) << endl;
        for (auto tid : houseTypes) {
            log_file << houseTypeNames.at(tid) << endl;
        }
        
        const string pathName = isMapIndoor(i) ? "Exits or sub dungeons" : "Dungeons on this map";
        log_file << none_if_empty(pathName, dungeonIds.empty()) << endl;
        for (auto x : dungeonIds) {
            auto dungeonInfo = &pMapStats->pInfos[x];
            log_file << format("{} ({})", dungeonInfo->name, (int)x) << endl;
        }
    }
}