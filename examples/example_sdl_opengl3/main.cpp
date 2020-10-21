// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
#include "imgui_internal.h"

typedef int ImGuiMsgBoxButtons;         // -> enum ImGuiMsgBoxButtons_

enum ImGuiMsgBoxButtons_
{
    ImGuiMsgBoxButtons_None     = 0,
    ImGuiMsgBoxButtons_Ok       = 1 << 1,
    ImGuiMsgBoxButtons_Open     = 1 << 2,
    ImGuiMsgBoxButtons_Save     = 1 << 3,
    ImGuiMsgBoxButtons_Cancel   = 1 << 4,
    ImGuiMsgBoxButtons_Close    = 1 << 5,
    ImGuiMsgBoxButtons_Discard  = 1 << 6,
    ImGuiMsgBoxButtons_Apply    = 1 << 7,
    ImGuiMsgBoxButtons_Reset    = 1 << 8,
    ImGuiMsgBoxButtons_Restore  = 1 << 9,
    ImGuiMsgBoxButtons_Help     = 1 << 10,
    ImGuiMsgBoxButtons_SaveAll  = 1 << 11,
    ImGuiMsgBoxButtons_Yes      = 1 << 12,
    ImGuiMsgBoxButtons_YesToAll = 1 << 13,
    ImGuiMsgBoxButtons_No       = 1 << 14,
    ImGuiMsgBoxButtons_NoToAll  = 1 << 15,
    ImGuiMsgBoxButtons_Abort    = 1 << 16,
    ImGuiMsgBoxButtons_Retry    = 1 << 17,
    ImGuiMsgBoxButtons_Ignore   = 1 << 18,
};

namespace ImGui
{

IMGUI_API ImGuiMsgBoxButtons MsgBoxButtons(ImGuiMsgBoxButtons buttons, ImGuiMsgBoxButtons default_button = 0);                                      // Render standard buttons aligned to the right side. Returns pressed button or 0. Set nav focus to default_button if specified.
IMGUI_API ImGuiMsgBoxButtons MsgBox(const char* name, const char* message, ImGuiMsgBoxButtons buttons, ImGuiMsgBoxButtons default_button = 0); // Render a popup with a text message and one or more standard buttons.

}

// Order of this array describes button order in the widget.
static struct ImGuiMsgBoxButtonInfo
{
    ImGuiButtonFlags Button;
    const char* Name;
} StandardButtonInfo[] = {
    // Positive
    { ImGuiMsgBoxButtons_Ok,       "Ok"         },
    { ImGuiMsgBoxButtons_Open,     "Open"       },
    { ImGuiMsgBoxButtons_Save,     "Save"       },
    { ImGuiMsgBoxButtons_SaveAll,  "Save All"   },
    { ImGuiMsgBoxButtons_Yes,      "Yes"        },
    { ImGuiMsgBoxButtons_YesToAll, "Yes to All" },
    { ImGuiMsgBoxButtons_Apply,    "Apply"      },
    { ImGuiMsgBoxButtons_Retry,    "Retry"      },
    // Negative
    { ImGuiMsgBoxButtons_No,       "No"         },
    { ImGuiMsgBoxButtons_NoToAll,  "No to All"  },
    { ImGuiMsgBoxButtons_Discard,  "Discard"    },
    { ImGuiMsgBoxButtons_Restore,  "Restore"    },
    { ImGuiMsgBoxButtons_Reset,    "Reset"      },
    { ImGuiMsgBoxButtons_Abort,    "Abort"      },
    { ImGuiMsgBoxButtons_Close,    "Close"      },
    // Ignore
    { ImGuiMsgBoxButtons_Ignore,   "Ignore"     },
    { ImGuiMsgBoxButtons_Cancel,   "Cancel"     },
    // Misc
    { ImGuiMsgBoxButtons_Help,     "Help"       },
};

