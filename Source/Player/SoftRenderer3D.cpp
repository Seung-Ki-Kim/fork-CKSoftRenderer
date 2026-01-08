#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DDD;

// �׸��� �׸���
void SoftRenderer::DrawGizmo3D() {
    auto& r = GetRenderer();
    const GameEngine& g = Get3DGameEngine();

    // �� ����� �׸���
    std::vector<Vertex3D> viewGizmo = {
        Vertex3D(Vector4(Vector3::Zero)),
        Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
        Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
        Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
    };

    Matrix4x4 viewMatRotationOnly = g.GetMainCamera().GetViewMatrixRotationOnly();
    Matrix4x4 pvMatrix = g.GetMainCamera().GetPerspectiveViewMatrix();
    VertexShader3D(viewGizmo, viewMatRotationOnly);

    // �� �׸���
    Vector2 v0 = viewGizmo[0].Position.ToVector2() + _GizmoPositionOffset;
    Vector2 v1 = viewGizmo[1].Position.ToVector2() + _GizmoPositionOffset;
    Vector2 v2 = viewGizmo[2].Position.ToVector2() + _GizmoPositionOffset;
    Vector2 v3 = viewGizmo[3].Position.ToVector2() + _GizmoPositionOffset;
    r.DrawLine(v0, v1, LinearColor::Red);
    r.DrawLine(v0, v2, LinearColor::Green);
    r.DrawLine(v0, v3, LinearColor::Blue);

    // �ٴ� �����
    DrawMode prevShowMode = GetDrawMode();
    SetDrawMode(DrawMode::Wireframe);
    {
        static float planeScale = 100.f;
        const Mesh& planeMesh = g.GetMesh(GameEngine::PlaneMesh);
        Transform pt(Vector3::Zero, Quaternion::Identity, Vector3::One * planeScale);
        DrawMesh3D(planeMesh, pvMatrix * pt.GetMatrix(), _WireframeColor);
    }
    SetDrawMode(prevShowMode);
}

// ���� ����
void SoftRenderer::Update3D(float InDeltaSeconds) {
    // �⺻ ���۷���
    GameEngine& g = Get3DGameEngine();
    const InputManager& input = g.GetInputManager();
    CameraObject& camera = g.GetMainCamera();

    // �⺻ ���� ����
    static float fovSpeed = 100.f;
    static float rotateSpeed = 180.f;
    static float moveSpeed = 500.f;

    // ���� �������� ����� ���� ������Ʈ ���۷���
    GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);
    GameObject& goCameraRig = g.GetGameObject(GameEngine::CameraRigGo);

    goPlayer.GetTransform().AddLocalYawRotation(-input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds);
    goPlayer.GetTransform().AddLocalPosition(
        goPlayer.GetTransform().GetLocalZ() * input.GetAxis(InputAxis::YAxis) * moveSpeed * InDeltaSeconds);

    // ī�޶� ȭ�� ����
    float newFOV = Math::Clamp(camera.GetFOV() + input.GetAxis(InputAxis::ZAxis) * fovSpeed * InDeltaSeconds, 5.f,
                               179.f);
    camera.SetFOV(newFOV);
}

// ĳ���� �ִϸ��̼� ����
void SoftRenderer::LateUpdate3D(float InDeltaSeconds) {
    // �⺻ ���۷���
    GameEngine& g = Get3DGameEngine();

    // �⺻ ���� ����
    static float elapsedTime = 0.f;
    static float neckLength = 5.f;
    static float armLegLength = 0.7f;
    static float neckDegree = 15.f;
    static float armLegDegree = 30.f;
    elapsedTime += InDeltaSeconds;

    // �ִϸ��̼��� ���� Ŀ�� ���� 
    float armLegCurrent = Math::FMod(elapsedTime, armLegLength) * Math::TwoPI / armLegLength;
    float neckCurrent = Math::FMod(elapsedTime, neckLength) * Math::TwoPI / neckLength;

    float armLegCurve = sinf(armLegCurrent) * armLegDegree;
    float neckCurve = sinf(neckCurrent) * neckDegree;

    // ĳ���� ���۷���
    GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);

    // ĳ���� �޽�
    Mesh& m = g.GetMesh(goPlayer.GetMeshKey());

    // ���� ȸ��
    Bone& neckBone = m.GetBone(GameEngine::NeckBone);
    neckBone.GetTransform().SetLocalRotation(Rotator(neckCurve, 0.f, 0.f));

    // ���� ȸ��
    Bone& leftArmBone = m.GetBone(GameEngine::LeftArmBone);
    leftArmBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, armLegCurve));

    Bone& rightArmBone = m.GetBone(GameEngine::RightArmBone);
    rightArmBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, -armLegCurve));

    // �ٸ��� ȸ��
    Bone& leftLegBone = m.GetBone(GameEngine::LeftLegBone);
    leftLegBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, -armLegCurve));

    Bone& rightLegBone = m.GetBone(GameEngine::RightLegBone);
    rightLegBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, armLegCurve));
}

