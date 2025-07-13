# OpenEnroth AI Game Master Integration: Project Summary

This document outlines the project goal, code structure analysis, changes made, and the solution to the current implementation issue for integrating an AI Game Master into OpenEnroth.

## 1. Project Goal

The primary objective is to integrate a Large Language Model (LLM) to act as a dynamic Game Master (GM). The AI GM will:
-   Create backstories and context for NPCs.
-   Enable free-text dialogue between the player and NPCs.
-   Respond to player input based on NPC context and conversation history.
-   Utilize in-game tools to interact with game mechanics (e.g., give items, manage quests).

## 2. Code Structure & Key Findings

Our exploration of the codebase has revealed the following key areas for this project:

### Scripting System (Lua)
-   **Location:** [`resources/scripts/`](resources/scripts)
-   **Entry Point:** [`resources/scripts/init.lua`](resources/scripts/init.lua:1) loads all necessary game scripts.
-   **Command System:** A robust command manager exists in [`resources/scripts/dev/commands/command_manager.lua`](resources/scripts/dev/commands/command_manager.lua:1). This is the ideal place to create new "tools" for the AI to use (e.g., `giveItem`, `startQuest`). Commands are registered and interact with the C++ engine via Lua bindings.

### C++ / Lua Bindings
-   **Bridge File:** [`src/Scripting/GameBindings.cpp`](src/Scripting/GameBindings.cpp:1) is the central hub for exposing C++ functions and data structures to the Lua environment. It uses the `sol` library for these bindings.

### Dialogue System (C++)
-   **Interaction Flow:**
    1.  Player clicks an NPC, triggering a `UIMSG_StartNPCDialogue` message in [`src/Engine/Graphics/Viewport.cpp`](src/Engine/Graphics/Viewport.cpp:213).
    2.  The main game loop in [`src/Application/Game.cpp`](src/Application/Game.cpp:317) processes this message and calls `Game_StartDialogue`.
    3.  This function is the ideal injection point for our custom logic.
-   **UI Classes:**
    *   [`src/GUI/UI/UIDialogue.h`](src/GUI/UI/UIDialogue.h:1): Handles complex, branching dialogues.
    *   [`src/GUI/UI/UIBranchlessDialogue.h`](src/GUI/UI/UIBranchlessDialogue.h:1): Handles simple, non-interactive messages.
-   **Content & Logic:**
    *   Dialogue content is loaded from `.txt` files (e.g., `npctext.txt`, `npctopic.txt`) via [`src/Engine/Tables/NPCTable.cpp`](src/Engine/Tables/NPCTable.cpp:1).
    *   The scripted event flow is managed by the [`EvtInterpreter`](src/Engine/Evt/EvtInterpreter.cpp:1).

### NPC Data & State
-   **Core Struct:** The main `NPCData` struct is defined in [`src/Engine/Tables/NPCTable.h`](src/Engine/Tables/NPCTable.h:37).
-   **Initial Loading:** NPC data is first loaded from `npcdata.txt` into an `pOriginalNPCData` array within the `NPCStats::InitializeNPCData` function in [`src/Engine/Tables/NPCTable.cpp`](src/Engine/Tables/NPCTable.cpp:158).
-   **Live Data:** At party creation, the data is copied to a "live" `pNPCData` array in [`src/GUI/UI/UIPartyCreation.cpp`](src/GUI/UI/UIPartyCreation.cpp:256).

### Save/Load System (Snapshotting)
-   **Snapshot Struct:** A separate, simplified struct, `NPCData_MM7`, is used for serialization. It is defined in [`src/Engine/Snapshots/EntitySnapshots.h`](src/Engine/Snapshots/EntitySnapshots.h:202).
-   **Serialization Logic:** The `snapshot` and `reconstruct` functions in [`src/Engine/Snapshots/EntitySnapshots.cpp`](src/Engine/Snapshots/EntitySnapshots.cpp) handle the conversion between the live `NPCData` and the saved `NPCData_MM7` format.

## 3. Changes Implemented So Far

