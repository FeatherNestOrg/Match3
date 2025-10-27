#pragma once

#include <cstdint>

namespace Match3::Components {

/**
 * @brief 宝石类型枚举
 */
enum class GemType : uint8_t {
    Red = 0,
    Green = 1,
    Blue = 2,
    Yellow = 3,
    Purple = 4,
    Cyan = 5,
    Empty = 255
};

/**
 * @brief 宝石状态枚举
 */
enum class GemState : uint8_t {
    Idle,           // 空闲
    Selected,       // 被选中
    Swapping,       // 交换中
    Falling,        // 下落中
    Matched,        // 已匹配
    Eliminating     // 消除中
};

/**
 * @brief 宝石组件 - 核心数据
 */
struct Gem {
    GemType type = GemType::Empty;
    GemState state = GemState::Idle;
    bool canMatch = true;           // 是否参与匹配
    bool isSpecial = false;         // 是否特殊宝石（预留）
    int matchPriority = 0;          // 匹配优先级（预留）
    
    Gem() = default;
    explicit Gem(GemType type, GemState state = GemState::Idle)
        : type(type), state(state) {}
    
    // 检查是否为空
    [[nodiscard]] bool IsEmpty() const {
        return type == GemType::Empty;
    }
    
    // 检查是否匹配状态
    [[nodiscard]] bool IsMatched() const {
        return state == GemState::Matched;
    }
};

/**
 * @brief 匹配标记组件 - 临时组件，标记参与匹配的宝石
 */
struct Matched {
    int matchId = 0;            // 所属匹配组ID
    int matchSize = 0;          // 匹配数量（3、4、5等）
    
    Matched() = default;
    Matched(int matchId, int matchSize) 
        : matchId(matchId), matchSize(matchSize) {}
};

/**
 * @brief 选中组件 - 临时组件，标记被选中的宝石
 */
struct Selected {
    float pulseFrequency = 2.0f;    // 脉冲频率（Hz）
    float pulseIntensity = 0.3f;    // 脉冲强度（0-1）
    float time = 0.0f;              // 动画时间
    
    Selected() = default;
    Selected(float frequency, float intensity) 
        : pulseFrequency(frequency), pulseIntensity(intensity) {}
};

} // namespace Match3::Components