// ������ ����
void SoftRenderer::Render3D() {
    const GameEngine& g = Get3DGameEngine();

    const CameraObject& mainCamera = g.GetMainCamera();
    const Matrix4x4 pvMatrix = mainCamera.GetPerspectiveViewMatrix();
    const ScreenPoint viewportSize = mainCamera.GetViewportSize();
    float nearZ = mainCamera.GetNearZ();
    float farZ = mainCamera.GetFarZ();

    // ����� �׸���
    DrawGizmo3D();

    for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it) {
        const GameObject& gameObject = *(*it);
        const TransformComponent& transform = gameObject.GetTransform();
        if (!gameObject.HasMesh() || !gameObject.IsVisible()) {
            continue;
        }

        // ������ ����
        const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
        // ��Ű���̰� WireFrame�� ��� ���� �׸���
        if (mesh.IsSkinnedMesh() && IsWireframeDrawing()) {
            const Mesh& boneMesh = g.GetMesh(GameEngine::ArrowMesh);
            for (const auto& b : mesh.GetBones()) {
                if (!b.second.HasParent()) {
                    continue;
                }
                const Bone& bone = b.second;
                const Bone& parentBone = mesh.GetBone(bone.GetParentName());
                const Transform& tGameObject = transform.GetWorldTransform();

                // �𵨸� ���������� ���� ��ġ
                const Transform& t1 = parentBone.GetTransform().GetWorldTransform();
                const Transform& t2 = bone.GetTransform().GetWorldTransform();

                // ���� ���� ���������� ���� ��ġ
                const Transform& wt1 = t1.LocalToWorld(tGameObject);
                const Transform& wt2 = t2.LocalToWorld(tGameObject);

                Vector3 boneVector = wt2.GetPosition() - wt1.GetPosition();
                Transform tboneObject(wt1.GetPosition(), Quaternion(boneVector),
                                      Vector3(10.f, 10.f, boneVector.Size()));
                Matrix4x4 boneMatrix = pvMatrix * tboneObject.GetMatrix();
                DrawMesh3D(boneMesh, boneMatrix, LinearColor::Red);
            }
        }

        // ���� ��ȯ ���
        Matrix4x4 finalMatrix = pvMatrix * transform.GetWorldMatrix();
        DrawMesh3D(mesh, finalMatrix, gameObject.GetColor());
    }
}

