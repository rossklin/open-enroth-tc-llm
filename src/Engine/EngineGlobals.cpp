#include "EngineGlobals.h"

#include "Library/Platform/Application/PlatformApplication.h"
#include "LLM/GMState.h"

Platform *platform = nullptr;
PlatformWindow *window = nullptr;
PlatformOpenGLContext *openGLContext = nullptr;
PlatformEventLoop *eventLoop = nullptr;
PlatformEventHandler *eventHandler = nullptr;
PlatformApplication *application = nullptr;
ScriptingSystem *scriptingSystem = nullptr;
GMState *pGMState = nullptr;


void detail::globalProcessMessages() {
    application->processMessages();
}

void detail::globalWaitForMessages() {
    application->waitForMessages();
}
