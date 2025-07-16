#include <set>
#include <iostream>
#include <format>

#include "Engine/Engine.h"
#include "Engine/MapInfo.h"
#include "Engine/Tables/NPCTable.h"
#include "Engine/Tables/HouseTable.h"

using namespace std;

struct HousedNPC {
    unsigned int npcId;
    HouseData house;
    NPCData NPC;
};

void compileLLMContext() {
    auto local_map = engine->_localEventMap.getEventMap(EvtOpcode::EVENT_SpeakInHouse);
    set<HouseId> house_ids;

    for (auto x : local_map) house_ids.insert(x.second.data.house_id);

    // There should not be houses that are not on local maps?
    // auto global_map = engine->_globalEventMap.getEventMap(EvtOpcode::EVENT_SpeakInHouse);

    // Find the NPCs that live in the houses on the map
    vector<HousedNPC> housedNPCs;
    for (unsigned int i = 0; i < pNPCStats->uNumNewNPCs; ++i) {
        NPCData *npc = &pNPCStats->pNPCData[i];
        if (npc->Location2D != HOUSE_INVALID && house_ids.contains(npc->Location2D)) {
            housedNPCs.push_back({i, houseTable[npc->Location2D], *npc});
        }
    }

    auto mapInfo = &pMapStats->pInfos[engine->_currentLoadedMapId];

    // Debug output
    cout << "*****************************************************************" << endl;
    cout << "Housed NPCs on map " << mapInfo->name << endl;
    cout << "*****************************************************************" << endl;
    for (auto x : housedNPCs) {
        cout << format("NPC {} ({}) lives in house {} ({}) owned by {}", x.NPC.name, x.npcId, x.house.name, (int)x.NPC.Location2D, x.house.pProprieterName) << endl;
    }
}