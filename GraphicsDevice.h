#pragma once

#include "GraphicsDefs.h"
#include "GraphicsContext.h"

// Graphics device is responsible for displaying game graphics

class GraphicsDevice final: public cxx::noncopyable
{
public:
    // public for convenience, don't change these fields directly
    RenderStates mCurrentStates;
    Rect2D mViewportRect;

public:
    GraphicsDevice();
    ~GraphicsDevice();

    // Initialize graphics system, setup resolution and vsync mode
    // @param screensizex, screensizey: Screen dimensions
    // @param fullscreen: Fullscreen or windowed mode
    // @param vsync: Vertical synchronization enabled or disabled
    bool Initialize(int screensizex, int screensizey, bool fullscreen, bool vsync);

    // Shutdown graphics system, any render operations will be ignored after this
    void Deinit();

    // Turn vsync mode on or off
    // @param vsyncEnabled: True to enable or false to disable
    void EnableVSync(bool vsyncEnabled);

    // Turn fullscreen mode on or off
    // @param fullscreenEnabled: True to fullscreen or false to windowed
    void EnableFullscreen(bool fullscreenEnabled);

    // Create 2D texture, client is responsible for destroying resource
    GpuTexture2D* CreateTexture2D();

    // Create render program, client is responsible for destroying resource
    GpuProgram* CreateRenderProgram();

    // Create hardware buffer, client is responsible for destroying resource
    GpuBuffer* CreateBuffer();

    // Set source buffer for geometries vertex data and setup layout for bound shader
    // @param sourceBuffer: Buffer reference or nullptr to unbind current
    // @param streamDefinition: Layout
    void BindVertexBuffer(GpuBuffer* sourceBuffer, const VertexFormat& streamDefinition);

    // Set source buffer for geometries index data
    // @param sourceBuffer: Buffer reference or nullptr to unbind current
    void BindIndexBuffer(GpuBuffer* sourceBuffer);

    // Set source texture on specified texture unit
    // @param textureUnit: Target unit
    // @param texture2D: Texture
    void BindTexture2D(eTextureUnit textureUnit, GpuTexture2D* texture2D);

    // Set source render program to render with
    // @param program: Target program
    void BindRenderProgram(GpuProgram* program);

    // Free hardware resource
    // @param textureResource: Target texture, pointer becomes invalid
    void DestroyTexture2D(GpuTexture2D* textureResource);

    // Free hardware resource
    // @param programResource: Target render program, pointer becomes invalid
    void DestroyRenderProgram(GpuProgram* programResource);

    // Free hardware resource
    // @param bufferResource: Target buffer, pointer becomes invalid
    void DestroyBuffer(GpuBuffer* bufferResource);

    // Set current render states
    // @param renderStates: Render states
    void SetRenderStates(const RenderStates& renderStates)
    {
        InternalSetRenderStates(renderStates, false);
    }

    // Render indexed geometry
    // @param primitiveType: Type of primitives to render
    // @param indexOffset: Offset within index buffer in bytes
    // @param numIndices: Number of elements
    // @param baseVertex: Specifies a constant that should be added to each element of indices when chosing elements from the vertex arrays
    void RenderIndexedPrimitives(ePrimitiveType primitiveType, unsigned int dataOffset, unsigned int numIndices);
    void RenderIndexedPrimitives(ePrimitiveType primitiveType, unsigned int dataOffset, unsigned int numIndices, unsigned int baseVertex);

    // Render geometry
    // @param primitiveType: Type of primitives to render
    // @param firstIndex: Start position in attribute buffers, index
    // @param numElements: Number of elements to render
    void RenderPrimitives(ePrimitiveType primitiveType, unsigned int firstIndex, unsigned int numElements);

    // Finish render frame, prenent on screen
    void Present();

    // Setup dimensions of graphic device viewport
    // @param sourceRectangle: Viewport rectangle
    void SetViewportRect(const Rect2D& sourceRectangle);

    // Set clear color for render revice
    // @param clearColor: Color components
    void SetClearColor(Color32 clearColor);

    // Clear color and depth of current framebuffer
    void ClearScreen();

    // Test whether graphics is initialized properly
    bool IsDeviceInited() const;
    
private:
    // Force render state
    // @param rstate: Render state
    void InternalSetRenderStates(const RenderStates& renderStates, bool forceState);
    bool InitializeOGLExtensions();

    void SetupVertexAttributes(const VertexFormat& streamDefinition);

private:
    GraphicsContext mGraphicsContext;
    GLFWwindow* mGraphicsWindow;
    GLFWmonitor* mGraphicsMonitor;
};

extern GraphicsDevice gGraphicsDevice;