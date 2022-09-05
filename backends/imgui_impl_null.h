// dear imgui: Null Platform Backend
// This is a dummy backend serving as an example and implementing no interfaces with underlying platform.

// Implemented features:
//  [X] Platform: Clipboard support.
//  [ ] Platform: Keyboard support. Events are not processed by this backend.
//  [ ] Platform: Gamepad support. Events are not processed by this backend.
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

struct ImGui_ImplNull_InitInfo
{
    float   DeltaTime;
    ImVec2  WindowSize;
};

IMGUI_IMPL_API bool     ImGui_ImplNull_Init(ImGui_ImplNull_InitInfo* params);
IMGUI_IMPL_API void     ImGui_ImplNull_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplNull_NewFrame();
IMGUI_IMPL_API bool     ImGui_ImplNull_ProcessEvent(const void* event);
