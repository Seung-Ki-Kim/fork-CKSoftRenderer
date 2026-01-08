#include "Precompiled.h"
#include <random>
using namespace CK::DD;

// �޽�
const std::size_t GameEngine::QuadMesh = std::hash<std::string>()("SM_Quad");

// ���� ������Ʈ
const std::string GameEngine::PlayerGo("Player");

// �ؽ���
const std::size_t GameEngine::DiffuseTexture = std::hash<std::string>()("Diffuse");
const std::string GameEngine::SteveTexturePath("Steve.png");

struct GameObjectCompare {
    bool operator()(const std::unique_ptr<GameObject>& lhs, std::size_t rhs) {
        return lhs->GetHash() < rhs;
    }
};

void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize) {
    // ȭ�� ũ���� ����
    _ScreenSize = InScreenSize;
    _MainCamera.SetViewportSize(_ScreenSize);
}

bool GameEngine::Init() {
    // �̹� �ʱ�ȭ�Ǿ� ������ �ʱ�ȭ �������� ����.
    if (_IsInitialized) {
        return true;
    }

    // ȭ�� ũ�Ⱑ �ùٷ� �����Ǿ� �ִ��� Ȯ��
    if (_ScreenSize.HasZero()) {
        return false;
    }

    if (!_InputManager.IsInputReady()) {
        return false;
    }

    if (!LoadResources()) {
        return false;
    }

    if (!LoadScene()) {
        return false;
    }

    _IsInitialized = true;
    return true;
}

bool GameEngine::LoadResources() {
    // �޽� ������ �ε�
    Mesh& quadMesh = CreateMesh(GameEngine::QuadMesh);

    constexpr float squareHalfSize = 0.5f;
    constexpr int vertexCount = 4;
    constexpr int triangleCount = 2;
    constexpr int indexCount = triangleCount * 3;

    auto& v = quadMesh.GetVertices();
    auto& i = quadMesh.GetIndices();
    auto& uv = quadMesh.GetUVs();

    v = {
        Vector2(-squareHalfSize, -squareHalfSize),
        Vector2(-squareHalfSize, squareHalfSize),
        Vector2(squareHalfSize, squareHalfSize),
        Vector2(squareHalfSize, -squareHalfSize)
    };

    uv = {
        Vector2(0.125f, 0.75f),
        Vector2(0.125f, 0.875f),
        Vector2(0.25f, 0.875f),
        Vector2(0.25f, 0.75f)
    };

    i = {
        0, 2, 1, 0, 3, 2
    };

    quadMesh.CalculateBounds();

    // �ؽ��� �ε�
    Texture& diffuseTexture = CreateTexture(GameEngine::DiffuseTexture, GameEngine::SteveTexturePath);
    if (!diffuseTexture.IsIntialized()) {
        return false;
    }

    return true;
}

bool GameEngine::LoadScene() {
    return true;
}


Mesh& GameEngine::CreateMesh(const std::size_t& InKey) {
    auto meshPtr = std::make_unique<Mesh>();
    _Meshes.insert({InKey, std::move(meshPtr)});
    return *_Meshes.at(InKey).get();
}

Texture& GameEngine::CreateTexture(const std::size_t& InKey, const std::string& InTexturePath) {
    auto texturePtr = std::make_unique<Texture>(InTexturePath);
    _Textures.insert({InKey, std::move(texturePtr)});
    return *_Textures.at(InKey).get();
}

GameObject& GameEngine::CreateNewGameObject(const std::string& InName) {
    std::size_t inHash = std::hash<std::string>()(InName);
    const auto it = std::lower_bound(SceneBegin(), SceneEnd(), inHash, GameObjectCompare());

    auto newGameObject = std::make_unique<GameObject>(InName);
    if (it != _Scene.end()) {
        std::size_t targetHash = (*it)->GetHash();
        if (targetHash == inHash) {
            // �ߺ��� Ű �߻�. ����.
            assert(false);
            return GameObject::Invalid;
        }
        else if (targetHash < inHash) {
            _Scene.insert(it + 1, std::move(newGameObject));
        }
        else {
            _Scene.insert(it, std::move(newGameObject));
        }
    }
    else {
        _Scene.push_back(std::move(newGameObject));
    }

    return GetGameObject(InName);
}

GameObject& GameEngine::GetGameObject(const std::string& InName) {
    std::size_t targetHash = std::hash<std::string>()(InName);
    const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash, GameObjectCompare());

    return (it != _Scene.end()) ? *(*it).get() : GameObject::Invalid;
}