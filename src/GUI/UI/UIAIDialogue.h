#pragma once

#include "GUI/GUIWindow.h"

class GUIWindow_AIDialogue : public GUIWindow {
public:
    explicit GUIWindow_AIDialogue(int npcId);
    virtual ~GUIWindow_AIDialogue() {}

    virtual void Update() override;
    virtual void Release() override;

private:
    int _npcId;
};