void SoftRenderer::DrawMesh3D(const Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor) {
    size_t vertexCount = InMesh.GetVertices().size();
    size_t indexCount = InMesh.GetIndices().size();
    size_t triangleCount = indexCount / 3;

    // �������� ����� ���� ���ۿ� �ε��� ���۷� ��ȯ
    std::vector<Vertex3D> vertices(vertexCount);
    std::vector<size_t> indice(InMesh.GetIndices());
    for (size_t vi = 0; vi < vertexCount; ++vi) {
        vertices[vi].Position = Vector4(InMesh.GetVertices()[vi]);

        //���̷�Ż �޽ø� ��Ű�� �۾� ����
        if (InMesh.IsSkinnedMesh()) {
            Vector4 totalPosition = Vector4::Zero;
            Weight w = InMesh.GetWeights()[vi];
            for (size_t wi = 0; wi < InMesh.GetConnectedBones()[vi]; ++wi) {
                std::string boneName = w.Bones[wi];
                if (InMesh.HasBone(boneName)) {
                    const Bone& b = InMesh.GetBone(boneName);
                    const Transform& t = b.GetTransform().GetWorldTransform(); // ���� ����
                    const Transform& bindPose = b.GetBindPose(); // ���� ����

                    // BindPose ������ �߽����� Bone�� ���� ������ ���
                    Transform boneLocal = t.WorldToLocal(bindPose);

                    // BindPose �������� ���� ��ȭ
                    Vector3 localPosition = bindPose.Inverse().GetPosition() + vertices[vi].Position.ToVector3();

                    // BindPose ���������� ���� ���� ��ġ
                    Vector3 skinnedLocalPosition = boneLocal.GetMatrix() * localPosition;

                    // ���� �������� �ٽ� ����
                    Vector3 skinnedWorldPosition = bindPose.GetMatrix() * skinnedLocalPosition;

                    // ����ġ�� ���ؼ� ������
                    totalPosition += Vector4(skinnedWorldPosition * w.Values[wi], true);
                }
            }

            vertices[vi].Position = totalPosition;
        }

        if (InMesh.HasColor()) {
            vertices[vi].Color = InMesh.GetColors()[vi];
        }

        if (InMesh.HasUV()) {
            vertices[vi].UV = InMesh.GetUVs()[vi];
        }
    }

    // ���� ��ȯ ����
    VertexShader3D(vertices, InMatrix);

    // �׸����� ����
    FillMode fm = FillMode::None;
    if (InMesh.HasColor()) {
        fm |= FillMode::Color;
    }
    if (InMesh.HasUV()) {
        fm |= FillMode::Texture;
    }

    // �ﰢ�� ���� �׸���
    for (int ti = 0; ti < triangleCount; ++ti) {
        int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
        std::vector<Vertex3D> tvs = {vertices[indice[bi0]], vertices[indice[bi1]], vertices[indice[bi2]]};

        // ������ǥ�迡�� Ŭ������ ���� ����
        std::vector<PerspectiveTest> testPlanes = {
            {TestFuncW0, EdgeFuncW0},
            {TestFuncNY, EdgeFuncNY},
            {TestFuncPY, EdgeFuncPY},
            {TestFuncNX, EdgeFuncNX},
            {TestFuncPX, EdgeFuncPX},
            {TestFuncFar, EdgeFuncFar},
            {TestFuncNear, EdgeFuncNear}
        };

        // ������ǥ�迡�� Ŭ���� ����
        for (auto& p : testPlanes) {
            p.ClipTriangles(tvs);
        }

        size_t triangles = tvs.size() / 3;
        for (size_t ti = 0; ti < triangles; ++ti) {
            size_t si = ti * 3;
            std::vector<Vertex3D> sub(tvs.begin() + si, tvs.begin() + si + 3);
            DrawTriangle3D(sub, InColor, fm);
        }
    }
}

