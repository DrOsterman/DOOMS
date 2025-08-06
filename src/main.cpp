//
// Created by Ryan Matimu on 8/3/25.
//
# include <bgfx/bgfx.h>
# include <cstdio>
# include <cstdlib>
# include <iostream>
# include <SDL.h>
# include <SDL_metal.h>
# include "bgfx/platform.h"
# include "bx/math.h"
# include <unistd.h> // for getcwd

const uint32_t WNDW_WIDTH = 1600;
const uint32_t WNDW_HEIGHT = 900;



struct PosColorVertex {
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cubeVertices[] = {
    // Front face
    { -1.0f,  1.0f,  1.0f, 0xff0000ff }, // 0
    {  1.0f,  1.0f,  1.0f, 0xff00ff00 }, // 1
    { -1.0f, -1.0f,  1.0f, 0xffff0000 }, // 2
    {  1.0f, -1.0f,  1.0f, 0xffffffff }, // 3

    // Back face
    { -1.0f,  1.0f, -1.0f, 0xff0000ff }, // 4
    {  1.0f,  1.0f, -1.0f, 0xff00ff00 }, // 5
    { -1.0f, -1.0f, -1.0f, 0xffff0000 }, // 6
    {  1.0f, -1.0f, -1.0f, 0xffffffff }, // 7
};

static const uint16_t cubeTriangleIndices[] = {
    0, 1, 2,  // Front face
    1, 3, 2,

    4, 6, 5,  // Back face
    5, 6, 7,

    4, 5, 0,  // Top face
    5, 1, 0,

    2, 3, 6,  // Bottom face
    3, 7, 6,

    4, 0, 6,  // Left face
    0, 2, 6,

    1, 5, 3,  // Right face
    5, 7, 3,
};

bgfx::ShaderHandle loadShader(const char *filename);

int main() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("Current working directory: %s\n", cwd);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "SDL + Metal",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WNDW_WIDTH, WNDW_HEIGHT,
        SDL_WINDOW_METAL | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cout << "failed create window" << SDL_GetError() << std::endl;
        return -1;
    }

    void *metalView = SDL_Metal_CreateView(window);
    void *metalLayer = SDL_Metal_GetLayer(metalView);


    bgfx::PlatformData pdata;
    pdata.nwh = metalLayer;
    pdata.ndt = nullptr;
    bgfx::setPlatformData(pdata);

    bgfx::Init bgfxInit;
    bgfxInit.platformData = pdata;
    bgfxInit.type = bgfx::RendererType::Metal;
    bgfxInit.resolution.width = WNDW_WIDTH;
    bgfxInit.resolution.height = WNDW_HEIGHT;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;


    std::cout << "Attempting to initialize bgfx..." << std::endl;
    std::cout << "Renderer type: " << (int) bgfxInit.type << std::endl;
    std::cout << "Resolution: " << bgfxInit.resolution.width << "x" << bgfxInit.resolution.height <<
            std::endl;
    std::cout << "Platform handle (nwh): " << pdata.nwh << std::endl;
    std::cout << "metalLayer: " << metalLayer << std::endl;


    if (!bgfx::init(bgfxInit)) {
        std::cout << "failed bgfx init" << std::endl;
        SDL_Metal_DestroyView(metalView);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }


    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(cubeTriangleIndices, sizeof(cubeTriangleIndices)));


    bgfx::ShaderHandle vsh = loadShader("vs_cubes.bin");
    bgfx::ShaderHandle fsh = loadShader("fs_cubes.bin");
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);


    unsigned int counter = 0;
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);

        const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
        const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
        float view[16];
        bx::mtxLookAt(view, eye, at);
        float proj[16];
        bx::mtxProj(proj, 60.0f,
                    float(WNDW_WIDTH) / float(WNDW_HEIGHT),
                    0.1f,
                    100.0f,
                    bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);
        float mtx[16];
        bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
        bgfx::setTransform(mtx);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_CULL_CCW); // or _CCW depending on your winding

        // bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_PT_LINES);

        bgfx::submit(0, program);
        bgfx::touch(0);
        bgfx::frame();
        counter++;
    }

    SDL_Metal_DestroyView(metalView);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


bgfx::ShaderHandle loadShader(const char *filename) {
    const char *shaderPath = "???";

    switch (bgfx::getRendererType()) {
        case bgfx::RendererType::Metal: shaderPath = "shaders/metal/";
            break;
        case bgfx::RendererType::Noop:
            break;
        case bgfx::RendererType::OpenGL: shaderPath = "shaders/glsl/";
            break;
        case bgfx::RendererType::Vulkan: shaderPath = "shaders/spirv/";
            break;
        default:
            fprintf(stderr, "Unsupported renderer.\n");
            return bgfx::ShaderHandle{bgfx::kInvalidHandle};
    }

    size_t shaderPathLen = strlen(shaderPath);
    size_t fileLen = strlen(filename);
    char *filePath = (char *) malloc(shaderPathLen + fileLen + 1);
    memcpy(filePath, shaderPath, shaderPathLen);
    memcpy(&filePath[shaderPathLen], filename, fileLen);
    filePath[shaderPathLen + fileLen] = '\0';
    FILE *file = fopen(filePath, "rb");

    if (!file) {
        fprintf(stderr, "Failed to open shader file: %s\n", filePath);
        free(filePath);
        return bgfx::ShaderHandle{bgfx::kInvalidHandle};
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
    fread(mem->data, 1, fileSize, file);
    mem->data[mem->size - 1] = '\0';
    fclose(file);

    return bgfx::createShader(mem);
}
