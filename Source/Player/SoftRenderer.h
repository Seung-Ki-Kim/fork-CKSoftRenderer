#pragma once

enum class GameEngineType : UINT32 {
    DD = 0,
    DDD
};

enum class DrawMode : UINT32 {
    Normal = 0,
    Wireframe,
    DepthBuffer
};

enum class FillMode : UINT32 {
    None = 0x00,
    Color = 0x01,
    Texture = 0x02
};

FORCEINLINE FillMode operator|(FillMode InLhs, FillMode InRhs) {
    return static_cast<FillMode>(
        static_cast<std::underlying_type<FillMode>::type>(InLhs) |
        static_cast<std::underlying_type<FillMode>::type>(InRhs)
    );
}

FORCEINLINE FillMode& operator|=(FillMode& InLhs, FillMode InRhs) {
    InLhs = InLhs | InRhs;
    return InLhs;
}

FORCEINLINE bool operator&(FillMode InLhs, FillMode InRhs) {
    BYTE rhsValue = static_cast<std::underlying_type<FillMode>::type>(InRhs);
    return ((static_cast<std::underlying_type<FillMode>::type>(InLhs) & rhsValue) == rhsValue);
}

class SoftRenderer {
public:
    // ������
    SoftRenderer(GameEngineType InGameEngineType, RendererInterface* InRSI);

    // ������ �̺�Ʈ ó��
    void OnTick();
    void OnResize(const ScreenPoint& InNewScreenSize);
    void OnShutdown();

    // ���α׷� �⺻ ����
    SystemInputManager& GetSystemInput() { return _SystemInputManager; }
    const ScreenPoint& GetScreenSize() { return _ScreenSize; }
    float GetFrameFPS() const { return _FrameFPS; }
	FORCEINLINE float GetElapsedTime() const { return _ElapsedTime; }

    // ���� ����
    std::function<float()> _PerformanceInitFunc;
    std::function<INT64()> _PerformanceMeasureFunc;
    std::function<void(InputManager &)> _InputBindingFunc;

    // ���� ���� ���۷��� 
	FORCEINLINE EngineInterface& GetGameEngine() {
        return (_GameEngineType == GameEngineType::DD)
                   ? static_cast<EngineInterface&>(_GameEngine2)
                   : static_cast<EngineInterface&>(_GameEngine3);
    }

    FORCEINLINE DD::GameEngine& Get2DGameEngine() { return _GameEngine2; }
    FORCEINLINE DDD::GameEngine& Get3DGameEngine() { return _GameEngine3; }

private:
    // ���� ���� ����
    void SetDefaultGameEngine(GameEngineType InGameEngineType);

    // �⺻ ���� �Լ�
    void PreUpdate();
    void PostUpdate();

    // ������ ���۷���
	FORCEINLINE RendererInterface& GetRenderer() { return *_RSIPtr.get(); }
	FORCEINLINE void SetBackgroundColor(const LinearColor& InLinearColor) { _BackgroundColor = InLinearColor; }
	FORCEINLINE void SetWireframeColor(const LinearColor& InLinearColor) { _WireframeColor = InLinearColor; }

    // 2D �׷��Ƚ� ����
    void Update2D(float InDeltaSeconds);
    void Render2D();
    void DrawGrid2D();
    void DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor);
    void DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode);

    int _Grid2DUnit = 10;

    // 3D �׷��Ƚ� ����
    void DrawGizmo3D();
    void Update3D(float InDeltaSeconds);
    void LateUpdate3D(float InDeltaSeconds);
    void Render3D();

    void DrawMesh3D(const class DDD::Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor);
    void DrawTriangle3D(std::vector<DDD::Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode);
    bool IsDepthBufferDrawing() const { return _CurrentDrawMode == DrawMode::DepthBuffer; }
    bool IsWireframeDrawing() const { return _CurrentDrawMode == DrawMode::Wireframe; }
    DrawMode GetDrawMode() const { return _CurrentDrawMode; }
    void SetDrawMode(DrawMode InDrawMode) { _CurrentDrawMode = InDrawMode; }

    float _GizmoUnitLength = 50.f;
    Vector2 _GizmoPositionOffset = Vector2(-320.f, -250.f);
    DrawMode _CurrentDrawMode = DrawMode::Normal;

private:
    // �ʱ�ȭ ���� ����
    bool _PerformanceCheckInitialized = false;
    bool _RendererInitialized = false;
    bool _GameEngineInitialized = false;
    bool _TickEnabled = false;
    bool _AllInitialized = false;

    // ȭ�� ũ��
    ScreenPoint _ScreenSize;

    // ��� ����
    LinearColor _BackgroundColor = LinearColor::WhiteSmoke;

    // ���̾������� ����
    LinearColor _WireframeColor = LinearColor::DimGray;

    // ���� ����
    long long _StartTimeStamp = 0;
    long long _FrameTimeStamp = 0;
    long _FrameCount = 0;
    float _CyclesPerMilliSeconds = 0.f;
    float _FrameTime = 0.f;
    float _ElapsedTime = 0.f;
    float _AverageFPS = 0.f;
    float _FrameFPS = 0.f;

    // ������ �������̽�
    std::unique_ptr<RendererInterface> _RSIPtr;
    GameEngineType _GameEngineType = GameEngineType::DD;

    // ���� ����
    DD::GameEngine _GameEngine2;
    DDD::GameEngine _GameEngine3;

    // ���� ���α׷� �Է�
    SystemInputManager _SystemInputManager;
};