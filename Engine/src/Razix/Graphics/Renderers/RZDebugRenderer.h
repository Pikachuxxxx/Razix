#if 0
#pragma once

namespace Razix {
    namespace Graphics {
        class RZRenderPass;
        class RZPipeline;
        class RZDescriptorSet;
        class RZCommandBuffer;
        class RZUniformBuffer;
        class RZRenderable2D;
        class RZFramebuffer;
        class RZTexture;
        class RZShader;
        class RZIndexBuffer;
        class RZRenderer2D;
        class RZLineRenderer;
        class RZPointRenderer;
        class RZMaterial;
        struct RZLight;
    }    // namespace Graphics
    class RZTexture2D;
    class RZScene;
    class RZSceneCamera;

    class RAZIX_API RZDebugRenderer
    {
        friend class RZScene;
        friend class RZGraphicsPipeline;
        friend class RZApplication;

    public:
        static void Init(uint32_t width, uint32_t height);
        static void Release();

        static void Clear()
        {
            if (s_Instance)
                s_Instance->ClearInternal();
        }

        static void BeginScene(RZScene* scene, TransformComponent* overrideCameraTransform)
        {
            if (s_Instance)
                s_Instance->BeginSceneInternal(scene, s_Instance->m_OverrideCameraTransform);
        }

        static void Render()
        {
            if (s_Instance)
                s_Instance->RenderInternal();
        }
        static void SetRenderTarget(Graphics::Texture* texture, bool rebuildFramebuffer);

        DebugRenderer();
        ~DebugRenderer();

        //Note: Functions appended with 'NDT' (no depth testing) will always be rendered in the foreground. This can be useful for debugging things inside objects.

        //Draw Point (circle)
        static void DrawPoint(const glm::vector3& pos, float point_radius, const glm::vector3& colour);
        static void DrawPoint(const glm::vector3& pos, float point_radius, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));
        static void DrawPointNDT(const glm::vector3& pos, float point_radius, const glm::vector3& colour);
        static void DrawPointNDT(const glm::vector3& pos, float point_radius, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));

        //Draw Line with a given thickness
        static void DrawThickLine(const glm::vector3& start, const glm::vector3& end, float line_width, const glm::vector3& colour);
        static void DrawThickLine(const glm::vector3& start, const glm::vector3& end, float line_width, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));
        static void DrawThickLineNDT(const glm::vector3& start, const glm::vector3& end, float line_width, const glm::vector3& colour);
        static void DrawThickLineNDT(const glm::vector3& start, const glm::vector3& end, float line_width, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));

        //Draw line with thickness of 1 screen pixel regardless of distance from camera
        static void DrawHairLine(const glm::vector3& start, const glm::vector3& end, const glm::vector3& colour);
        static void DrawHairLine(const glm::vector3& start, const glm::vector3& end, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));
        static void DrawHairLineNDT(const glm::vector3& start, const glm::vector3& end, const glm::vector3& colour);
        static void DrawHairLineNDT(const glm::vector3& start, const glm::vector3& end, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));

        //Draw Matrix (x,y,z axis at pos)
        static void DrawMatrix(const Maths::Matrix4& transform_mtx);
        static void DrawMatrix(const Maths::Matrix3& rotation_mtx, const glm::vector3& position);
        static void DrawMatrixNDT(const Maths::Matrix4& transform_mtx);
        static void DrawMatrixNDT(const Maths::Matrix3& rotation_mtx, const glm::vector3& position);

        //Draw Triangle
        static void DrawTriangle(const glm::vector3& v0, const glm::vector3& v1, const glm::vector3& v2, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));
        static void DrawTriangleNDT(const glm::vector3& v0, const glm::vector3& v1, const glm::vector3& v2, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));

        //Draw Polygon (Renders as a triangle fan, so verts must be arranged in order)
        static void DrawPolygon(int n_verts, const glm::vector3* verts, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));
        static void DrawPolygonNDT(int n_verts, const glm::vector3* verts, const glm::vector4& colour = glm::vector4(1.0f, 1.0f, 1.0f, 1.0f));

        static void DebugDraw(const Maths::BoundingBox& box, const glm::vector4& edgeColour, bool cornersOnly = false, float width = 0.02f);
        static void DebugDraw(const Maths::Sphere& sphere, const glm::vector4& colour);
        static void DebugDraw(const Maths::Frustum& frustum, const glm::vector4& colour);
        static void DebugDraw(Graphics::Light* light, const Maths::Quaternion& rotation, const glm::vector4& colour);
        static void DebugDrawSphere(float radius, const glm::vector3& position, const glm::vector4& colour);
        static void DebugDrawCircle(int numVerts, float radius, const glm::vector3& position, const Maths::Quaternion& rotation, const glm::vector4& colour);
        static void DebugDrawCone(int numCircleVerts, int numLinesToCircle, float angle, float length, const glm::vector3& position, const Maths::Quaternion& rotation, const glm::vector4& colour);
        static void OnResize(uint32_t width, uint32_t height)
        {
            if (s_Instance)
                s_Instance->OnResizeInternal(width, height);
        }

        static void SetOverrideCamera(Camera* camera)
        {
            if (s_Instance) {
                s_Instance->m_OverrideCamera          = camera;
                s_Instance->m_OverrideCameraTransform = overrideCameraTransform;
            }
        }

        static DebugRenderer* GetInstance()
        {
            return s_Instance;
        }
        static void Reset()
        {
            if (s_Instance)
                s_Instance->Begin();
        }

    protected:
        //Actual functions managing data parsing to save code bloat - called by public functions
        static void GenDrawPoint(bool ndt, const glm::vector3& pos, float point_radius, const glm::vector4& colour);
        static void GenDrawThickLine(bool ndt, const glm::vector3& start, const glm::vector3& end, float line_width, const glm::vector4& colour);
        static void GenDrawHairLine(bool ndt, const glm::vector3& start, const glm::vector3& end, const glm::vector4& colour);
        static void GenDrawTriangle(bool ndt, const glm::vector3& v0, const glm::vector3& v1, const glm::vector3& v2, const glm::vector4& colour);

    private:
        void Begin();
        void BeginSceneInternal(Scene* scene, Camera* overrideCamera);
        void RenderInternal();
        void ClearInternal();

        void OnResizeInternal(uint32_t width, uint32_t height);

        static DebugRenderer* s_Instance;

   
        Camera*                  m_OverrideCamera          = nullptr;
    };
}    // namespace Razix
#endif
