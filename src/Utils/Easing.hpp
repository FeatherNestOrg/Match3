#pragma once

#include <cmath>
#include <numbers>

namespace Match3
{
    /**
     * @brief 缓动函数集合 - 用于动画插值
     * 
     * 所有函数的输入范围为 [0, 1]，输出范围为 [0, 1]
     * t = 0 表示动画开始，t = 1 表示动画结束
     */
    namespace Easing
    {
        // ========== Linear ==========
        inline constexpr float Linear(float t)
        {
            return t;
        }

        // ========== Quadratic ==========
        inline constexpr float InQuad(float t)
        {
            return t * t;
        }

        inline constexpr float OutQuad(float t)
        {
            return t * (2.0f - t);
        }

        inline constexpr float InOutQuad(float t)
        {
            return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
        }

        // ========== Cubic ==========
        inline constexpr float InCubic(float t)
        {
            return t * t * t;
        }

        inline constexpr float OutCubic(float t)
        {
            const float f = t - 1.0f;
            return f * f * f + 1.0f;
        }

        inline constexpr float InOutCubic(float t)
        {
            return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
        }

        // ========== Quartic ==========
        inline constexpr float InQuart(float t)
        {
            return t * t * t * t;
        }

        inline constexpr float OutQuart(float t)
        {
            const float f = t - 1.0f;
            return 1.0f - f * f * f * f;
        }

        inline constexpr float InOutQuart(float t)
        {
            return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - 8.0f * (t - 1.0f) * (t - 1.0f) * (t - 1.0f) * (t - 1.0f);
        }

        // ========== Quintic ==========
        inline constexpr float InQuint(float t)
        {
            return t * t * t * t * t;
        }

        inline constexpr float OutQuint(float t)
        {
            const float f = t - 1.0f;
            return f * f * f * f * f + 1.0f;
        }

        inline constexpr float InOutQuint(float t)
        {
            return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f + 16.0f * (t - 1.0f) * (t - 1.0f) * (t - 1.0f) * (t - 1.0f) * (t - 1.0f);
        }

        // ========== Sine ==========
        inline float InSine(float t)
        {
            return 1.0f - std::cos(t * std::numbers::pi_v<float> / 2.0f);
        }

        inline float OutSine(float t)
        {
            return std::sin(t * std::numbers::pi_v<float> / 2.0f);
        }

        inline float InOutSine(float t)
        {
            return -(std::cos(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
        }

        // ========== Exponential ==========
        inline float InExpo(float t)
        {
            return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f));
        }

        inline float OutExpo(float t)
        {
            return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
        }

        inline float InOutExpo(float t)
        {
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;

            return t < 0.5f
                       ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f
                       : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
        }

        // ========== Circular ==========
        inline float InCirc(float t)
        {
            return 1.0f - std::sqrt(1.0f - t * t);
        }

        inline float OutCirc(float t)
        {
            return std::sqrt(1.0f - (t - 1.0f) * (t - 1.0f));
        }

        inline float InOutCirc(float t)
        {
            return t < 0.5f
                       ? (1.0f - std::sqrt(1.0f - 4.0f * t * t)) / 2.0f
                       : (std::sqrt(1.0f - (-2.0f * t + 2.0f) * (-2.0f * t + 2.0f)) + 1.0f) / 2.0f;
        }

        // ========== Back ==========
        inline constexpr float InBack(float t)
        {
            constexpr float c1 = 1.70158f;
            constexpr float c3 = c1 + 1.0f;

            return c3 * t * t * t - c1 * t * t;
        }

        inline constexpr float OutBack(float t)
        {
            constexpr float c1 = 1.70158f;
            constexpr float c3 = c1 + 1.0f;

            return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
        }

        inline constexpr float InOutBack(float t)
        {
            constexpr float c1 = 1.70158f;
            constexpr float c2 = c1 * 1.525f;

            return t < 0.5f
                       ? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
                       : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
        }

        // ========== Elastic ==========
        inline float InElastic(float t)
        {
            constexpr float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;

            return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f
                                                 : -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * c4);
        }

        inline float OutElastic(float t)
        {
            constexpr float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;

            return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f
                                                 : std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
        }

        inline float InOutElastic(float t)
        {
            constexpr float c5 = (2.0f * std::numbers::pi_v<float>) / 4.5f;

            return t == 0.0f    ? 0.0f
                   : t == 1.0f  ? 1.0f
                   : t < 0.5f   ? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f
                                : (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
        }

        // ========== Bounce ==========
        inline constexpr float OutBounce(float t)
        {
            constexpr float n1 = 7.5625f;
            constexpr float d1 = 2.75f;

            if (t < 1.0f / d1)
            {
                return n1 * t * t;
            }
            else if (t < 2.0f / d1)
            {
                const float t2 = t - 1.5f / d1;
                return n1 * t2 * t2 + 0.75f;
            }
            else if (t < 2.5f / d1)
            {
                const float t2 = t - 2.25f / d1;
                return n1 * t2 * t2 + 0.9375f;
            }
            else
            {
                const float t2 = t - 2.625f / d1;
                return n1 * t2 * t2 + 0.984375f;
            }
        }

        inline constexpr float InBounce(float t)
        {
            return 1.0f - OutBounce(1.0f - t);
        }

        inline constexpr float InOutBounce(float t)
        {
            return t < 0.5f
                       ? (1.0f - OutBounce(1.0f - 2.0f * t)) / 2.0f
                       : (1.0f + OutBounce(2.0f * t - 1.0f)) / 2.0f;
        }

        // ========== 实用函数 ==========

        /**
         * @brief 线性插值
         * @param start 起始值
         * @param end 结束值
         * @param t 插值参数 [0, 1]
         * @return 插值结果
         */
        template <typename T>
        inline constexpr T Lerp(T start, T end, float t)
        {
            return start + (end - start) * t;
        }

        /**
         * @brief 将值限制在范围内
         * @param value 要限制的值
         * @param min 最小值
         * @param max 最大值
         * @return 限制后的值
         */
        template <typename T>
        inline constexpr T Clamp(T value, T min, T max)
        {
            return value < min ? min : (value > max ? max : value);
        }

    } // namespace Easing
} // namespace Match3
