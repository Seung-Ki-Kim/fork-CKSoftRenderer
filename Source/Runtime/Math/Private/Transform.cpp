#include "Precompiled.h"
using namespace CK;

Transform::Transform(const Matrix4x4& InMatrix) {
    // ������ ȸ�� ��ĸ� �и�
    Matrix3x3 rotScaleMatrix = InMatrix.ToMatrix3x3();
    Position = InMatrix[3].ToVector3();

    // ũ�� ��ĺ��� ���Ѵ�. 
    Scale = Vector3::Zero;
    const float squareSumX = rotScaleMatrix[0].SizeSquared();
    const float squareSumY = rotScaleMatrix[1].SizeSquared();
    const float squareSumZ = rotScaleMatrix[2].SizeSquared();
    if (squareSumX > SMALL_NUMBER) { Scale.X = sqrtf(squareSumX); }
    if (squareSumY > SMALL_NUMBER) { Scale.Y = sqrtf(squareSumY); }
    if (squareSumZ > SMALL_NUMBER) { Scale.Z = sqrtf(squareSumZ); }

    // ũ�� ��Ҹ� ���� ���� ���� ����� ���Ѵ�.
    rotScaleMatrix[0] /= squareSumX;
    rotScaleMatrix[1] /= squareSumY;
    rotScaleMatrix[2] /= squareSumZ;

    // ������� ��ȯ�Ѵ�.
    Rotation = Quaternion(rotScaleMatrix);
}