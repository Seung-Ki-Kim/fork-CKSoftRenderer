#pragma once

namespace CK {
    namespace DDD {
        struct PerspectiveTest {
            std::function<bool(const Vertex3D & InVertex)> ClippingTestFunc;
            std::function<Vertex3D(const Vertex3D & InStartVertex, const Vertex3D & InEndVertex)> GetEdgeVertexFunc;
            std::array<bool, 3> TestResult;

            void ClipTriangles(std::vector<Vertex3D>& InTriangleVertices) {
                size_t nTriangles = InTriangleVertices.size() / 3;
                for (size_t ti = 0; ti < nTriangles; ++ti) {
                    size_t si = ti * 3;
                    size_t testNotPassedCount = 0;

                    std::vector<Vertex3D> sub(InTriangleVertices.begin() + si, InTriangleVertices.begin() + si + 3);
                    // �׽�Ʈ�� ������ �� ���� ���
                    for (size_t ix = 0; ix < 3; ++ix) {
                        bool testResult = ClippingTestFunc(sub[ix]);
                        TestResult[ix] = testResult;
                        if (testResult) testNotPassedCount++;
                    }

                    GetNewVertices(sub, testNotPassedCount);

                    if (testNotPassedCount == 0) {
                        continue;
                    }
                    else if (testNotPassedCount == 1) // �ﰢ�� �߰�
                    {
                        InTriangleVertices[si] = sub[0];
                        InTriangleVertices[si + 1] = sub[1];
                        InTriangleVertices[si + 2] = sub[2];
                        InTriangleVertices.push_back(sub[3]);
                        InTriangleVertices.push_back(sub[4]);
                        InTriangleVertices.push_back(sub[5]);
                    }
                    else if (testNotPassedCount == 2) // �ﰢ�� ���� ����
                    {
                        InTriangleVertices[si] = sub[0];
                        InTriangleVertices[si + 1] = sub[1];
                        InTriangleVertices[si + 2] = sub[2];
                    }
                    else // �ﰢ���� ��Ͽ��� ����
                    {
                        InTriangleVertices.erase(InTriangleVertices.begin() + si, InTriangleVertices.begin() + si + 3);
                        nTriangles--;
                        ti--;
                    }
                }
            }

            // �н��ϸ� false, ����ҰŸ� true
            bool GetNewVertices(std::vector<Vertex3D>& InVertices, size_t NonPassCount) {
                if (NonPassCount == 0) // �״�� ���
                {
                    return true;
                }
                else if (NonPassCount == 1) {
                    // Edge�� ���� �� Ŭ���� ����. ���� �� ���� �߰��ǰ� �ﰢ���� 2���� �ɰ���
                    BYTE index = 0; // �׽�Ʈ�� �ɸ� ���� �ε���
                    if (!TestResult[0]) {
                        index = TestResult[1] ? 1 : 2;
                    }
                    Vertex3D v1 = InVertices[(index + 1) % 3];
                    Vertex3D v2 = InVertices[(index + 2) % 3];
                    Vertex3D clipped1 = GetEdgeVertexFunc(InVertices[index], v1);
                    Vertex3D clipped2 = GetEdgeVertexFunc(InVertices[index], v2);
                    InVertices[0] = clipped1;
                    InVertices[1] = v1;
                    InVertices[2] = v2;
                    InVertices.push_back(clipped1);
                    InVertices.push_back(v2);
                    InVertices.push_back(clipped2);
                    return true;
                }
                else if (NonPassCount == 2) {
                    // Edge�� ���� �� Ŭ���� ����. ���� �� ���� ����ǰ� �ﰢ���� �״��.
                    BYTE index = 0; // �׽�Ʈ�� �ɸ��� ���� ���� �ε���
                    if (TestResult[0]) {
                        index = !TestResult[1] ? 1 : 2;
                    }

                    Vertex3D v1 = InVertices[(index + 1) % 3];
                    Vertex3D v2 = InVertices[(index + 2) % 3];
                    Vertex3D clipped1 = GetEdgeVertexFunc(InVertices[index], v1);
                    Vertex3D clipped2 = GetEdgeVertexFunc(InVertices[index], v2);
                    InVertices[0] = InVertices[index];
                    InVertices[1] = clipped1;
                    InVertices[2] = clipped2;
                    return true;
                }
                else {
                    return false;
                }
            }
        };

