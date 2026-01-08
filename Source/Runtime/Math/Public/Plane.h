#pragma once

namespace CK {
    struct Plane {
    public:
        FORCEINLINE constexpr Plane() = default;

        FORCEINLINE explicit constexpr
        Plane(const Vector3& InNormal, float InDistance) : Normal(InNormal), D(InDistance) {
            assert(InNormal.SizeSquared() == 1);
        } // ����ȭ�� ��ҷ� ��� �����ϱ�
        FORCEINLINE explicit constexpr Plane(const Vector3& InNormal, Vector3 InPlanePoint); // ����ȭ�� ��ҷ� ��� �����ϱ�
        explicit Plane(const Vector3& InPoint1, const Vector3& InPoint2, const Vector3& InPoint3);
        explicit Plane(const Vector4& InVector4); // ����ȭ�ȵ� �� ���� ��ҷ� ��� �����ϱ�

	FORCEINLINE constexpr float Distance(const Vector3& InPoint) const;
	FORCEINLINE constexpr bool IsOutside(const Vector3& InPoint) const;

    private:
        void Normalize();

    public:
        Vector3 Normal = Vector3::UnitY;
        float D = 0.f;
    };

    FORCEINLINE constexpr Plane::Plane(const Vector3& InNormal, Vector3 InPlanePoint) : Normal(InNormal) {
        assert(InNormal.SizeSquared() == 1);
        D = -Normal.Dot(InPlanePoint);
    }

FORCEINLINE constexpr float Plane::Distance(const Vector3& InPoint) const {
        return Normal.Dot(InPoint) + D;
    }

FORCEINLINE constexpr bool Plane::IsOutside(const Vector3& InPoint) const {
        return Distance(InPoint) > 0.f;
    }
}