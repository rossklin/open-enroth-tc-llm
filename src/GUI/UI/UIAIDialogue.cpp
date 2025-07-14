#include "UIAIDialogue.h"

#include <map>
#include <string>

#include "Engine/AssetsManager.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Localization.h"
#include "Engine/mm7_data.h"
#include "Engine/Objects/NPC.h"
#include "Engine/Party.h"
#include "GUI/GUIFont.h"
#include "GUI/GUIWindow.h"
#include "GUI/UI/UIHouses.h"
#include "Io/KeyboardInputHandler.h"
#include "GUI/GUIMessageQueue.h"
#include "Engine/Graphics/Viewport.h"
#include "Utility/String/Ascii.h"
#include "Engine/Engine.h"
#include "GUI/UI/UIGame.h"
#include "GUI/UI/UIDialogue.h"
#include <iostream>

Actor *currentSpeakingAI = nullptr;

void initializeAIControlledDialogue(int npcId, int bPlayerSaysHello, Actor *actor) {
    currentAddressingAwardBit = -1;
    pNPCStats->dword_AE336C_LastMispronouncedNameFirstLetter = -1;
    pEventTimer->setPaused(true);
    pMiscTimer->setPaused(true);
    speakingNpcId = npcId;
    currentSpeakingAI = actor;
    NPCData *pNPCInfo = getNPCData(npcId);
    if (!(pNPCInfo->uFlags & NPC_GREETED_SECOND)) {
        if (pNPCInfo->uFlags & NPC_GREETED_FIRST) {
            pNPCInfo->uFlags &= ~NPC_GREETED_FIRST;
            pNPCInfo->uFlags |= NPC_GREETED_SECOND;
        } else {
            pNPCInfo->uFlags |= NPC_GREETED_FIRST;
        }
    }

    game_ui_dialogue_background = assets->getImage_Solid(dialogueBackgroundResourceByAlignment[pParty->alignment]);

    currentHouseNpc = 0;

    HouseNpcDesc desc;
    desc.type = HOUSE_NPC;
    desc.label = localization->FormatString(LSTR_CONVERSE_WITH_S, pNPCInfo->name);
    desc.icon = assets->getImage_ColorKey(fmt::format("npc{:03}", pNPCInfo->uPortraitID));
    desc.npc = pNPCInfo;

    houseNpcs.push_back(desc);

    pDialogueWindow = new GUIWindow_AIDialogue();
}

GUIWindow_AIDialogue::GUIWindow_AIDialogue() : GUIWindow(WINDOW_Dialogue, {0, 0}, render->GetRenderDimensions()) {
    prev_screen_type = current_screen_type;
    current_screen_type = SCREEN_NPC_DIALOGUE;
    keyboardInputHandler->StartTextInput(Io::TextInputType::Text, 256, this);
    conversationLog.push_back("NPC: Hello!");
}

void GUIWindow_AIDialogue::Update() {
    render->DrawTextureNew(477 / 640.0f, 0, game_ui_dialogue_background);
    render->DrawTextureNew(468 / 640.0f, 0, game_ui_right_panel_frame);
    render->DrawTextureNew((pNPCPortraits_x[0][0] - 4) / 640.0f, (pNPCPortraits_y[0][0] - 4) / 480.0f, game_ui_evtnpc);
    render->DrawTextureNew(pNPCPortraits_x[0][0] / 640.0f, pNPCPortraits_y[0][0] / 480.0f, houseNpcs[0].icon);

    NPCData *pNPC = getNPCData(speakingNpcId);
    DrawTitleText(assets->pFontArrus.get(), SIDE_TEXT_BOX_POS_X, SIDE_TEXT_BOX_POS_Y, ui_game_dialogue_npc_name_color, NameAndTitle(pNPC), 3);

    int y = 50;
    for (const auto& line : conversationLog) {
        DrawText(assets->pFontArrus.get(), {13, y}, colorTable.White, line);
        y += assets->pFontArrus->GetHeight();
    }

    std::string currentInput = "You: " + keyboardInputHandler->GetTextInput();
    DrawText(assets->pFontArrus.get(), {13, y}, colorTable.Yellow, currentInput);

    if (this->keyboard_input_status == WindowInputStatus::WINDOW_INPUT_CONFIRMED) {
        std::string playerInput = keyboardInputHandler->GetTextInput();
        
        if (ascii::noCaseEquals(playerInput, "exit")) {
            engine->_messageQueue->addMessageCurrentFrame(UIMSG_Escape, 1, 0);
            return;
        }

        conversationLog.push_back("You: " + playerInput);

        if (ascii::noCaseEquals(playerInput, "gold")) {
            pParty->AddGold(100);
            conversationLog.push_back("NPC: Here is 100 gold.");
        } else {
            conversationLog.push_back("NPC: OK");
        }
        
        keyboardInputHandler->SetTextInput("");
        keyboardInputHandler->StartTextInput(Io::TextInputType::Text, 256, this);
    } else if (this->keyboard_input_status == WindowInputStatus::WINDOW_INPUT_CANCELLED) {
        engine->_messageQueue->addMessageCurrentFrame(UIMSG_Escape, 1, 0);
    }
}

void GUIWindow_AIDialogue::Release() {
    keyboardInputHandler->EndTextInput();
    if (houseNpcs[0].icon) {
        houseNpcs[0].icon->Release();
    }
    houseNpcs.clear();

    if (game_ui_dialogue_background) {
        game_ui_dialogue_background->Release();
        game_ui_dialogue_background = nullptr;
    }

    current_screen_type = prev_screen_type;
    currentSpeakingAI = nullptr;
    pParty->switchToNextActiveCharacter();
    GUIWindow::Release();
}