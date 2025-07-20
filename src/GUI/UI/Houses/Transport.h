#pragma once

#include <vector>

#include "GUI/UI/UIHouses.h"
#include "GUI/UI/UIHouseEnums.h"

class GUIWindow_Transport : public GUIWindow_House {
 public:
    explicit GUIWindow_Transport(HouseId houseId) : GUIWindow_House(houseId) {}
    virtual ~GUIWindow_Transport() {}

    virtual void houseDialogueOptionSelected(DialogueId option) override;
    virtual void houseSpecificDialogue() override;
    virtual std::vector<DialogueId> listDialogueOptions() override;

 protected:
    void mainDialogue();
    void transportDialogue();

 private:
    /**
     * @brief                               New function.
     *
     * @param schedule_id                   Index to transport_schedule.
     *
     * @return                              Number of days travel by transport will take with hireling modifiers.
     */
    int getTravelTimeTransportDays(int schedule_id);
};

bool isTravelAvailable(HouseId houseId);

struct TransportInfo {
    MapId uMapInfoID;
    std::array<unsigned char, 7> pSchedule;
    int uTravelTime; // In days.
    Vec3f arrivalPos;
    int arrival_view_yaw;
    QuestBit uQuestBit;  // quest bit required to set for this travel option to be enabled; otherwise 0
};

extern const std::array<TransportInfo, 35> transportSchedule;
extern const IndexedArray<std::array<int, 4>, HOUSE_FIRST_TRANSPORT, HOUSE_LAST_TRANSPORT> transportRoutes;