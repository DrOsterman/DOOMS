//
// Created by Ryan Matimu on 8/3/25.
//

# include <cstdlib>
# include <iostream>
# include "bgfx/bgfx.h"
# include <GLFW/glfw3.h>
# define GLFW_EXPOSE_NATIVE_COCOA
# include <GLFW/glfw3native.h>

# include "bgfx/platform.h"

const uint32_t WNDW_WIDTH = 1600;
const uint32_t WNDW_HEIGHT = 900;

int main()
{
    std::cout << "GLFW Version: " << glfwGetVersionString() << std::endl;
    if (!glfwInit()) {
        std::cout << "failed glfw init" << glfwGetVersionString() << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Don't use OpenGL context
    GLFWwindow *window = glfwCreateWindow(WNDW_WIDTH, WNDW_HEIGHT, "BGFX + GLFW Test", NULL, NULL);
    if (!window) {
        std::cout << "failed create window" << std::endl;
        glfwTerminate();
        return -1;
    }

#if BX_PLATFORM_OSX
    // 👇 Attach CAMetalLayer to the NSView
    NSWindow* nsWindow = (NSWindow*)glfwGetCocoaWindow(window);
    NSView* contentView = [nsWindow contentView];

    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    [metalLayer setPixelFormat:MTLPixelFormatBGRA8Unorm];
    [contentView setLayer:metalLayer];
    [contentView setWantsLayer:YES];
#endif

    // ✅ Setup BGFX platform data
    bgfx::PlatformData pData{};
#if BX_PLATFORM_OSX
    pData.nwh = (void*)[contentView layer]; // <- Pass the Metal layer itself!
#else
    pData.nwh = window;
#endif


    // bgfx::PlatformData pdata;
    // pdata.nwh = glfwGetCocoaWindow(window);
    // pdata.ndt = nullptr;
    bgfx::setPlatformData(pData);

    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::Metal;
    bgfxInit.resolution.width = WNDW_WIDTH;
    bgfxInit.resolution.height = WNDW_HEIGHT;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;


    std::cout << "Attempting to initialize bgfx..." << std::endl;
    std::cout << "Renderer type: " << (int)bgfxInit.type << std::endl;
    std::cout << "Resolution: " << bgfxInit.resolution.width << "x" << bgfxInit.resolution.height << std::endl;
    std::cout << "Platform handle (nwh): " << pData.nwh << std::endl;


    if (!bgfx::init(bgfxInit)) {
        std::cout << "failed bgfx init" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }


    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);


    while (!glfwWindowShouldClose(window)) {
        bgfx::touch(0);
        bgfx::frame();
        glfwPollEvents();
    }

    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
