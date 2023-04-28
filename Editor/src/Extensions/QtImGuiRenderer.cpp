// clang-format off
#include "rzepch.h"
// clang-format on
#include "QtImGuiRenderer.h"

#include <QDateTime>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QClipboard>
#include <QCursor>


#ifdef USE_GLSL_ES
#define IMGUIRENDERER_GLSL_VERSION "#version 300 es\n"
#else
#define IMGUIRENDERER_GLSL_VERSION "#version 330\n"
#endif

namespace QtImGui {

namespace {

// Keyboard mapping. Dear ImGui use those indices to peek into the io.KeysDown[] array.
const QHash<int, ImGuiKey> keyMap = {
    { Qt::Key_Tab, ImGuiKey_Tab },
    { Qt::Key_Left, ImGuiKey_LeftArrow },
    { Qt::Key_Right, ImGuiKey_RightArrow },
    { Qt::Key_Up, ImGuiKey_UpArrow },
    { Qt::Key_Down, ImGuiKey_DownArrow },
    { Qt::Key_PageUp, ImGuiKey_PageUp },
    { Qt::Key_PageDown, ImGuiKey_PageDown },
    { Qt::Key_Home, ImGuiKey_Home },
    { Qt::Key_End, ImGuiKey_End },
    { Qt::Key_Insert, ImGuiKey_Insert },
    { Qt::Key_Delete, ImGuiKey_Delete },
    { Qt::Key_Backspace, ImGuiKey_Backspace },
    { Qt::Key_Space, ImGuiKey_Space },
    { Qt::Key_Enter, ImGuiKey_Enter },
    { Qt::Key_Return, ImGuiKey_Enter },
    { Qt::Key_Escape, ImGuiKey_Escape },
    { Qt::Key_A, ImGuiKey_A },
    { Qt::Key_C, ImGuiKey_C },
    { Qt::Key_V, ImGuiKey_V },
    { Qt::Key_X, ImGuiKey_X },
    { Qt::Key_Y, ImGuiKey_Y },
    { Qt::Key_Z, ImGuiKey_Z },
    { Qt::MiddleButton, ImGuiMouseButton_Middle }
};

#ifndef QT_NO_CURSOR
const QHash<ImGuiMouseCursor, Qt::CursorShape> cursorMap = {
    { ImGuiMouseCursor_Arrow,      Qt::CursorShape::ArrowCursor },
    { ImGuiMouseCursor_TextInput,  Qt::CursorShape::IBeamCursor },
    { ImGuiMouseCursor_ResizeAll,  Qt::CursorShape::SizeAllCursor },
    { ImGuiMouseCursor_ResizeNS,   Qt::CursorShape::SizeVerCursor },
    { ImGuiMouseCursor_ResizeEW,   Qt::CursorShape::SizeHorCursor },
    { ImGuiMouseCursor_ResizeNESW, Qt::CursorShape::SizeBDiagCursor },
    { ImGuiMouseCursor_ResizeNWSE, Qt::CursorShape::SizeFDiagCursor },
    { ImGuiMouseCursor_Hand,       Qt::CursorShape::PointingHandCursor },
    { ImGuiMouseCursor_NotAllowed, Qt::CursorShape::ForbiddenCursor },
};
#endif

QByteArray g_currentClipboardText;

} // namespace

void QtImGuiRenderer::initialize(WindowWrapper *window) {
    m_window.reset(window);
    initializeOpenGLFunctions();

    g_ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(g_ctx);

    // Setup backend capabilities flags
    ImGuiIO &io = ImGui::GetIO();
    #ifndef QT_NO_CURSOR
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)
    #endif
    io.BackendPlatformName = "qtimgui";
    
    // Setup keyboard mapping
    for (ImGuiKey key : keyMap.values()) {
        io.KeyMap[key] = key;
    }
    
    // io.RenderDrawListsFn = [](ImDrawData *drawData) {
    //    instance()->renderDrawList(drawData);
    // };
    io.SetClipboardTextFn = [](void *user_data, const char *text) {
        Q_UNUSED(user_data);
        QGuiApplication::clipboard()->setText(text);
    };
    io.GetClipboardTextFn = [](void *user_data) {
        Q_UNUSED(user_data);
        g_currentClipboardText = QGuiApplication::clipboard()->text().toUtf8();
        return (const char *)g_currentClipboardText.data();
    };

