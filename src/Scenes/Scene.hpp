#pragma once

#include <string>

namespace Match3
{
    class Renderer;
    class UIManager;
    class FontRenderer;

    /**
     * @brief Scene 基类 - 所有场景的基类
     * 
     * 场景代表游戏的不同界面状态（主菜单、游戏中、暂停等）
     */
    class Scene
    {
    public:
        virtual ~Scene() = default;

        /**
         * @brief 场景初始化
         * 在场景第一次创建时调用
         */
        virtual void OnEnter() = 0;

        /**
         * @brief 场景退出
         * 在场景被销毁或切换到其他场景时调用
         */
        virtual void OnExit() = 0;

        /**
         * @brief 场景暂停
         * 当场景被其他场景覆盖时调用（例如暂停菜单）
         */
        virtual void OnPause() {}

        /**
         * @brief 场景恢复
         * 当覆盖的场景被移除时调用
         */
        virtual void OnResume() {}

        /**
         * @brief 更新场景逻辑
         * @param deltaTime 帧间隔时间（秒）
         */
        virtual void Update(float deltaTime) = 0;

        /**
         * @brief 渲染场景
         */
        virtual void Render() = 0;

        /**
         * @brief 处理鼠标点击事件
         * @param x 鼠标 X 坐标
         * @param y 鼠标 Y 坐标
         * @return 如果事件被处理返回 true
         */
        virtual bool HandleMouseClick(int x, int y) { return false; }

        /**
         * @brief 处理鼠标移动事件
         * @param x 鼠标 X 坐标
         * @param y 鼠标 Y 坐标
         * @return 如果事件被处理返回 true
         */
        virtual bool HandleMouseMove(int x, int y) { return false; }

        /**
         * @brief 处理鼠标按下事件
         * @param x 鼠标 X 坐标
         * @param y 鼠标 Y 坐标
         * @return 如果事件被处理返回 true
         */
        virtual bool HandleMouseDown(int x, int y) { return false; }

        /**
         * @brief 处理鼠标释放事件
         * @param x 鼠标 X 坐标
         * @param y 鼠标 Y 坐标
         * @return 如果事件被处理返回 true
         */
        virtual bool HandleMouseUp(int x, int y) { return false; }

        /**
         * @brief 处理键盘事件
         * @param key SDL 键码
         * @return 如果事件被处理返回 true
         */
        virtual bool HandleKeyPress(int key) { return false; }

        /**
         * @brief 获取场景名称
         */
        [[nodiscard]] virtual std::string GetName() const = 0;

        /**
         * @brief 处理窗口大小变化
         * @param width 新的窗口宽度
         * @param height 新的窗口高度
         */
        virtual void HandleWindowResize(int width, int height) {}

    protected:
        Scene() = default;
    };
} // namespace Match3
