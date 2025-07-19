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
        auto houseEventMap = evtProg.getEventMap(EvtOpcode::EVENT_SpeakInHouse);
        set<HouseId> houseIds;
        for (auto x : houseEventMap) houseIds.insert(x.second.data.house_id);

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
        log_file << "MAP " << mapInfo->name << format(" ({}) type {}", (int)i, isMapIndoor(i) ? "indoor" : "outdoor") << endl;
        log_file << "*****************************************************************" << endl;
        
        log_file << "Housed NPCs";
        if (housedNPCs.empty()) {
            log_file << ": none" << endl;
        } else {
            log_file << endl;
        }
        for (auto x : housedNPCs) {
            log_file << format("NPC {} ({}) lives in house {} ({})", x.NPC.name, x.npcId, x.house.name, (int)x.NPC.Location2D) << endl;
        }
        
        if (isMapIndoor(i)) {
            // Most dungeons just have on exit, but some have sub dungeons
            log_file << "Exits";
        } else {
            log_file << "Dungeons";
        }
        if (dungeonIds.empty()) {
            log_file << ": none" << endl;
        } else {
            log_file << endl;
        }
        for (auto x : dungeonIds) {
            auto dungeonInfo = &pMapStats->pInfos[x];
            log_file << format("{} ({}) type {}", dungeonInfo->name, (int)x, isMapIndoor(x) ? "indoor" : "outdoor") << endl;
        }
    }
}