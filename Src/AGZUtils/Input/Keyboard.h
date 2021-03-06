﻿#pragma once

#include <cstdint>
#include <cstring>

#include "Event.h"
#include "PredefinedHandler.h"

namespace AGZ::Input
{
    
/**
 * @brief 键值类型
 */
using Key = std::int32_t;

constexpr Key KEY_UNKNOWN = -1;

constexpr Key KEY_SPACE      = 32;
constexpr Key KEY_APOSTROPHE = 39; // '
constexpr Key KEY_COMMA      = 44; // ,
constexpr Key KEY_MINUS      = 45; // -
constexpr Key KEY_PERIOD     = 46; // .
constexpr Key KEY_SLASH      = 47; // /

// 数字0-9与ASCII '0'-'9'相同

constexpr Key KEY_D0 = 48;
constexpr Key KEY_D1 = 49;
constexpr Key KEY_D2 = 50;
constexpr Key KEY_D3 = 51;
constexpr Key KEY_D4 = 52;
constexpr Key KEY_D5 = 53;
constexpr Key KEY_D6 = 54;
constexpr Key KEY_D7 = 55;
constexpr Key KEY_D8 = 56;
constexpr Key KEY_D9 = 57;

constexpr Key KEY_SEMICOLON = 59; // ;
constexpr Key KEY_EQUAL     = 61; // =

// 字母A-Z与ASCII 'A'-'Z'相同

constexpr Key KEY_A = 65;
constexpr Key KEY_B = 66;
constexpr Key KEY_C = 67;
constexpr Key KEY_D = 68;
constexpr Key KEY_E = 69;
constexpr Key KEY_F = 70;
constexpr Key KEY_G = 71;
constexpr Key KEY_H = 72;
constexpr Key KEY_I = 73;
constexpr Key KEY_J = 74;
constexpr Key KEY_K = 75;
constexpr Key KEY_L = 76;
constexpr Key KEY_M = 77;
constexpr Key KEY_N = 78;
constexpr Key KEY_O = 79;
constexpr Key KEY_P = 80;
constexpr Key KEY_Q = 81;
constexpr Key KEY_R = 82;
constexpr Key KEY_S = 83;
constexpr Key KEY_T = 84;
constexpr Key KEY_U = 85;
constexpr Key KEY_V = 86;
constexpr Key KEY_W = 87;
constexpr Key KEY_X = 88;
constexpr Key KEY_Y = 89;
constexpr Key KEY_Z = 90;

constexpr Key KEY_LBRAC        = 91; // [
constexpr Key KEY_BACKSLASH    = 92; // backslash
constexpr Key KEY_RBRAC        = 93; // ]
constexpr Key KEY_GRAVE_ACCENT = 96; // `

constexpr Key KEY_ESCAPE    = 256;
constexpr Key KEY_ENTER     = 257;
constexpr Key KEY_TAB       = 258;
constexpr Key KEY_BACKSPACE = 259;
constexpr Key KEY_INSERT    = 260;
constexpr Key KEY_DELETE    = 261;

constexpr Key KEY_RIGHT = 262;
constexpr Key KEY_LEFT  = 263;
constexpr Key KEY_DOWN  = 264;
constexpr Key KEY_UP    = 265;

constexpr Key KEY_PAGE_UP   = 266;
constexpr Key KEY_PAGE_DOWN = 267;
constexpr Key KEY_HOME      = 268;
constexpr Key KEY_END       = 269;

constexpr Key KEY_CAPS_LOCK    = 280;
constexpr Key KEY_NUM_LOCK     = 282;
constexpr Key KEY_PRINT_SCREEN = 283;
constexpr Key KEY_PAUSE        = 284;

constexpr Key KEY_F1  = 290;
constexpr Key KEY_F2  = 291;
constexpr Key KEY_F3  = 292;
constexpr Key KEY_F4  = 293;
constexpr Key KEY_F5  = 294;
constexpr Key KEY_F6  = 295;
constexpr Key KEY_F7  = 296;
constexpr Key KEY_F8  = 297;
constexpr Key KEY_F9  = 298;
constexpr Key KEY_F10 = 299;
constexpr Key KEY_F11 = 300;
constexpr Key KEY_F12 = 301;

constexpr Key KEY_NUMPAD_0 = 320;
constexpr Key KEY_NUMPAD_1 = 321;
constexpr Key KEY_NUMPAD_2 = 322;
constexpr Key KEY_NUMPAD_3 = 323;
constexpr Key KEY_NUMPAD_4 = 324;
constexpr Key KEY_NUMPAD_5 = 325;
constexpr Key KEY_NUMPAD_6 = 326;
constexpr Key KEY_NUMPAD_7 = 327;
constexpr Key KEY_NUMPAD_8 = 328;
constexpr Key KEY_NUMPAD_9 = 329;

constexpr Key KEY_NUMPAD_DECIMAL = 330;
constexpr Key KEY_NUMPAD_DIV     = 331;
constexpr Key KEY_NUMPAD_MUL     = 332;
constexpr Key KEY_NUMPAD_SUB     = 333;
constexpr Key KEY_NUMPAD_ADD     = 334;
constexpr Key KEY_NUMPAD_ENTER   = 335;

constexpr Key KEY_LSHIFT = 340;
constexpr Key KEY_LCTRL  = 341;
constexpr Key KEY_LALT   = 342;
constexpr Key KEY_RSHIFT = 344;
constexpr Key KEY_RCTRL  = 345;
constexpr Key KEY_RALT   = 346;

constexpr Key KEY_MAX = 346;

/**
 * @brief 键盘按下事件
 */
struct KeyDown
{
    Key key;
};

/**
 * @brief 键盘松开事件
 */
struct KeyUp
{
    Key key;
};

/**
 * @brief 字符输入事件
 */
struct CharEnter
{
    uint32_t ch;
};

/**
 * @brief 键盘事件category
 */
class Keyboard : public EventCategoryBase<KeyDown, KeyUp, CharEnter>
{
    bool isKeyPressed_[KEY_MAX + 1];

public:

