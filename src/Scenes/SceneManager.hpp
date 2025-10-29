#pragma once

#include <memory>
#include <stack>
#include <unordered_map>
#include <string>

namespace Match3
{
    class Scene;
    class Renderer;

    /**
     * @brief 场景管理器 - 管理场景栈和场景切换
     * 
     * 支持场景栈，可以将场景推入栈中（如暂停菜单覆盖游戏场景）
     * 或完全切换场景（如从主菜单进入游戏）
     */
    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager();

        // 禁止拷贝
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        /**
         * @brief 推入新场景到栈顶
         * 当前场景会被暂停，新场景会被激活
         * @param scene 要推入的场景
         */
        void PushScene(std::unique_ptr<Scene> scene);

        /**
         * @brief 弹出栈顶场景
         * 栈顶场景会被销毁，下一个场景会被恢复
         */
        void PopScene();

        /**
         * @brief 更换场景（清空栈并推入新场景）
         * 所有现有场景会被销毁
         * @param scene 新场景
         */
        void ChangeScene(std::unique_ptr<Scene> scene);

        /**
         * @brief 更新当前活动场景
         * @param deltaTime 帧间隔时间（秒）
         */
        void Update(float deltaTime);

        /**
         * @brief 渲染所有可见场景
         * 从底到顶渲染所有场景
         */
        void Render();

        /**
         * @brief 处理鼠标点击事件
         * 事件会传递给栈顶场景
         * @param x 鼠标 X 坐标
         * @param y 鼠标 Y 坐标
         * @return 如果事件被处理返回 true
         */
        bool HandleMouseClick(int x, int y);

        /**
         * @brief 处理鼠标移动事件
         * @param x 鼠标 X 坐标
         * @param y 鼠标 Y 坐标
         * @return 如果事件被处理返回 true
         */
        bool HandleMouseMove(int x, int y);

        /**
         * @brief 处理鼠标按下事件
         * @param x 鼠标 X 坐标
         * @param y 鼠标 Y 坐标
         * @return 如果事件被处理返回 true
         */
        bool HandleMouseDown(int x, int y);

        /**
         * @brief 处理鼠标释放事件
         * @param x 鼠标 X 坐标
         * @param y 鼠标 Y 坐标
         * @return 如果事件被处理返回 true
         */
        bool HandleMouseUp(int x, int y);

        /**
         * @brief 处理键盘事件
         * 事件会传递给栈顶场景
         * @param key SDL 键码
         * @return 如果事件被处理返回 true
         */
        bool HandleKeyPress(int key);

        /**
         * @brief 检查场景栈是否为空
         */
        [[nodiscard]] bool IsEmpty() const { return m_sceneStack.empty(); }

        /**
         * @brief 获取当前活动场景
         */
        [[nodiscard]] Scene* GetCurrentScene() const;

        /**
         * @brief 通知当前场景窗口大小已改变
         * @param width 新的窗口宽度
         * @param height 新的窗口高度
         */
        void NotifyWindowResize(int width, int height);

    private:
        std::stack<std::unique_ptr<Scene>> m_sceneStack;
    };
} // namespace Match3
