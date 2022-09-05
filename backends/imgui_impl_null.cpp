// dear imgui: Null Platform Backend
// This is a dummy backend serving as an example and implementing no interfaces with underlying platform.

// Implemented features:
//  [X] Platform: Clipboard support.
//  [ ] Platform: Keyboard support. Events are not processed by this backend.
//  [ ] Platform: Gamepad support. Events are not processed by this backend.
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.

#include "imgui.h"
#include "imgui_impl_null.h"

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

bool ImGui_ImplNull_ProcessEvent(const void* event)
{
    IM_UNUSED(event);
    return false;
}

bool ImGui_ImplNull_Init(ImGui_ImplNull_InitInfo* params)
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

void ImGui_ImplNull_Shutdown()
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

void ImGui_ImplNull_NewFrame()
{
    ImGui_ImplNull_Data* bd = ImGui_ImplNull_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplNull_Init()?");
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
