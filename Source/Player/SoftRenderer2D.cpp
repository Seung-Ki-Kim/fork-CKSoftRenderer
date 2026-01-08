
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// 그리드 그리기
void SoftRenderer::DrawGrid2D() {
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)
		_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix) {
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy) {
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	// 월드의 원점
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// 실습을 위한 변수
Vector2 deltaPosition;
Vector2 currentPosition(100.f, 100.f);

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds) {
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	static float moveSpeed = 100.f;

	Vector2 inputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).Normalize();
	Vector2 deltaPosition = inputVector * moveSpeed * InDeltaSeconds;

	currentPosition += deltaPosition;
}

// 렌더링 로직
void SoftRenderer::Render2D() {
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 격자 그리기
	DrawGrid2D();

	// 변수 선언
	static float radius = 50.f;
	static std::vector<Vector2> circles;

	// 원 그리기
	if (circles.empty()) {
		for (float x = -radius; x <= radius; ++x) {
			for (float y = -radius; y <= radius; ++y) {
				Vector2 pointToTest = Vector2(x, y);
				float squaredLength = pointToTest.SizeSquared();

				if (squaredLength <= radius * radius) {
					circles.push_back(pointToTest);
				}
			}
		}
	}

	for (auto const& v : circles) {
		r.DrawPoint(v + currentPosition, LinearColor::Red);
	}

	// 좌표 출력
	r.PushStatisticText("Coordinate : " + currentPosition.ToString());
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor) {
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor,
                                  FillMode InFillMode) {
}
