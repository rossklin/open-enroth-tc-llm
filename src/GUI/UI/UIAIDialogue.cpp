#include "UIAIDialogue.h"

#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/AssetsManager.h"
#include "GUI/GUIFont.h"
#include "GUI/GUIWindow.h"

extern GraphicsImage *game_ui_dialogue_background;

GUIWindow_AIDialogue::GUIWindow_AIDialogue(int npcId) : GUIWindow(WINDOW_Dialogue, {0, 0}, render->GetRenderDimensions()), _npcId(npcId) {
    // TODO: Implement constructor
}

void GUIWindow_AIDialogue::Update() {
    render->DrawTextureNew(477 / 640.0f, 0, game_ui_dialogue_background);
    DrawTitleText(assets->pFontArrus.get(), 13, 100, colorTable.White, "I am AI managed", 3);
}

void GUIWindow_AIDialogue::Release() {
    // TODO: Implement release logic
    GUIWindow::Release();
}