#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(GameEngineType InGameEngineType, RendererInterface* InRSI) : _GameEngineType(
    InGameEngineType), _RSIPtr(InRSI) {
}

void SoftRenderer::OnTick() {
    if (!_AllInitialized) {
        // �����ս� ī���� �ʱ�ȭ.
        if (!_PerformanceCheckInitialized) {
            if (_PerformanceInitFunc && _PerformanceMeasureFunc) {
                _CyclesPerMilliSeconds = _PerformanceInitFunc();
                _PerformanceCheckInitialized = true;
            }
            else {
                assert(false);
                return;
            }
        }

        // ��ũ�� ũ�� Ȯ��
        if (_ScreenSize.HasZero()) {
            return;
        }

        // ����Ʈ ������ �ʱ�ȭ.
        if (!_RendererInitialized) {
            _RendererInitialized = GetRenderer().Init(_ScreenSize);
            if (!_RendererInitialized) {
                assert(false);
                return;
            }
        }

        // ���� ���� �ʱ�ȭ
        _GameEngineInitialized = GetGameEngine().IsInitialized();
        if (!_GameEngineInitialized) {
            GetGameEngine().OnScreenResize(_ScreenSize);
            InputManager& input = GetGameEngine().GetInputManager();
            if (!input.IsInputReady()) {
                _InputBindingFunc(input);
            }

            _GameEngineInitialized = GetGameEngine().Init();
            if (!_GameEngineInitialized) {
                assert(false);
                return;
            }
        }

        _AllInitialized = _RendererInitialized && _PerformanceCheckInitialized && _GameEngineInitialized;
        if (_AllInitialized) {
            _TickEnabled = true;
        }
    }
    else {
        assert(_RSIPtr != nullptr && GetRenderer().IsInitialized() && !_ScreenSize.HasZero());

        if (_TickEnabled) {
            PreUpdate();

            // ���� ���� ��ü�� �Լ� ���� ����
            if (!_AllInitialized) {
                GetSystemInput().UpdateSystemInput();
                return;
            }

            if (_GameEngineType == GameEngineType::DD) {
                Update2D(_FrameTime / 1000.f);
                Render2D();
            }
            else {
                Update3D(_FrameTime / 1000.f);
                LateUpdate3D(_FrameTime / 1000.f);
                Render3D();
            }

            PostUpdate();
        }
    }
}

void SoftRenderer::OnResize(const ScreenPoint& InNewScreenSize) {
    _ScreenSize = InNewScreenSize;

    // ũ�Ⱑ ����Ǹ� �������� ���� �ʱ�ȭ
    if (_RendererInitialized) {
        GetRenderer().Init(InNewScreenSize);
    }

    if (_GameEngineInitialized) {
        GetGameEngine().OnScreenResize(_ScreenSize);
    }
}

void SoftRenderer::OnShutdown() {
    GetRenderer().Shutdown();
}

void SoftRenderer::SetDefaultGameEngine(GameEngineType InGameEngineType) {
    _GameEngineType = InGameEngineType;
}

void SoftRenderer::PreUpdate() {
    // ���� ���� ����.
    _FrameTimeStamp = _PerformanceMeasureFunc();
    if (_FrameCount == 0) {
        _StartTimeStamp = _FrameTimeStamp;
    }

    // ��� �����.
    GetRenderer().Clear(_BackgroundColor);

    // ���� �ð�ȭ
    const SystemInputManager& sinput = GetSystemInput();

    if (sinput.IsReleased(SystemInputButton::F1)) { _CurrentDrawMode = DrawMode::Normal; }
    if (sinput.IsReleased(SystemInputButton::F2)) { _CurrentDrawMode = DrawMode::Wireframe; }
    if (sinput.IsReleased(SystemInputButton::F3)) { _CurrentDrawMode = DrawMode::DepthBuffer; }
    if (sinput.IsReleased(SystemInputButton::F10)) {
        SetDefaultGameEngine((_GameEngineType == GameEngineType::DD) ? GameEngineType::DDD : GameEngineType::DD);
        _AllInitialized = false;
    }
}

void SoftRenderer::PostUpdate() {
    // ������ ������.
    GetRenderer().EndFrame();

    // �Է� ���� ������Ʈ
    GetGameEngine().GetInputManager().UpdateInput();
    GetSystemInput().UpdateSystemInput();

    // ���� ���� ������.
    _FrameCount++;
    INT64 currentTimeStamp = _PerformanceMeasureFunc();
    INT64 frameCycles = currentTimeStamp - _FrameTimeStamp;
    INT64 elapsedCycles = currentTimeStamp - _StartTimeStamp;
    _FrameTime = frameCycles / _CyclesPerMilliSeconds;
    _ElapsedTime = elapsedCycles / _CyclesPerMilliSeconds;
    _FrameFPS = _FrameTime == 0.f ? 0.f : 1000.f / _FrameTime;
    _AverageFPS = _ElapsedTime == 0.f ? 0.f : 1000.f / _ElapsedTime * _FrameCount;
}