    window->installEventFilter(this);
}

void QtImGuiRenderer::renderDrawList(ImDrawData *draw_data)
{
    // Select current context
    ImGui::SetCurrentContext(g_ctx);

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    const ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
    GLint last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
    GLint last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
    GLint last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
    GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(g_VaoHandle);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle (Y is inverted in OpenGL)
                glScissor((int)clip_min.x, (int)(fb_height - clip_max.y), (int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y));

                 // Bind texture, Draw
                glBindTexture(GL_TEXTURE_2D, (GLuint)(size_t)pcmd->TextureId);
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset + pcmd->IdxOffset);
            }
        }
    }

    // Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glActiveTexture(last_active_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

bool QtImGuiRenderer::createFontsTexture()
{
    // Select current context
    ImGui::SetCurrentContext(g_ctx);

    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(size_t)g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

bool QtImGuiRenderer::createDeviceObjects()
{
    // Select current context
    ImGui::SetCurrentContext(g_ctx);

    // Backup GL state
    GLint last_texture, last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar *vertex_shader =
        IMGUIRENDERER_GLSL_VERSION
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        IMGUIRENDERER_GLSL_VERSION
        "precision mediump float;"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
    g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    glGenVertexArrays(1, &g_VaoHandle);
    glBindVertexArray(g_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    createFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);

    return true;
}

void QtImGuiRenderer::newFrame()
{
    // Select current context
    ImGui::SetCurrentContext(g_ctx);

    if (!g_FontTexture)
        createDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2(m_window->size().width(), m_window->size().height());
    io.DisplayFramebufferScale = ImVec2(m_window->devicePixelRatio(), m_window->devicePixelRatio());

    // Setup time step
    double current_time =  QDateTime::currentMSecsSinceEpoch() / double(1000);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
    if (io.DeltaTime <= 0.0f) io.DeltaTime = 0.00001f;
    g_Time = current_time;
    
    
    // If ImGui wants to set cursor position (for example, during navigation by using keyboard)
    // we need to do it here (before getting `QCursor::pos()` below).
    setCursorPos(io);

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    if (m_window->isActive())
    {
        const QPoint pos = m_window->mapFromGlobal(QCursor::pos());
        io.MousePos = ImVec2(pos.x(), pos.y());   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
    }
    else
    {
        io.MousePos = ImVec2(-1,-1);
    }

    for (int i = 0; i < 3; i++)
    {
        io.MouseDown[i] = g_MousePressed[i];
    }

    io.MouseWheelH = g_MouseWheelH;
    io.MouseWheel = g_MouseWheel;
    g_MouseWheelH = 0;
    g_MouseWheel = 0;

    
    updateCursorShape(io);
    

    // Start the frame
    ImGui::NewFrame();
}

void QtImGuiRenderer::render()
{
  // Select current context
  ImGui::SetCurrentContext(g_ctx);

  auto drawData = ImGui::GetDrawData();
  renderDrawList(drawData);
}

QtImGuiRenderer::QtImGuiRenderer()
  : g_ctx(nullptr)
{
}

QtImGuiRenderer::~QtImGuiRenderer()
{
  // remove this context
  ImGui::DestroyContext(g_ctx);
}

void QtImGuiRenderer::onMousePressedChange(QMouseEvent *event)
{
    g_MousePressed[0] = event->buttons() & Qt::LeftButton;
    g_MousePressed[1] = event->buttons() & Qt::RightButton;
    g_MousePressed[2] = event->buttons() & Qt::MiddleButton;
}

void QtImGuiRenderer::onWheel(QWheelEvent *event)
{
    // Select current context
    ImGui::SetCurrentContext(g_ctx);

    // Handle horizontal component
    if(event->pixelDelta().x() != 0)
    {
        g_MouseWheelH += event->pixelDelta().x() / (ImGui::GetTextLineHeight());
    } else {
        // Magic number of 120 comes from Qt doc on QWheelEvent::pixelDelta()
        g_MouseWheelH += event->angleDelta().x() / 120.0f;
    }

    // Handle vertical component
    if(event->pixelDelta().y() != 0)
    {
        // 5 lines per unit
        g_MouseWheel += event->pixelDelta().y() / (5.0 * ImGui::GetTextLineHeight());
    } else {
        // Magic number of 120 comes from Qt doc on QWheelEvent::pixelDelta()
        g_MouseWheel += event->angleDelta().y() / 120.0f;
    }
}

void QtImGuiRenderer::onKeyPressRelease(QKeyEvent *event)
{
    // Select current context
    ImGui::SetCurrentContext(g_ctx);

    ImGuiIO& io = ImGui::GetIO();
    
    const bool key_pressed = (event->type() == QEvent::KeyPress);
    
    // Translate `Qt::Key` into `ImGuiKey`, and apply 'pressed' state for that key
    const auto key_it = keyMap.constFind( event->key() );
    if (key_it != keyMap.constEnd()) { // Qt's key found in keyMap
        const int imgui_key = *(key_it);
        io.KeysDown[imgui_key] = key_pressed;
    }

    if (key_pressed) {
        const QString text = event->text();
        if (text.size() == 1) {
            io.AddInputCharacter( text.at(0).unicode() );
        }
    }

#ifdef Q_OS_MAC
    io.KeyCtrl  = event->modifiers() & Qt::MetaModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt   = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::ControlModifier; // Comamnd key
#else
    io.KeyCtrl  = event->modifiers() & Qt::ControlModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt   = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::MetaModifier;
#endif
}

void QtImGuiRenderer::updateCursorShape(const ImGuiIO& io)
{
    // NOTE: This code will be executed, only if the following flags have been set:
    // - backend flag: `ImGuiBackendFlags_HasMouseCursors`    - enabled
    // - config  flag: `ImGuiConfigFlags_NoMouseCursorChange` - disabled

#ifndef QT_NO_CURSOR
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    const ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (io.MouseDrawCursor || (imgui_cursor == ImGuiMouseCursor_None))
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        m_window->setCursorShape(Qt::CursorShape::BlankCursor);
    }
    else
    {
        // Show OS mouse cursor
        
        // Translate `ImGuiMouseCursor` into `Qt::CursorShape` and show it, if we can
        const auto cursor_it = cursorMap.constFind( imgui_cursor );
        if(cursor_it != cursorMap.constEnd()) // `Qt::CursorShape` found for `ImGuiMouseCursor`
        {
            const Qt::CursorShape qt_cursor_shape = *(cursor_it);
            m_window->setCursorShape(qt_cursor_shape);
        }
        else // shape NOT found - use default
        {
            m_window->setCursorShape(Qt::CursorShape::ArrowCursor);
        }
    }
#else
    Q_UNUSED(io);
#endif
}

