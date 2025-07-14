#pragma once

#include <string>
#include <vector>
#include "GUI/GUIWindow.h"

void initializeAIControlledDialogue(int npcId, int bPlayerSaysHello, Actor *actor = nullptr);

class GUIWindow_AIDialogue : public GUIWindow {
public:
    explicit GUIWindow_AIDialogue();
    virtual ~GUIWindow_AIDialogue() {}

    virtual void Update() override;
    virtual void Release() override;
private:
    std::vector<std::string> conversationLog;
};