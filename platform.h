/* date = December 15th 2022 6:33 am */
#ifndef PLATFORM_H
#include "types.h"

struct Platform;
global Platform *platform;

#include "arena.h"

enum EventKind{
    EventKind_Null, 
    EventKind_MouseButtonPress,
    EventKind_MouseButtonRelease,
    EventKind_MouseScroll,
    EventKind_MouseMove,
    EventKind_GamepadButtonPress,
    EventKind_GamepadStickMove,
    EventKind_GamepadTrigger,
    EventKind_KeyPress,
    EventKind_KeyRelease,
};

enum MouseButton{
    MouseButton_Null,
    MouseButton_Left,
    MouseButton_Right,
    MouseButton_Middle,
    MouseButton_X1, 
    MouseButton_X2,
    MouseButton_VWheel,
    MouseButton_HWheel,
};

enum KbdButton{
    KbdButton_Null,
    KbdButton_0,KbdButton_1, KbdButton_2, KbdButton_3, KbdButton_4, 
    KbdButton_5, KbdButton_6, KbdButton_7, KbdButton_8, KbdButton_9,
    
    KbdButton_Minus, KbdButton_Plus, KbdButton_Equal, KbdButton_Comma,
    KbdButton_Period, KbdButton_Colon, KbdButton_Semicolon, KbdButton_Quote,
    KbdButton_DoubleQuote, KbdButton_OpenBracket, KbdButton_CloseBracket,
    KbdButton_OpenBrace, KbdButton_CloseBrace, KbdButton_LessThan, KbdButton_GreaterThan,
    KbdButton_ForwardSlash, KbdButton_BackSlash, KbdButton_Bar, KbdButton_Underscore,
    KbdButton_QuestionMark, KbdButton_Acute, KbdButton_Tilde,
    
    KbdButton_A, KbdButton_B, KbdButton_C, KbdButton_D, KbdButton_E,
    KbdButton_F, KbdButton_G, KbdButton_H, KbdButton_I, KbdButton_J,
    KbdButton_K, KbdButton_L, KbdButton_M, KbdButton_N, KbdButton_O, 
    KbdButton_P, KbdButton_Q, KbdButton_R, KbdButton_S, KbdButton_T,
    KbdButton_U, KbdButton_V, KbdButton_W, KbdButton_X, KbdButton_Y,
    KbdButton_Z,
    
    KbdButton_Shift, KbdButton_Ctrl, KbdButton_Alt, KbdButton_Left,
    KbdButton_Right, KbdButton_Up, KbdButton_Down, KbdButton_Backspace,
    KbdButton_Tab, KbdButton_Return, KbdButton_CapsLock, KbdButton_Space,
    KbdButton_PageUp, KbdButton_PageDown, KbdButton_End, KbdButton_Home,
    KbdButton_Delete, KbdButton_Escape,
    
    KbdButton_F1, KbdButton_F2, KbdButton_F3, KbdButton_F4, KbdButton_F5,
    KbdButton_F6, KbdButton_F7, KbdButton_F8, KbdButton_F9, KbdButton_F10,
    KbdButton_F11, KbdButton_F12,
};

enum GamepadButton{
    GamepadButton_Null,
    GamepadButton_DpadUp = 0x1,
    GamepadButton_DpadDown = 0x2,
    GamepadButton_DpadLeft = 0x4,
    GamepadButton_DpadRight = 0x8,
    GamepadButton_Start = 0x10,
    GamepadButton_Back = 0x20,
    GamepadButton_LeftThumb = 0x40,
    GamepadButton_RightThumb = 0x80,
    GamepadButton_LeftShoulder = 0x100, 
    GamepadButton_RightShoulder = 0x200,
    GamepadButton_A = 0x1000,
    GamepadButton_B = 0x2000,
    GamepadButton_X = 0x4000,
    GamepadButton_Y = 0x8000,
};

enum ButtonModifier{
    ButtonModifier_Null, 
    ButtonModifier_Ctrl = 0x1, 
    ButtonModifier_Shift = 0x2, 
    ButtonModifier_Alt = 0x4,
    ButtonModifier_Left = 0x8, // mouse
    ButtonModifier_Right = 0x10,
    ButtonModifier_Middle = 0x20,
    ButtonModifier_X1 = 0x40,
    ButtonModifier_X2 = 0x80,
};

struct KeyMap{
    String8 name;
    U64 keycode;
};

