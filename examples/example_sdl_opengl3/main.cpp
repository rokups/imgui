// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include "imgui_internal.h"

namespace ImGui
{

// buttons - array of button names, last array item must be NULL.
// Function returns true when button is pressed, index of pressed button is stored in pressed_button variable.
// default_button is focused for navigation by default.
// h_align allows chosing side of buttons, by default they are rendered on the right. Use 0.5f to center buttons.
IMGUI_API bool MsgBoxButtons(const char* buttons[], int* pressed_button, int default_button = 0, float h_align = 1.0f);
// buttons - a string of button names, separated with \0 character. String must end with two nulls.
IMGUI_API bool MsgBoxButtons(const char* buttons, int* pressed_button, int default_button = 0, float h_align = 1.0f);
IMGUI_API bool MsgBox(const char* name, const char* message, const char* buttons[], int* pressed_button, int default_button = 0, float h_align = 1.0f);
IMGUI_API bool MsgBox(const char* name, const char* message, const char* buttons, int* pressed_button, int default_button = 0, float h_align = 1.0f);
// message_format - a sprintf() - compatible format.
IMGUI_API bool MsgBoxF(const char* name, const char* buttons[], int* pressed_button, int default_button /*= 0*/, float h_align /*= 1.0f*/, const char* message_format, ...);
IMGUI_API bool MsgBoxF(const char* name, const char* buttons, int* pressed_button, int default_button /*= 0*/, float h_align /*= 1.0f*/, const char* message_format, ...);
IMGUI_API bool MsgBoxV(const char* name, const char* buttons[], int* pressed_button, int default_button /*= 0*/, float h_align /*= 1.0f*/, const char* message_format, va_list ap);
IMGUI_API bool MsgBoxV(const char* name, const char* buttons, int* pressed_button, int default_button /*= 0*/, float h_align /*= 1.0f*/, const char* message_format, va_list ap);

}

bool ImGui::MsgBoxButtons(const char* buttons[], int* pressed_button, int default_button, float h_align)
{
    IM_ASSERT(buttons != 0);
    IM_ASSERT(pressed_button != 0);
    IM_ASSERT(h_align >= 0.0f && h_align <= 1.0f);

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    bool pressed = false;

    // Measure space needed for buttons.
    ImVec2 size_needed;
    size_needed.y = g.FontSize + g.Style.FramePadding.y * 2.0f;
    for (int i = 0; buttons[i]; i++)
        size_needed.x += CalcTextSize(buttons[i]).x + g.Style.FramePadding.x * 2.0f + g.Style.ItemSpacing.x;
    size_needed.x -= g.Style.ItemSpacing.x;

    // Align buttons to the bottom-right if there is enough space.
    const ImVec2 size_avail = GetContentRegionAvail();
    if (size_avail.x > size_needed.x)
        window->DC.CursorPos.x += (size_avail.x - size_needed.x) * h_align;
    if (size_avail.y > size_needed.y)
        window->DC.CursorPos.y += size_avail.y - size_needed.y;

    // Draw buttons.
    for (int i = 0; buttons[i]; i++)
    {
        if (Button(buttons[i]))
        {
            pressed = true;
            *pressed_button = i;
            ImGui::CloseCurrentPopup();
        }
        if (i == default_button)
            SetItemDefaultFocus();  // FIXME: We should implement a nav feature to provide handlers for ESC/Enter.
        SameLine();
    }
    NewLine();

    return pressed;
}

template<size_t Size>
static void StringToArray(const char* str, const char* (&array)[Size])
{
    int index = 0;
    for (const char* button = str; *button; button += strlen(button) + 1)
        array[index++] = button;
    IM_ASSERT(index < IM_ARRAYSIZE(array));
    array[index] = 0;
}

bool ImGui::MsgBoxButtons(const char* buttons, int* pressed_button, int default_button, float h_align)
{
    const char* buttons_array[16];
    StringToArray(buttons, buttons_array);
    return MsgBoxButtons(buttons_array, pressed_button, default_button, h_align);
}