    Keyboard()
    {
        std::memset(isKeyPressed_, 0, sizeof(isKeyPressed_));
    }

    void Invoke(const KeyDown &param)
    {
        Key k = param.key;
        AGZ_ASSERT(k == KEY_UNKNOWN || (0 <= k && k <= KEY_MAX));
        if(k != KEY_UNKNOWN)
            isKeyPressed_[k] = true;
        InvokeAllHandlers(param);
    }

    void Invoke(const KeyUp &param)
    {
        Key k = param.key;
        AGZ_ASSERT(k == KEY_UNKNOWN || (0 <= k && k <= KEY_MAX));
        if(k != KEY_UNKNOWN)
            isKeyPressed_[k] = false;
        InvokeAllHandlers(param);
    }

    void Invoke(const CharEnter &param)
    {
        InvokeAllHandlers(param);
    }

    /**
     * @brief 查询某个键盘按键是否处于按压状态
     */
    bool IsKeyPressed(Key k) const noexcept
    {
        AGZ_ASSERT(0 <= k && k <= KEY_MAX);
        return isKeyPressed_[k];
    }

    /**
     * @warning 供Capturer使用
     */
    void _setKeyPressed(Key k, bool pressed) noexcept
    {
        isKeyPressed_[k] = pressed;
    }
};

PREDEFINED_HANDLER_FOR_SPECIFIC_EVENT(KeyDown);
PREDEFINED_HANDLER_FOR_SPECIFIC_EVENT(KeyUp);
PREDEFINED_HANDLER_FOR_SPECIFIC_EVENT(CharEnter);

/**
 * @brief 键盘管理器，负责打包键盘category和指定的事件捕获器
 * @tparam CapturerType 捕获器类型
 */
template<typename CapturerType>
class KeyboardManager : public EventManagerBase<EventCategoryList<Keyboard>, EventCapturerList<CapturerType>>
{
public:

    /**
     * @brief 取得键盘实例
     */
    Keyboard &GetKeyboard() noexcept
    {
        return this->template GetCategoryByType<Keyboard>();
    }

    /**
     * @brief 取得捕获器实例
     */
    CapturerType &GetCapturer() noexcept
    {
        return this->template GetCapturerByType<CapturerType>();
    }

    /**
     * @brief 取得键盘实例
     */
    const Keyboard &GetKeyboard() const noexcept
    {
        return this->template GetCategoryByType<Keyboard>();
    }

    /**
     * @brief 取得捕获器实例
     */
    const CapturerType &GetCapturer() const noexcept
    {
        return this->template GetCapturerByType<CapturerType>();
    }
};

} // namespace AGZ::Input