1.  **AI Flag Added:** A `bool isAIControlled` flag was added to the `NPCData` struct in [`src/Engine/Tables/NPCTable.h`](src/Engine/Tables/NPCTable.h:59).
2.  **Dialogue Hook:** The `Game_StartDialogue` function in [`src/Application/Game.cpp`](src/Application/Game.cpp:212) was modified to check the `isAIControlled` flag and route to a new `startAIDialogue` function if true.
3.  **AI Dialogue UI:**
    *   Created [`src/GUI/UI/UIAIDialogue.h`](src/GUI/UI/UIAIDialogue.h:1) and [`src/GUI/UI/UIAIDialogue.cpp`](src/GUI/UI/UIAIDialogue.cpp:1) for the new UI.
    *   Added a placeholder implementation to display "I am AI managed".
    *   Updated [`src/GUI/UI/CMakeLists.txt`](src/GUI/UI/CMakeLists.txt:1) to include the new files in the build.
4.  **Test NPC Logic:** Added logic to `NPCStats::InitializeNPCData` in [`src/Engine/Tables/NPCTable.cpp`](src/Engine/Tables/NPCTable.cpp:190) to set the `isAIControlled` flag for testing.
5.  **Debugging Setup:** Created [`.vscode/launch.json`](.vscode/launch.json:1) and [`.vscode/tasks.json`](.vscode/tasks.json:1) to enable easy debugging in VS Code, correctly setting the `OPENENROTH_MM7_PATH` environment variable.

## 4. Current Issue & The Correct Path Forward

**The Problem:** Debugging revealed that the `isAIControlled` flag was being reset. The root cause is that the flag was not being saved and loaded correctly during the game's snapshotting process.

**The Solution (The changes that were being attempted):**

To fix this permanently, the `isAIControlled` flag must be integrated into the save/load system.

1.  **Update the Snapshot Struct:**
    *   Open [`src/Engine/Snapshots/EntitySnapshots.h`](src/Engine/Snapshots/EntitySnapshots.h:1).
    *   Add `uint8_t isAIControlled;` to the `NPCData_MM7` struct.
    *   Update the `static_assert` for `NPCData_MM7` to `static_assert(sizeof(NPCData_MM7) == 0x4D);`.
    *   Update the `static_assert` for `Party_MM7` to `static_assert(sizeof(Party_MM7) == 0x1623A);` to reflect the size change.

2.  **Update Serialization Functions:**
    *   Open [`src/Engine/Snapshots/EntitySnapshots.cpp`](src/Engine/Snapshots/EntitySnapshots.cpp:1).
    *   In the `snapshot(const NPCData &src, NPCData_MM7 *dst)` function, add the line:
        ```cpp
        dst->isAIControlled = src.isAIControlled;
        ```
    *   In the `reconstruct(const NPCData_MM7 &src, NPCData *dst)` function, add the line:
        ```cpp
        dst->isAIControlled = src.isAIControlled;
        ```

This will ensure the AI-controlled state of an NPC persists across save and load cycles.

## 5. Deeper Investigation: The `isAIControlled` Flag Reset

Even with the snapshot system updated, the `isAIControlled` flag is still not set when `Game_StartDialogue` is called in a new game. Our investigation has revealed the following data flow and a new problem:

1.  **Initial Load:** The `isAIControlled` flag is correctly set to `true` in the `pOriginalNPCData` array within `NPCStats::InitializeNPCData`.
2.  **Party Creation:** The data, including the flag, is correctly copied from `pOriginalNPCData` to the live `pNPCData` array in `UIPartyCreation.cpp`. `std::array` assignment performs a deep copy, so this is not a pointer issue.
3.  **Map Load:** When a new game starts, `DoPrepareWorld` is called. This function, through either `loadAndPrepareBLV` (indoor) or `loadAndPrepareODM` (outdoor), reloads the default map state. This process **overwrites** the live `pNPCData` array with default data from the game files, clearing the `isAIControlled` flag.
4.  **The `setNPCNamesOnLoad` Fix:** We attempted to fix this by re-syncing the `isAIControlled` flag in `NPCStats::setNPCNamesOnLoad`, which is called after the map is loaded. Debugging confirms the flag is correctly set in `pNPCData` at this stage.
5.  **The Lingering Problem:** Despite the flag being set correctly after map load, it is *still* not set when `Game_StartDialogue` is called. This indicates another data manipulation step is occurring between `setNPCNamesOnLoad` and the dialogue initiation. The function `InitializeAdditionalNPCs` was investigated but appears to be for dynamically spawned peasants and is not the cause.

The next step is to analyze how `Game_StartDialogue` retrieves the NPC data via the `getNPCData` function to understand why it's not seeing the correct, updated flag.