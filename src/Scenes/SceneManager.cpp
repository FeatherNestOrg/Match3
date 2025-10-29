#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Core/Logger.hpp"

namespace Match3
{
    SceneManager::SceneManager() = default;

    SceneManager::~SceneManager()
    {
        // 清空所有场景
        while (!m_sceneStack.empty())
        {
            m_sceneStack.top()->OnExit();
            m_sceneStack.pop();
        }
    }

    void SceneManager::PushScene(std::unique_ptr<Scene> scene)
    {
        if (!scene)
        {
            LOG_ERROR("SceneManager::PushScene - Trying to push null scene");
            return;
        }

        LOG_INFO("SceneManager: Pushing scene '{}'", scene->GetName());

        // 暂停当前场景（如果有）
        if (!m_sceneStack.empty())
        {
            m_sceneStack.top()->OnPause();
        }

        // 推入新场景并初始化
        scene->OnEnter();
        m_sceneStack.push(std::move(scene));
    }

    void SceneManager::PopScene()
    {
        if (m_sceneStack.empty())
        {
            LOG_WARN("SceneManager::PopScene - No scene to pop");
            return;
        }

        LOG_INFO("SceneManager: Popping scene '{}'", m_sceneStack.top()->GetName());

        // 退出当前场景
        m_sceneStack.top()->OnExit();
        m_sceneStack.pop();

        // 恢复下一个场景（如果有）
        if (!m_sceneStack.empty())
        {
            m_sceneStack.top()->OnResume();
        }
    }

    void SceneManager::ChangeScene(std::unique_ptr<Scene> scene)
    {
        if (!scene)
        {
            LOG_ERROR("SceneManager::ChangeScene - Trying to change to null scene");
            return;
        }

        LOG_INFO("SceneManager: Changing scene to '{}'", scene->GetName());

        // 退出并清空所有场景
        while (!m_sceneStack.empty())
        {
            m_sceneStack.top()->OnExit();
            m_sceneStack.pop();
        }

        // 推入新场景并初始化
        scene->OnEnter();
        m_sceneStack.push(std::move(scene));
    }

    void SceneManager::Update(float deltaTime)
    {
        if (!m_sceneStack.empty())
        {
            m_sceneStack.top()->Update(deltaTime);
        }
    }

    void SceneManager::Render()
    {
        if (!m_sceneStack.empty())
        {
            // 目前只渲染栈顶场景
            // 未来可以支持半透明场景，渲染多个场景
            m_sceneStack.top()->Render();
        }
    }

    bool SceneManager::HandleMouseClick(int x, int y)
    {
        if (!m_sceneStack.empty())
        {
            return m_sceneStack.top()->HandleMouseClick(x, y);
        }
        return false;
    }

    bool SceneManager::HandleMouseMove(int x, int y)
    {
        if (!m_sceneStack.empty())
        {
            return m_sceneStack.top()->HandleMouseMove(x, y);
        }
        return false;
    }

    bool SceneManager::HandleMouseDown(int x, int y)
    {
        if (!m_sceneStack.empty())
        {
            return m_sceneStack.top()->HandleMouseDown(x, y);
        }
        return false;
    }

    bool SceneManager::HandleMouseUp(int x, int y)
    {
        if (!m_sceneStack.empty())
        {
            return m_sceneStack.top()->HandleMouseUp(x, y);
        }
        return false;
    }

    bool SceneManager::HandleKeyPress(int key)
    {
        if (!m_sceneStack.empty())
        {
            return m_sceneStack.top()->HandleKeyPress(key);
        }
        return false;
    }

    Scene* SceneManager::GetCurrentScene() const
    {
        if (!m_sceneStack.empty())
        {
            return m_sceneStack.top().get();
        }
        return nullptr;
    }

    void SceneManager::NotifyWindowResize(int width, int height)
    {
        if (!m_sceneStack.empty())
        {
            m_sceneStack.top()->HandleWindowResize(width, height);
        }
    }

} // namespace Match3