        static auto TestFuncW0 = [](const Vertex3D& InVertex) {
            return InVertex.Position.W < 0.f;
        };

        static auto EdgeFuncW0 = [](const Vertex3D& InStartVertex, const Vertex3D& InEndVertex) {
            float p1 = InStartVertex.Position.W;
            float p2 = InEndVertex.Position.W;
            float t = p1 / (p1 - p2);
            return InStartVertex * (1.f - t) + InEndVertex * t;
        };

        static auto TestFuncNY = [](const Vertex3D& InVertex) {
            return InVertex.Position.Y < -InVertex.Position.W;
        };

        static auto EdgeFuncNY = [](const Vertex3D& InStartVertex, const Vertex3D& InEndVertex) {
            float p1 = InStartVertex.Position.W + InStartVertex.Position.Y;
            float p2 = InEndVertex.Position.W + InEndVertex.Position.Y;
            float t = p1 / (p1 - p2);
            return InStartVertex * (1.f - t) + InEndVertex * t;
        };

        static auto TestFuncPY = [](const Vertex3D& InVertex) {
            return InVertex.Position.Y > InVertex.Position.W;
        };

        static auto EdgeFuncPY = [](const Vertex3D& InStartVertex, const Vertex3D& InEndVertex) {
            float p1 = InStartVertex.Position.W - InStartVertex.Position.Y;
            float p2 = InEndVertex.Position.W - InEndVertex.Position.Y;
            float t = p1 / (p1 - p2);
            return InStartVertex * (1.f - t) + InEndVertex * t;
        };

        static auto TestFuncNX = [](const Vertex3D& InVertex) {
            return InVertex.Position.X < -InVertex.Position.W;
        };

        static auto EdgeFuncNX = [](const Vertex3D& InStartVertex, const Vertex3D& InEndVertex) {
            float p1 = InStartVertex.Position.W + InStartVertex.Position.X;
            float p2 = InEndVertex.Position.W + InEndVertex.Position.X;
            float t = p1 / (p1 - p2);
            return InStartVertex * (1.f - t) + InEndVertex * t;
        };

        static auto TestFuncPX = [](const Vertex3D& InVertex) {
            return InVertex.Position.X > InVertex.Position.W;
        };

        static auto EdgeFuncPX = [](const Vertex3D& InStartVertex, const Vertex3D& InEndVertex) {
            float p1 = InStartVertex.Position.W - InStartVertex.Position.X;
            float p2 = InEndVertex.Position.W - InEndVertex.Position.X;
            float t = p1 / (p1 - p2);
            return InStartVertex * (1.f - t) + InEndVertex * t;
        };

        static auto TestFuncFar = [](const Vertex3D& InVertex) {
            return InVertex.Position.Z > InVertex.Position.W;
        };

        static auto EdgeFuncFar = [](const Vertex3D& InStartVertex, const Vertex3D& InEndVertex) {
            float p1 = InStartVertex.Position.W - InStartVertex.Position.Z;
            float p2 = InEndVertex.Position.W - InEndVertex.Position.Z;
            float t = p1 / (p1 - p2);
            return InStartVertex * (1.f - t) + InEndVertex * t;
        };

        static auto TestFuncNear = [](const Vertex3D& InVertex) {
            return InVertex.Position.Z < -InVertex.Position.W;
        };

        static auto EdgeFuncNear = [](const Vertex3D& InStartVertex, const Vertex3D& InEndVertex) {
            float p1 = InStartVertex.Position.W + InStartVertex.Position.Z;
            float p2 = InEndVertex.Position.W + InEndVertex.Position.Z;
            float t = p1 / (p1 - p2);
            return InStartVertex * (1.f - t) + InEndVertex * t;
        };
    }
}