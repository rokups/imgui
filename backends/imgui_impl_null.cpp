// dear imgui: Null Platform Backend
// This is a dummy backend serving as an example and implementing no interfaces with underlying platform.

// Implemented features:
//  [X] Platform: Clipboard support.
//  [ ] Platform: Keyboard support. Events are not processed by this backend.
//  [ ] Platform: Gamepad support. Events are not processed by this backend.
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.

#include "imgui.h"
#include "imgui_impl_null.h"
#include <stdint.h>     // intptr_t

//-------------------------------------------------------------------------
// [SECTION] NULL PLATFORM BACKEND
//-------------------------------------------------------------------------

// Null Data
struct ImGui_ImplNull_Data
{
    char*   ClipboardTextData;
    float   DeltaTime;
    ImVec2  WindowSize;

    ImGui_ImplNull_Data()   { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
static ImGui_ImplNull_Data* ImGui_ImplNull_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplNull_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}

// Functions
static const char* ImGui_ImplNull_GetClipboardText(void*)
{
    ImGui_ImplNull_Data* bd = ImGui_ImplNull_GetBackendData();
    return bd->ClipboardTextData;
}

static void ImGui_ImplNull_SetClipboardText(void*, const char* text)
{
    ImGui_ImplNull_Data* bd = ImGui_ImplNull_GetBackendData();
    if (bd->ClipboardTextData)
        IM_FREE(bd->ClipboardTextData);
    bd->ClipboardTextData = (char*)IM_ALLOC(strlen(text) + 1);
    strcpy(bd->ClipboardTextData, text);
}

bool ImGui_ImplNullPlatform_ProcessEvent(const void* event)
{
    IM_UNUSED(event);
    return false;
}

bool ImGui_ImplNullPlatform_Init(ImGui_ImplNull_InitInfo* params)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    // Setup backend capabilities flags
    ImGui_ImplNull_Data* bd = IM_NEW(ImGui_ImplNull_Data)();
    bd->DeltaTime = params->DeltaTime;
    bd->WindowSize = params->WindowSize;

    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_null";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;       // We observe GetMouseCursor() values (optional)

    io.SetPlatformImeDataFn = NULL;                             // Avoid default implementation
    io.SetClipboardTextFn = ImGui_ImplNull_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplNull_GetClipboardText;
    io.ClipboardUserData = NULL;

    return true;
}

void ImGui_ImplNullPlatform_Shutdown()
{
    ImGui_ImplNull_Data* bd = ImGui_ImplNull_GetBackendData();
    IM_ASSERT(bd != NULL && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    if (bd->ClipboardTextData)
        IM_FREE(bd->ClipboardTextData);

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;
    IM_DELETE(bd);
}

void ImGui_ImplNullPlatform_NewFrame()
{
    ImGui_ImplNull_Data* bd = ImGui_ImplNull_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplNullPlatform_Init()?");
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = bd->WindowSize;
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.DeltaTime = bd->DeltaTime;

    if (io.Fonts->TexPixelsRGBA32 == NULL)
    {
        unsigned char* tex_pixels = NULL;
        int tex_w, tex_h;
        io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);
    }
}

//-------------------------------------------------------------------------
// [SECTION] NULL GRAPHICS BACKEND
//-------------------------------------------------------------------------

struct ImGui_ImplNullRenderer_Data
{
    void*       FontTexture;

    ImGui_ImplNullRenderer_Data() { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplNullRenderer_Data* ImGui_ImplNullRenderer_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplNullRenderer_Data*)ImGui::GetIO().BackendRendererUserData : NULL;
}

bool ImGui_ImplNullRenderer_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendRendererUserData == NULL && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    ImGui_ImplNullRenderer_Data* bd = IM_NEW(ImGui_ImplNullRenderer_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_null";

    return true;
}

void ImGui_ImplNullRenderer_Shutdown()
{
    ImGui_ImplNullRenderer_Data* bd = ImGui_ImplNullRenderer_GetBackendData();
    IM_ASSERT(bd != NULL && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplNullRenderer_DestroyDeviceObjects();
    io.BackendRendererName = NULL;
    io.BackendRendererUserData = NULL;
    IM_DELETE(bd);
}

void ImGui_ImplNullRenderer_NewFrame()
{
    ImGui_ImplNullRenderer_Data* bd = ImGui_ImplNullRenderer_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplNullRenderer_Init()?");

    if (!bd->FontTexture)
        ImGui_ImplNullRenderer_CreateDeviceObjects();
}

void ImGui_ImplNullRenderer_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback != ImDrawCallback_ResetRenderState)
                    pcmd->UserCallback(cmd_list, pcmd);
            }
        }
    }
}

bool ImGui_ImplNullRenderer_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplNullRenderer_Data* bd = ImGui_ImplNullRenderer_GetBackendData();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    bd->FontTexture = io.Fonts->TexPixelsRGBA32;
    io.Fonts->SetTexID((ImTextureID)(intptr_t)bd->FontTexture);

    return true;
}

void ImGui_ImplNullRenderer_DestroyFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplNullRenderer_Data* bd = ImGui_ImplNullRenderer_GetBackendData();
    if (bd->FontTexture)
    {
        io.Fonts->SetTexID(0);
        bd->FontTexture = NULL;
    }
}

bool ImGui_ImplNullRenderer_CreateDeviceObjects()
{
    return ImGui_ImplNullRenderer_CreateFontsTexture();
}

void ImGui_ImplNullRenderer_DestroyDeviceObjects()
{
    ImGui_ImplNullRenderer_DestroyFontsTexture();
}