void QtImGuiRenderer::setCursorPos(const ImGuiIO& io)
{
    // NOTE: This code will be executed, only if the following flags have been set:
    // - backend flag: `ImGuiBackendFlags_HasSetMousePos`      - enabled
    // - config  flag: `ImGuiConfigFlags_NavEnableSetMousePos` - enabled
    
#ifndef QT_NO_CURSOR
    if(io.WantSetMousePos) {
        m_window->setCursorPos({(int)io.MousePos.x, (int)io.MousePos.y});
    }
#else
    Q_UNUSED(io);
#endif
}

bool QtImGuiRenderer::eventFilter(QObject *watched, QEvent *event)
{
  if (watched == m_window->object()) {
    switch (event->type()) {
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
      this->onMousePressedChange(static_cast<QMouseEvent*>(event));
      break;
    case QEvent::Wheel:
      this->onWheel(static_cast<QWheelEvent*>(event));
      break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      this->onKeyPressRelease(static_cast<QKeyEvent*>(event));
      break;
    default:
      break;
    }
  }
  return QObject::eventFilter(watched, event);
}

QtImGuiRenderer* QtImGuiRenderer::instance() {
    static QtImGuiRenderer* instance = nullptr;
    if (!instance) {
        instance = new QtImGuiRenderer();
    }
    return instance;
}

} // namespace QtImGui