#define MapKey(keycode, name) {S8Literal(name), keycode}
global KeyMap keymap[]={
    MapKey(KbdButton_Null, "Null"),
    MapKey(KbdButton_0, "0"), MapKey(KbdButton_1, "1"), MapKey(KbdButton_2, "2"), 
    MapKey(KbdButton_3, "3"), MapKey(KbdButton_4, "4"), MapKey(KbdButton_5, "5"),
    MapKey(KbdButton_6, "6"), MapKey(KbdButton_7, "7"), MapKey(KbdButton_8, "8"), 
    MapKey(KbdButton_9, "9"), MapKey(KbdButton_Minus, "Minus"), MapKey(KbdButton_Plus, "Plus"),
    MapKey(KbdButton_Equal, "Equal"), MapKey(KbdButton_Comma, "Comma"), MapKey(KbdButton_Period, "Period"),
    MapKey(KbdButton_Colon, "Colon"), MapKey(KbdButton_Semicolon, "Semicolon"), MapKey(KbdButton_Quote, "Quote"),
    MapKey(KbdButton_DoubleQuote, "Double Quote"),  MapKey(KbdButton_OpenBracket, "Open Bracket"), 
    MapKey(KbdButton_CloseBracket, "Close Bracket"), MapKey(KbdButton_OpenBrace, "Open Brace"),  
    MapKey(KbdButton_CloseBrace, "Close Brace"),  MapKey(KbdButton_LessThan, "Less Than"), MapKey(KbdButton_GreaterThan, "Greater Than"),
    MapKey(KbdButton_ForwardSlash, "Forward Slash"),  MapKey(KbdButton_BackSlash, "Backslash"),  MapKey(KbdButton_Bar, "Bar"), 
    MapKey(KbdButton_Underscore, "Underscore"), MapKey(KbdButton_QuestionMark, "Question Mark"), MapKey(KbdButton_Acute, "Acute"), 
    MapKey(KbdButton_Tilde, "Tilde"),
    MapKey(KbdButton_A, "A"), MapKey(KbdButton_B, "B"), MapKey(KbdButton_C, "C"), MapKey(KbdButton_D, "D"), 
    MapKey(KbdButton_E, "E"), MapKey(KbdButton_F, "F"), MapKey(KbdButton_G, "G"), MapKey(KbdButton_H, "H"), 
    MapKey(KbdButton_I, "I"), MapKey(KbdButton_J, "J"), MapKey(KbdButton_K, "K"), MapKey(KbdButton_L, "L"), 
    MapKey(KbdButton_M, "M"), MapKey(KbdButton_N, "N"), MapKey(KbdButton_O, "O"), MapKey(KbdButton_P, "P"), 
    MapKey(KbdButton_Q, "Q"), MapKey(KbdButton_R, "R"), MapKey(KbdButton_S, "S"), MapKey(KbdButton_T, "T"),
    MapKey(KbdButton_U, "U"), MapKey(KbdButton_V, "V"), MapKey(KbdButton_W, "W"), MapKey(KbdButton_X, "X"), 
    MapKey(KbdButton_Y, "Y"), MapKey(KbdButton_Z, "Z"),
    MapKey(KbdButton_Shift, "Shift"), MapKey(KbdButton_Ctrl, "Ctrl"), MapKey(KbdButton_Alt, "Alt"), 
    MapKey(KbdButton_Left, "Left"), MapKey(KbdButton_Right, "Right"), MapKey(KbdButton_Up, "Up"), 
    MapKey(KbdButton_Down, "Down"), MapKey(KbdButton_Backspace, "Backspace"), MapKey(KbdButton_Tab, "Tab"), 
    MapKey(KbdButton_Return, "Return"),  MapKey(KbdButton_CapsLock, "Caps Lock"),  MapKey(KbdButton_Space, "Space"),
    MapKey(KbdButton_PageUp, "Page Up"),  MapKey(KbdButton_PageDown, "Page Down"),  MapKey(KbdButton_End, "End"), 
    MapKey(KbdButton_Home, "Home"), MapKey(KbdButton_Delete, "Delete"), MapKey(KbdButton_Escape, "Escape"),
    MapKey(KbdButton_F1, "F1"), MapKey(KbdButton_F2, "F2"), MapKey(KbdButton_F3, "F3"), MapKey(KbdButton_F4, "F4"), 
    MapKey(KbdButton_F5, "F5"), MapKey(KbdButton_F6, "F6"), MapKey(KbdButton_F7, "F7"), MapKey(KbdButton_F8, "F8"), 
    MapKey(KbdButton_F9, "F9"), MapKey(KbdButton_F10, "F10"), MapKey(KbdButton_F11, "F11"), MapKey(KbdButton_F12, "F12"),
};
#undef MapKey

struct Event{
    Event *next;
    Event *prev;
    I32 kind;
    I32 button;
    I32 modifier;
    U64 chr;
    union{
        struct{
            V2f lstick;
            V2f rstick;
            F32 ltrigger;
            F32 rtrigger;
        };
        struct{
            V2f pos;
            V2f delta; // TODO(oleksii): Retrieve the mouse position on the previous frame and compute the delta
            V2f scroll;
        };
    };
};

struct Platform{
    void *(*reserve_mem)(U64 size);
    void (*commit_mem)(void *ptr, U64 size);
    void (*decommit_mem)(void *ptr, U64 size);
    void (*release_mem)(void *ptr);
    
    Arena arena;
    Arena frame_arena;
    
    Event *event_queue;
    
    I32 wnd_height;
    I32 wnd_width;
};

// update the app state
#define APP_UPDATE(procname) void procname(Platform *_platform)
typedef void (*AppUpdatePtr)(Platform *);
APP_UPDATE(app_update_stub){}

// render the app
#define APP_RENDER(procname) void procname(Platform *_platform)
typedef void (*AppRenderPtr)(Platform *);
APP_RENDER(app_render_stub){}

struct UIRect{
    V2f minp;
    V2f maxp;
    V2f dist2maxp;
    V2f dist2minp;
    V4f color;
    B32 move;
};

function UIRect ui_make_rect(V2f minp, V2f maxp, V4f color){
    UIRect result = {};
    result.minp = minp;
    result.maxp = maxp;
    result.color = color;
    return(result);
}

struct AppState{
    UIRect rect0;
    UIRect rect1;
    V4f resize_rect_color;
    F32 ratio; // wnd_width / wnd_height
    V2f cursor; // relative to OpenGL coordinate system
    B32 initialized;
};

#define PLATFORM_H
#endif //PLATFORM_H