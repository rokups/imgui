// dear imgui: Null Platform Backend
// This is a dummy backend serving as an example and implementing no interfaces with underlying platform.

// Implemented features:
//  [X] Platform: Clipboard support.
//  [ ] Platform: Keyboard support. Events are not processed by this backend.
//  [ ] Platform: Gamepad support. Events are not processed by this backend.
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

//-------------------------------------------------------------------------
// [SECTION] NULL PLATFORM BACKEND
//-------------------------------------------------------------------------

struct ImGui_ImplNull_InitInfo
{
    float   DeltaTime;
    ImVec2  WindowSize;
};

IMGUI_IMPL_API bool     ImGui_ImplNullPlatform_Init(ImGui_ImplNull_InitInfo* params);
IMGUI_IMPL_API void     ImGui_ImplNullPlatform_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplNullPlatform_NewFrame();
IMGUI_IMPL_API bool     ImGui_ImplNullPlatform_ProcessEvent(const void* event);

//-------------------------------------------------------------------------
// [SECTION] NULL GRAPHICS BACKEND
//-------------------------------------------------------------------------

IMGUI_IMPL_API bool     ImGui_ImplNullRenderer_Init();
IMGUI_IMPL_API void     ImGui_ImplNullRenderer_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplNullRenderer_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplNullRenderer_RenderDrawData(ImDrawData* draw_data);

// Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool     ImGui_ImplNullRenderer_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplNullRenderer_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplNullRenderer_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplNullRenderer_DestroyDeviceObjects();