void SoftRenderer::DrawTriangle3D(std::vector<Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode) {
    auto& r = GetRenderer();
    const GameEngine& g = Get3DGameEngine();

    for (auto& v : InVertices) {
        // ���� ������ ���, �ణ �������ش�.
        if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

        float invW = 1.f / v.Position.W;
        v.Position.X *= invW;
        v.Position.Y *= invW;
        v.Position.Z *= invW;
    }

    // �����̽� �ø� ( �޸��̸� �׸��� ���� )
    Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
    Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
    float z = edge1.Cross(edge2).Z;
    if (z <= 0.f) {
        return;
    }

    if (IsWireframeDrawing()) {
        for (auto& v : InVertices) {
            v.Position.X *= _ScreenSize.X * 0.5f;
            v.Position.Y *= _ScreenSize.Y * 0.5f;
        }

        LinearColor finalColor = _WireframeColor;
        if (InColor != LinearColor::Error) {
            finalColor = InColor;
        }

        r.DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
        r.DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
        r.DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
    }
    else {
        // �ﰢ�� ĥ�ϱ�
        // �ﰢ���� ���� ����
        Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X),
                       Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
        Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X),
                       Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

        // �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
        Vector2 u = InVertices[1].Position.ToVector2() - InVertices[0].Position.ToVector2();
        Vector2 v = InVertices[2].Position.ToVector2() - InVertices[0].Position.ToVector2();

        // ���� �и� �� ( uu * vv - uv * uv )
        float udotv = u.Dot(v);
        float vdotv = v.Dot(v);
        float udotu = u.Dot(u);
        float denominator = udotv * udotv - vdotv * udotu;

        // ��ȭ �ﰢ�� ����.
        if (Math::EqualsInTolerance(denominator, 0.f)) {
            return;
        }

        float invDenominator = 1.f / denominator;

        // ȭ����� �� ���ϱ�
        minPos.X *= _ScreenSize.X * 0.5f;
        minPos.Y *= _ScreenSize.Y * 0.5f;
        maxPos.X *= _ScreenSize.X * 0.5f;
        maxPos.Y *= _ScreenSize.Y * 0.5f;

        ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
        ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

        // �� ���� ȭ�� ���� ����� ��� Ŭ���� ó��
        lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
        lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
        upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
        upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

        // �ﰢ�� ���� �� ��� ���� �����ϰ� ��ĥ
        for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x) {
            for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y) {
                ScreenPoint fragment = ScreenPoint(x, y);
                Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
                pointToTest.X *= (2.f / _ScreenSize.X);
                pointToTest.Y *= (2.f / _ScreenSize.Y);
                Vector2 w = pointToTest - InVertices[0].Position.ToVector2();
                float wdotu = w.Dot(u);
                float wdotv = w.Dot(v);

                float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
                float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
                float oneMinusST = 1.f - s - t;
                if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <=
                    1.f))) {
                    // �� ������ ������ �� ������ z��
                    float invZ0 = 1.f / InVertices[0].Position.W;
                    float invZ1 = 1.f / InVertices[1].Position.W;
                    float invZ2 = 1.f / InVertices[2].Position.W;

                    // ���� ���������� ����� ���� �и�
                    float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
                    float invZ = 1.f / z;

                    float n = g.GetMainCamera().GetNearZ();
                    float f = g.GetMainCamera().GetFarZ();
                    float newDepth = (InVertices[0].Position.Z * oneMinusST * invZ0 + InVertices[1].Position.Z * s *
                        invZ1 + InVertices[2].Position.Z * t * invZ2) * invZ;
                    float prevDepth = r.GetDepthBufferValue(fragment);
                    if (newDepth < prevDepth) {
                        r.SetDepthBufferValue(fragment, newDepth);
                    }
                    else {
                        // �̹� �տ� ���� �׷��������Ƿ� �ȼ��׸���� ����
                        continue;
                    }

                    if (IsDepthBufferDrawing()) {
                        // �ð�ȭ�� ���� ����ȭ�� ��� ��
                        float grayScale = (invZ - n) / (f - n);

                        // ���� ���� �׸���
                        r.DrawPoint(fragment, LinearColor::White * grayScale);
                    }
                    else {
                        // ���� �Ķ���Ͱ� ���� �ȵ� ��쿡�� ����� ���
                        LinearColor paramColor = LinearColor::White;
                        if (InColor != LinearColor::Error) {
                            paramColor = InColor;
                        }

                        // ���ؽ� �÷� �Ǵ� �ؽ��� �������� ���� ������ ����
                        LinearColor fragmentColor = LinearColor::White;
                        if (InFillMode & FillMode::Color) {
                            fragmentColor = (InVertices[0].Color * oneMinusST * invZ0 + InVertices[1].Color * s * invZ1
                                + InVertices[2].Color * t * invZ2) * invZ;
                        }

                        if (InFillMode & FillMode::Texture) {
                            // ���������������� ������ �ش� �ȼ��� UV ��
                            Vector2 targetUV = (InVertices[0].UV * oneMinusST * invZ0 + InVertices[1].UV * s * invZ1 +
                                InVertices[2].UV * t * invZ2) * invZ;

                            // �ؽ��� ���� ����
                            LinearColor textureColor = g.GetTexture(GameEngine::DiffuseTexture).GetSample(targetUV);
                            fragmentColor = fragmentColor * textureColor;
                        }

                        r.DrawPoint(fragment, FragmentShader3D(fragmentColor, paramColor));
                    }
                }
            }
        }
    }
}