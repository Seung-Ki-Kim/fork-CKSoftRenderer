#pragma once

namespace CK {
    namespace DDD {
        // ���� ��ȯ �ڵ�
FORCEINLINE void VertexShader3D(std::vector<Vertex3D>& InVertices, const Matrix4x4& InMatrix) {
            // ��ġ ���� ���� ����� ������ ��ȯ
            for (Vertex3D& v : InVertices) {
                v.Position = InMatrix * v.Position;
            }
        }

        // �ȼ� ��ȯ �ڵ�
FORCEINLINE LinearColor FragmentShader3D(LinearColor& InColor, const LinearColor& InColorParam) {
            return InColor * InColorParam;
        }
    }
}