#include "UIAIDialogue.h"

#include <map>
#include <string>

#include "Engine/AssetsManager.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Party.h"
#include "GUI/GUIFont.h"
#include "GUI/GUIWindow.h"

extern GraphicsImage *game_ui_dialogue_background;

static const std::map<PartyAlignment, std::string> dialogueBackgroundResourceByAlignment = {
    {PartyAlignment::PartyAlignment_Good, "dpanelg"},
    {PartyAlignment::PartyAlignment_Neutral, "dpaneln"},
    {PartyAlignment::PartyAlignment_Evil, "dpanele"},
};

GUIWindow_AIDialogue::GUIWindow_AIDialogue(int npcId) : GUIWindow(WINDOW_Dialogue, {0, 0}, render->GetRenderDimensions()), _npcId(npcId) {
    if (!game_ui_dialogue_background) {
        game_ui_dialogue_background = assets->getImage_Solid(dialogueBackgroundResourceByAlignment.at(pParty->alignment));
    }
}

void GUIWindow_AIDialogue::Update() {
    render->DrawTextureNew(477 / 640.0f, 0, game_ui_dialogue_background);
    DrawTitleText(assets->pFontArrus.get(), 13, 100, colorTable.White, "I am AI managed", 3);
}

void GUIWindow_AIDialogue::Release() {
    if (game_ui_dialogue_background) {
        game_ui_dialogue_background->Release();
        game_ui_dialogue_background = nullptr;
    }
    GUIWindow::Release();
}