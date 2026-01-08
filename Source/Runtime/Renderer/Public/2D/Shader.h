#pragma once

namespace CK {
    namespace DD {
        // ���� ��ȯ �ڵ�
FORCEINLINE void VertexShader2D(std::vector<Vertex2D>& InVertices, const Matrix3x3& InMatrix) {
            // ��ġ ���� ���� ����� ������ ��ȯ
            for (Vertex2D& v : InVertices) {
                v.Position = InMatrix * v.Position;
            }
        }

        // �ȼ� ��ȯ �ڵ�
FORCEINLINE LinearColor FragmentShader2D(LinearColor& InColor, const LinearColor& InColorParam) {
            return InColor * InColorParam;
        }
    }
}