ImGuiMsgBoxButtons ImGui::MsgBoxButtons(ImGuiMsgBoxButtons buttons, ImGuiMsgBoxButtons default_button)
{
    IM_ASSERT(buttons != 0);

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiMsgBoxButtons pressed_button = 0;

    // Measure space needed for buttons.
    ImVec2 size_needed;
    size_needed.y = g.FontSize + g.Style.FramePadding.y * 2.0f;
    for (int i = 0; i < IM_ARRAYSIZE(StandardButtonInfo); i++)
    {
        ImGuiMsgBoxButtonInfo* info = &StandardButtonInfo[i];
        if (buttons & info->Button)
            size_needed.x += CalcTextSize(info->Name).x + g.Style.FramePadding.x * 2.0f + g.Style.ItemSpacing.x;
    }
    size_needed.x -= g.Style.ItemSpacing.x;

    // Align buttons to the bottom-right if there is enough space.
    const ImVec2 size_avail = GetContentRegionAvail();
    if (size_avail.x > size_needed.x)
        window->DC.CursorPos.x += size_avail.x - size_needed.x;
    if (size_avail.y > size_needed.y)
        window->DC.CursorPos.y += size_avail.y - size_needed.y;

    // Draw buttons.
    for (int i = 0; i < IM_ARRAYSIZE(StandardButtonInfo); i++)
    {
        ImGuiMsgBoxButtonInfo* info = &StandardButtonInfo[i];
        if (buttons & info->Button)
        {
            if (Button(info->Name))
            {
                pressed_button = info->Button;
                ImGui::CloseCurrentPopup();
            }
            if (!window->WasActive && info->Button == default_button)
                SetItemDefaultFocus();
            SameLine();
        }
    }
    NewLine();

    return pressed_button;
}

ImGuiMsgBoxButtons ImGui::MsgBox(const char* name, const char* message, ImGuiMsgBoxButtons buttons, ImGuiMsgBoxButtons default_button)
{
    IM_ASSERT(name != NULL);
    IM_ASSERT(message != NULL);
    ImGuiMsgBoxButtons pressed_button = 0;
    if (BeginPopupModal(name))
    {
        TextUnformatted(message);
        pressed_button = MsgBoxButtons(buttons, default_button);
        EndPopup();
    }
    return pressed_button;
}

// Main code
int main(int, char**)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#ifdef __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            // Demonstrate a one function call popup message box which returns a pressed button.
            if (ImGui::Button("Open Simple Popup"))
                ImGui::OpenPopup("Standard Popup");

            ImGuiMsgBoxButtons pressed = ImGui::MsgBox("Standard Popup", "A simple message box with buttons.", ImGuiMsgBoxButtons_Yes | ImGuiMsgBoxButtons_No | ImGuiMsgBoxButtons_Cancel);
            if (pressed == ImGuiMsgBoxButtons_Yes)
                printf("Simple Yes\n");
            else if (pressed == ImGuiMsgBoxButtons_No)
                printf("Simple No\n");
            else if (pressed == ImGuiMsgBoxButtons_Cancel)
                printf("Simple Cancel\n");

            // Demonstrate a custom message box which contains standard buttons.
            if (ImGui::Button("Open Custom Popup"))
                ImGui::OpenPopup("Custom Message Box");

            if (ImGui::BeginPopupModal("Custom Message Box"))
            {
                ImGui::TextUnformatted("Message box demo.");
                pressed = ImGui::MsgBoxButtons(ImGuiMsgBoxButtons_Yes | ImGuiMsgBoxButtons_No | ImGuiMsgBoxButtons_Cancel);
                if (pressed == ImGuiMsgBoxButtons_Yes)
                    printf("Custom Yes\n");
                else if (pressed == ImGuiMsgBoxButtons_No)
                    printf("Custom No\n");
                else if (pressed == ImGuiMsgBoxButtons_Cancel)
                    printf("Custom Cancel\n");
                ImGui::EndPopup();
            }

            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
