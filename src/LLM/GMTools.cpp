#include "GMTools.h"
#include "GMState.h"
#include "Engine/EngineGlobals.h"
#include "Engine/Tables/ItemTable.h"

ItemId GM_CreateQuestItem(const ItemData& itemData) {
    if (pGMState->customItems.size() >= (std::to_underlying(ITEM_LAST_DYNAMIC) - std::to_underlying(ITEM_FIRST_DYNAMIC) + 1)) {
        return ITEM_NULL;
    }

    pGMState->customItems.push_back(itemData);
    pItemTable->LoadCustomItems();

    return static_cast<ItemId>(std::to_underlying(ITEM_FIRST_DYNAMIC) + pGMState->customItems.size() - 1);
}