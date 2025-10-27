#pragma once

#include <cstdint>

namespace Match3
{
    /**
     * @brief 宝石类型枚举
     */
    enum class GemType : uint8_t
    {
        Red = 0,
        Green = 1,
        Blue = 2,
        Yellow = 3,
        Purple = 4,
        Cyan = 5,
        Empty = 255 // 空位
    };

    /**
     * @brief 宝石状态枚举
     */
    enum class GemState : uint8_t
    {
        Idle, // 闲置
        Selected, // 被选中
        Swapping, // 交换中
        Falling, // 下落中
        Matched, // 已匹配（待消除）
        Eliminating // 消除中
    };

    /**
     * @brief 宝石类 - 表示单个宝石的数据和状态
     */
    class Gem
    {
    public:
        /**
         * @brief 构造函数
         * @param row 行位置
         * @param col 列位置
         * @param type 宝石类型
         */
        Gem(int row = 0, int col = 0, GemType type = GemType::Empty);

        // Getters
        [[nodiscard]] int GetRow() const { return m_row; }
        [[nodiscard]] int GetCol() const { return m_col; }
        [[nodiscard]] GemType GetType() const { return m_type; }
        [[nodiscard]] GemState GetState() const { return m_state; }
        [[nodiscard]] bool IsEmpty() const { return m_type == GemType::Empty; }
        [[nodiscard]] bool IsMatched() const { return m_state == GemState::Matched; }

        // Setters
        void SetRow(int row) { m_row = row; }
        void SetCol(int col) { m_col = col; }
        void SetType(GemType type) { m_type = type; }
        void SetState(GemState state) { m_state = state; }

        void SetPosition(int row, int col)
        {
            m_row = row;
            m_col = col;
        }

        /**
         * @brief 重置宝石到空状态
         */
        void Reset();

        /**
         * @brief 生成随机类型的宝石
         * @param maxType 最大类型值（不包含）
         */
        void RandomizeType(int maxType);

    private:
        int m_row; // 行位置（0-based）
        int m_col; // 列位置（0-based）
        GemType m_type; // 宝石类型
        GemState m_state; // 宝石状态
    };
} // namespace Match3