bool ImGui::MsgBox(const char* name, const char* message, const char* buttons[], int* pressed_button, int default_button, float h_align)
{
    IM_ASSERT(name != NULL);
    IM_ASSERT(message != NULL);
    bool pressed = false;
    if (BeginPopupModal(name))
    {
        TextUnformatted(message);
        pressed = MsgBoxButtons(buttons, pressed_button, default_button, h_align);
        if (IsKeyPressedMap(ImGuiKey_Escape))
            CloseCurrentPopup();
        EndPopup();
    }
    return pressed;
}

bool ImGui::MsgBox(const char* name, const char* message, const char* buttons, int* pressed_button, int default_button, float h_align)
{
    const char* buttons_array[16];
    StringToArray(buttons, buttons_array);
    return MsgBox(name, message, buttons_array, pressed_button, default_button, h_align);
}

bool ImGui::MsgBoxV(const char* name, const char* buttons[], int* pressed_button, int default_button, float h_align, const char* message_format, va_list ap)
{
    IM_ASSERT(name != NULL);
    IM_ASSERT(message_format != NULL);
    bool pressed = false;
    if (BeginPopupModal(name))
    {
        TextV(message_format, ap);
        pressed = MsgBoxButtons(buttons, pressed_button, default_button, h_align);
        if (IsKeyPressedMap(ImGuiKey_Escape))
            CloseCurrentPopup();
        EndPopup();
    }
    return pressed;
}

bool ImGui::MsgBoxV(const char* name, const char* buttons, int* pressed_button, int default_button, float h_align, const char* message_format, va_list ap)
{
    const char* buttons_array[16];
    StringToArray(buttons, buttons_array);
    return MsgBoxV(name, buttons_array, pressed_button, default_button, h_align, message_format, ap);
}

bool ImGui::MsgBoxF(const char* name, const char* buttons[], int* pressed_button, int default_button, float h_align, const char* message_format, ...)
{
    va_list args;
    va_start(args, message_format);
    bool pressed = MsgBoxV(name, buttons, pressed_button, default_button, h_align, message_format, args);
    va_end(args);
    return pressed;
}

bool ImGui::MsgBoxF(const char* name, const char* buttons, int* pressed_button, int default_button, float h_align, const char* message_format, ...)
{
    const char* buttons_array[16];
    StringToArray(buttons, buttons_array);
    va_list args;
    va_start(args, message_format);
    bool pressed = MsgBoxV(name, buttons_array, pressed_button, default_button, h_align, message_format, args);
    va_end(args);
    return pressed;
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
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

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
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
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
        ImGui_ImplSDL2_NewFrame();
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

            int pressed_button = 0;
            static const char* button_names[] = {"Yes", "No", "Cancel", NULL};

            if (ImGui::MsgBox("Standard Popup", "A simple message box with buttons.", button_names, &pressed_button, 0, 0.5f))
                printf("Simple %s\n", button_names[pressed_button]);

            // Demonstrate a custom message box which contains standard buttons.
            if (ImGui::Button("Open Custom Popup"))
                ImGui::OpenPopup("Custom Message Box");

            if (ImGui::BeginPopupModal("Custom Message Box"))
            {
                ImGui::TextUnformatted("Message box demo.");
                if (ImGui::MsgBoxButtons("Yes\0No\0Cancel\0", &pressed_button))
                    printf("Custom %s\n", button_names[pressed_button]);
                if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::MsgBoxButtons("Yes\0No\0Cancel\0", &pressed_button, 0, 0.0f);
            ImGui::MsgBoxButtons("Yes\0No\0Cancel\0", &pressed_button, 0, 0.3f);
            ImGui::MsgBoxButtons("Yes\0No\0Cancel\0", &pressed_button, 0, 0.5f);
            ImGui::MsgBoxButtons("Yes\0No\0Cancel\0", &pressed_button, 0, 0.8f);
            ImGui::MsgBoxButtons("Yes\0No\0Cancel\0", &pressed_button, 0, 1.0f);

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
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
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
