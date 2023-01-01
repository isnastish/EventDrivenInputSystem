#include "platform.h"
#include "arena.cpp"

#include "slim_windows.h"

#include <xinput.h>
#include <gl/gl.h>

global B32 global_running;

function void *w32_reserve_memory(U64 size){
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return(result);
}

function void w32_commit_memory(void *ptr, U64 size){
    VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

function void w32_decommit_memory(void *ptr, U64 size){
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

function void w32_release_memory(void *ptr){
    VirtualFree(ptr, 0, MEM_RELEASE);
}

typedef BOOL (*WglChoosePixelFormatARBPtr)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (*WglCreateContextAttribsARBPtr)(HDC hDC, HGLRC hShareContext, const int *attribList);

global WglChoosePixelFormatARBPtr    wglChoosePixelFormatARB;
global WglCreateContextAttribsARBPtr wglCreateContextAttribsARB;

#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023

global const I32 choose_pixel_attrib_list[]={
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0, // end
};

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

global const I32 create_ctx_attrib_list[]={
    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
    WGL_CONTEXT_MINOR_VERSION_ARB, 6,
    WGL_CONTEXT_PROFILE_MASK_ARB, 
    WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0, // end
};

function void *w32_load_glproc(String8 procname){
    void *result = wglGetProcAddress((char *)procname.data);
    if((result == 0) || 
       (result == (void *)0x1) ||
       (result == (void *)0x2) ||
       (result == (void *)0x3) ||
       (result == (void *)-1)){
        HMODULE lib = LoadLibraryA("opengl32.dll");
        result = (void *)GetProcAddress(lib, (char *)procname.data);
    }
    return(result);
}

function B32 w32_init_opengl(HDC hdc){
    B32 result = 0;
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    
    I32 pixel_format = ChoosePixelFormat(hdc, &pfd);
    PIXELFORMATDESCRIPTOR suggested_pfd;
    DescribePixelFormat(hdc, pixel_format, sizeof(suggested_pfd), &suggested_pfd);
    SetPixelFormat(hdc, pixel_format, &suggested_pfd);
    HGLRC dummy_opengl_ctx = wglCreateContext(hdc);
    wglMakeCurrent(hdc, dummy_opengl_ctx);
    if(dummy_opengl_ctx){
        wglChoosePixelFormatARB = (WglChoosePixelFormatARBPtr)w32_load_glproc(S8Literal("wglChoosePixelFormatARB"));
        I32 format;
        U32 nformats;
        wglChoosePixelFormatARB(hdc, choose_pixel_attrib_list, 0, 1, &format, &nformats);
        wglCreateContextAttribsARB = (WglCreateContextAttribsARBPtr)w32_load_glproc(S8Literal("wglCreateContextAttribsARB"));
        HGLRC real_opengl_ctx = wglCreateContextAttribsARB(hdc, dummy_opengl_ctx, create_ctx_attrib_list);
        if(real_opengl_ctx){ // delete the dummy context
            wglMakeCurrent(hdc, 0);
            wglDeleteContext(dummy_opengl_ctx);
            wglMakeCurrent(hdc, real_opengl_ctx);
        }
    }
    return(result);
}

function V2i w32_get_window_metrics(HWND window){
    V2i result = {};
    RECT rect = {};
    GetClientRect(window, &rect);
    result.width = rect.right - rect.left;
    result.height = rect.bottom - rect.top;
    return(result);
}

function LRESULT CALLBACK w32_main_wndproc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam){
    LRESULT result = 0;
    switch(msg){
        case WM_DESTROY:
        case WM_CLOSE:{
            global_running = 0; 
        }break;
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYUP:
        case WM_SYSCHAR:
        case WM_CHAR:{
            Assert(0);
        }break;
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:{
            Assert(0);
        }break;
        default:{
            result = DefWindowProc(wnd, msg, wparam, lparam); 
        };
    };
    return(result);
}

struct W32AppCode{
    HMODULE dll;
    AppUpdatePtr update;
    AppRenderPtr render;
    FILETIME last_write;
    B32 valid;
};

function W32AppCode w32_load_appcode(String8 dllname){
    W32AppCode result = {};
    result.dll = LoadLibraryA((LPCSTR)dllname.data);
    if(result.dll){
        result.update = (AppUpdatePtr)GetProcAddress(result.dll, "app_update");
        result.render = (AppRenderPtr)GetProcAddress(result.dll, "app_render");
        if(result.update && result.render){
            result.valid = true;
        }
    }
    if(!result.valid){
        result.update = app_update_stub;
        result.render = app_render_stub;
    }
    return(result);
}

function void w32_unload_appcode(W32AppCode *appcode){
    if(appcode->dll){
        FreeLibrary(appcode->dll);
        appcode->dll = 0;
    }
    appcode->update = app_update_stub;
    appcode->render = app_render_stub;
}

#define L_STICK_DEADZONE 7849
#define R_STICK_DEADZONE 8689
#define TRIGGER_THRESHOLD 30

#define XINPUT_SET_STATE(procname) DWORD procname(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef XINPUT_SET_STATE(xinput_set_state);
XINPUT_SET_STATE(XInputSetStateStub){
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global xinput_set_state *XInputSetStatePtr = XInputSetStateStub;
#define XInputSetState XInputSetStatePtr

#define XINPUT_GET_STATE(procname) DWORD procname(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef XINPUT_GET_STATE(xinput_get_state);
XINPUT_GET_STATE(XInputGetStateStub){
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global xinput_get_state *XInputGetStatePtr = XInputGetStateStub;
#define XInputGetState XInputGetStatePtr

function void w32_init_xinput(){
    HMODULE library = LoadLibraryA("xinput1_4.dll");
    library = (library ? library : LoadLibraryA("xinput9_1_0.dll"));
    library = (library ? library : LoadLibraryA("xinput1_3.dll"));
    if(library){
        XInputSetStatePtr = (xinput_set_state *)GetProcAddress(library, "XInputSetState");
        XInputGetStatePtr = (xinput_get_state *)GetProcAddress(library, "XInputGetState");
        if(!XInputGetStatePtr){
            XInputGetStatePtr = XInputGetStateStub;
        }
        if(!XInputSetStatePtr){
            XInputSetStatePtr = XInputSetStateStub;
        }
    }
}

// TODO(oleksii): COM stuff. Loaded CoInitialize/CoUninitialize only to suppress the warnings, that CoInitialize wasn't call, 
// I didn't give me an ability to debug the program and output things (keys being pressed) to the output debug stream.
// Decided to load the dll because didn't want to link with 'ole32.lib' library.
#include <combaseapi.h>
#define CO_INITIALIZE(procname) HRESULT procname(LPVOID pvReserved)
typedef CO_INITIALIZE(co_initialize);
CO_INITIALIZE(CoInitializeStub){
    return(E_UNEXPECTED);
}
global co_initialize *CoInitializePtr = CoInitializeStub;
#define CoInitialize CoInitializePtr

#define CO_UNINITIALIZE(procname) void procname()
typedef CO_UNINITIALIZE(co_uninitialize);
CO_UNINITIALIZE(CoUninitializeStub){}
global co_uninitialize *CoUninitializePtr = CoUninitializeStub;
#define CoUninitialize CoUninitializePtr

function void w32_dummy_init_com(){
    HMODULE library = LoadLibraryA("ole32.dll");
    if(library){
        CoInitializePtr = (co_initialize *)GetProcAddress(library, "CoInitialize");
        CoUninitializePtr = (co_uninitialize *)GetProcAddress(library, "CoUninitialize");
        if(!CoInitializePtr){
            CoInitializePtr = CoInitializeStub;
        }
        if(!CoUninitializePtr){
            CoUninitializePtr = CoUninitializeStub;
        }
    }
}

function V2f w32_get_cursor_pos(HWND window){
    V2f result = {};
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(window, &cursor);
    result.x = (F32)cursor.x;
    result.y = (F32)cursor.y;
    return(result);
}

// TODO(oleksii): I don't think that this variable should be global. If we will be able to handle
// all the inputs outside main window procedure, we can remove this variable completely, 
// otherwise it has to stay.
global Platform global_platform;

// TODO(oleksii): If it's possible, avoid the copy of Event and pass it by pointer or reference. 
// And what's more important is that this function shouldn't live here, 
// For now I don't really know where to put it, because the project is not well-structured!
function void enqueue_event(Arena *arena, Event *queue, Event event){
    Event *new_event = push_struct(arena, Event);
    *new_event = event;
    QueuePush(queue, new_event);
}

INT WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, int show_code){
    w32_dummy_init_com();
    CoInitialize(0);
    
    platform = &global_platform;
    
    global_platform.reserve_mem  = w32_reserve_memory;
    global_platform.commit_mem   = w32_commit_memory;
    global_platform.decommit_mem = w32_decommit_memory;
    global_platform.release_mem  = w32_release_memory;
    
    global_platform.arena       = arena_make_align(Gigabytes(4), 8u);
    global_platform.frame_arena = arena_make_align(Gigabytes(4), 8u);
    
    W32AppCode appcode = w32_load_appcode(S8Literal("e:/work/build/app_code.dll"));
    
    WNDCLASSA wndclass = {};
    wndclass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    wndclass.lpfnWndProc = w32_main_wndproc;
    wndclass.cbWndExtra = sizeof(Platform);
    wndclass.hInstance = (HINSTANCE)GetModuleHandleA(0);
    wndclass.hIcon;
    wndclass.hCursor;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wndclass.lpszClassName = (LPSTR)"ApplicationWindowClass";
    RegisterClassA(&wndclass);
    
    RECT rect = {0, 0, 1080, 720};
    // NOTE(oleksii): The documentation says that I cannot speicfy WS_OVERLAPPED style for whatever reason, 
    // so I have to exclude it, I guess. But even with WS_OVERLAPPED flag everything works fine, so I'm confused.
    AdjustWindowRect(&rect, (WS_OVERLAPPEDWINDOW ^ WS_OVERLAPPED), 0);
    HWND window = CreateWindowExA(0, (LPCSTR)wndclass.lpszClassName, (LPCSTR)"Application", WS_OVERLAPPEDWINDOW, 
                                  CW_USEDEFAULT, CW_USEDEFAULT, (rect.right - rect.left), (rect.bottom - rect.top), 0, 0, instance, 0);
    
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    
    HDC hdc = GetDC(window);
    w32_init_opengl(hdc);
    w32_init_xinput();
    
    // TODO(oleksii): 
    push_struct(&global_platform.arena, AppState);
    
    global_running = 1;
    while(global_running){
        TempArena tmp_arena = temp_arena_start(&platform->frame_arena);
        platform->event_queue = push_struct(&platform->frame_arena, Event);
        QueueInit(platform->event_queue);
        
        MSG msg;
        while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)){
            U32 message = msg.message;
            Event newevent = {};
            I32 button_modifier = 0; 
            
            // NOTE(oleksii): We should not compute the button based on its modifier here, 
            // we should propagate that to the app code, and compute the actual button there.
            // Cause the main purpose of the platform layer is to push an event together with 
            // all the auxiliary information, don't make any decisions here.
            button_modifier |= (GetKeyState(VK_SHIFT) & (1 << 16)) ? ButtonModifier_Shift : 0;
            button_modifier |= (GetKeyState(VK_CONTROL) & (1 << 16)) ? ButtonModifier_Ctrl : 0;
            button_modifier |= (GetKeyState(VK_MENU) & (1 << 16)) ? ButtonModifier_Alt : 0;
            button_modifier |= (GetKeyState(VK_LBUTTON) & (1 << 16)) ? ButtonModifier_Left : 0;
            button_modifier |= (GetKeyState(VK_RBUTTON) & (1 << 16)) ? ButtonModifier_Right : 0;
            button_modifier |= (GetKeyState(VK_MBUTTON) & (1 << 16)) ? ButtonModifier_Middle : 0;
            button_modifier |= (GetKeyState(VK_XBUTTON1) & (1 << 16)) ? ButtonModifier_X1 : 0;
            button_modifier |= (GetKeyState(VK_XBUTTON2) & (1 << 16)) ? ButtonModifier_X2 : 0;
            newevent.modifier = button_modifier;
            
            if((message == WM_SYSKEYDOWN) ||
               (message == WM_KEYDOWN) ||
               (message == WM_SYSKEYUP) ||
               (message == WM_KEYUP)){
                I64 vkcode = msg.wParam;
                I32 wasdown = ((msg.lParam & (1 << 30)) != 0); // not used
                I32 isdown = ((msg.lParam & (1 << 31)) == 0);
                
                if(vkcode >= '0' && vkcode <= '9'){
                    newevent.button = (KbdButton_0 + vkcode-'0');
                }
                else if(vkcode >= VK_F1 && vkcode <= VK_F12){
                    newevent.button = (KbdButton_F1 + vkcode-VK_F1);
                }
                else if(vkcode >= 'A' && vkcode <= 'Z'){
                    newevent.button = (KbdButton_A + vkcode-'A');
                }
                else if(vkcode == VK_BACK){
                    newevent.button = KbdButton_Backspace;
                }
                else if(vkcode == VK_TAB){
                    newevent.button = KbdButton_Tab;
                }
                else if(vkcode == VK_RETURN){
                    newevent.button = KbdButton_Return;
                }
                
                // NOTE(oleksii): If we receive VK_SHIFT/VK_CONTROL/VK_MENU messages and the button_modifier was set, 
                // we can zero it out and depend only on the actual button.
#if 1
                else if(vkcode == VK_SHIFT){
                    newevent.button = KbdButton_Shift;
                    newevent.modifier &= ~button_modifier;
                }
                else if(vkcode == VK_CONTROL){
                    newevent.button = KbdButton_Ctrl;
                    newevent.modifier &= ~button_modifier;
                }
                else if(vkcode == VK_MENU){
                    newevent.button = KbdButton_Alt;
                    newevent.modifier &= ~button_modifier;
                }
#endif
                // TODO(oleksii): When CapsLock button was pressed we have to switch the mode we print alphabetic letters
                // and then when it was released we have to switch the mode back, luckily it's very easy to do, because
                // we are reciving press and release events. But it has to be tested anyway.
                else if(vkcode == VK_CAPITAL){
                    newevent.button = KbdButton_CapsLock;
                }
                
                else if(vkcode == VK_ESCAPE){
                    newevent.button = KbdButton_Escape;
                }
                else if(vkcode == VK_SPACE){
                    newevent.button = KbdButton_Space;
                }
                else if(vkcode == VK_PRIOR){
                    newevent.button = KbdButton_PageUp;
                }
                else if(vkcode == VK_NEXT){
                    newevent.button = KbdButton_PageDown;
                }
                else if(vkcode == VK_END){
                    newevent.button = KbdButton_End;
                }
                else if(vkcode == VK_HOME){
                    newevent.button = KbdButton_Home;
                }
                else if(vkcode == VK_LEFT){
                    newevent.button = KbdButton_Left;
                }
                else if(vkcode == VK_UP){
                    newevent.button = KbdButton_Up;
                }
                else if(vkcode == VK_RIGHT){
                    newevent.button = KbdButton_Right;
                }
                else if(vkcode == VK_DOWN){
                    newevent.button = KbdButton_Down;
                }
                else if(vkcode == VK_DELETE){
                    newevent.button = KbdButton_Delete;
                }
                
                // TODO(oleksii): Not sure that computing the actual button here based on Shift modifier is better
                // than doing it on the application side, will have to think about that wisely.
                else if(vkcode == VK_OEM_1){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ? 
                                       KbdButton_Colon : KbdButton_Semicolon);
                }
                else if(vkcode == VK_OEM_PLUS){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ? 
                                       KbdButton_Plus : KbdButton_Equal);
                }
                else if(vkcode == VK_OEM_COMMA){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ? 
                                       KbdButton_LessThan : KbdButton_Comma);
                }
                else if(vkcode == VK_OEM_MINUS){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ? 
                                       KbdButton_Underscore : KbdButton_Minus);
                }
                else if(vkcode == VK_OEM_PERIOD){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ? 
                                       KbdButton_GreaterThan : KbdButton_Period);
                }
                else if(vkcode == VK_OEM_2){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ? 
                                       KbdButton_QuestionMark : KbdButton_ForwardSlash);
                }
                else if(vkcode == VK_OEM_3){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ? 
                                       KbdButton_Tilde : KbdButton_Acute);
                }
                else if(vkcode == VK_OEM_4){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ? 
                                       KbdButton_OpenBrace : KbdButton_OpenBracket);
                }
                else if(vkcode == VK_OEM_5){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ?
                                       KbdButton_Bar : KbdButton_BackSlash);
                }
                else if(vkcode == VK_OEM_6){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ?
                                       KbdButton_CloseBrace : KbdButton_CloseBracket);
                }
                else if(vkcode == VK_OEM_7){
                    newevent.button = ((button_modifier & ButtonModifier_Shift) ?
                                       KbdButton_DoubleQuote : KbdButton_Quote); 
                }
                
                if(isdown)
                    newevent.kind = EventKind_KeyPress;
                else
                    newevent.kind = EventKind_KeyRelease;
                
                // TODO(oleksii): WM_CHAR produced by WM_KEYDOWN and WM_KEYUP is not translated. 
                // Don't know the reason for that, but it seem like DefWindowProc(...) routine
                // has to be called while handling WM_SYSKEYDOWN/WM_KEYDOWN ... messages, and within WindowProc.
                // So, if we won't be able to work around WM_CHAR message, and read all the kbd input without it, 
                // we will have to move all the code which handles at least kbd input to WindowProc.
                // I think that moving all the kbd input will force mouse input as well to be moved to WindowProc, 
                // and the bad thing about it is that we have to maintain global_platform variable.
            }
            else if(message == WM_MOUSEWHEEL){
                newevent.kind = EventKind_MouseScroll;
                newevent.button = MouseButton_VWheel;
                newevent.scroll = V2f(0, HIWORD(msg.wParam));
                newevent.modifier = button_modifier;
            }
            else if(message == WM_MOUSEHWHEEL){
                newevent.kind = EventKind_MouseScroll;
                newevent.button = MouseButton_HWheel;
                newevent.scroll = V2f(HIWORD(msg.wParam), 0);
                newevent.modifier = button_modifier;
            }
            else if(message == WM_MOUSEMOVE){
                newevent.kind = EventKind_MouseMove;
                newevent.pos = w32_get_cursor_pos(window);
                newevent.modifier = button_modifier;
            }
            else if(message == WM_LBUTTONDOWN){
                newevent.kind = EventKind_MouseButtonPress;
                newevent.button = MouseButton_Left;
            }
            else if(message == WM_LBUTTONUP){
                newevent.kind = EventKind_MouseButtonRelease;
                newevent.button = MouseButton_Left;
            }
            else if(message == WM_RBUTTONDOWN){
                newevent.kind = EventKind_MouseButtonPress;
                newevent.button = MouseButton_Right;
            }
            else if(message == WM_RBUTTONUP){
                newevent.kind = EventKind_MouseButtonRelease;
                newevent.button = MouseButton_Right;
            }
            else if(message == WM_MBUTTONDOWN){
                newevent.kind = EventKind_MouseButtonPress;
                newevent.button = MouseButton_Middle;
            }
            else if(message == WM_MBUTTONUP){
                newevent.kind = EventKind_MouseButtonRelease;
                newevent.button = MouseButton_Middle;
            }
            else if(message == WM_XBUTTONDOWN){
                newevent.kind = EventKind_MouseButtonPress;
                if(HIWORD(msg.wParam) & XBUTTON1)
                    newevent.button = MouseButton_X1; 
                else if(HIWORD(msg.wParam) & XBUTTON2)
                    newevent.button = MouseButton_X2;
            }
            else if(message == WM_XBUTTONUP){
                newevent.kind = EventKind_MouseButtonRelease;
                if(HIWORD(msg.wParam) & XBUTTON1)
                    newevent.button = MouseButton_X1;
                else if(HIWORD(msg.wParam) & XBUTTON2)
                    newevent.button = MouseButton_X2;
            }
            else{
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
            
            if(newevent.kind != EventKind_Null){
                enqueue_event(&platform->frame_arena, platform->event_queue, newevent);
            }
        }
        
        // TODO(oleksii): We shouldn't call XInputGetState for 'empty' user slots on each frame for performance reasons, 
        // Instead we have to check whether a controller was connected/disconnected every 1-2 seconds.
        
        // TODO(oleksii): Currenly we combine all the button presses into a single event using, because it might be 
        // an overkill to create a separate event for each button press, but we have to test that and see what's better.
        // And, we create separate events for Left/Right sticks and Left/Right triggers. Techinally we can combine
        // those into a single event by introducing some flags, because it's possible that two sticks were moved on a single frame,
        // the same is true about buttons and triggers, (A button was hold while the Left stick was moved etc).
        // But, I think, using events I don't need to check whether the controller was connected, or not, 
        // because if it doesn't, then we won't simply generate an event. 
        // Though it might be the problem when I have multiple controllers, because how to determine the ownder of the event 
        // being sent? So, I will have to investigate that.
        
        // TODO(oleksii): Now, GAMEPAD_MAX_COUNT is equal to XUSER_MAX_COUNT, which is the maximum amount of 
        // controllers currently supported by XInput. But if we increase the number of user controllers, 
        // we will have to rewrite this for loop, and iterate over all the user controllers first, 
        // and check whether they are connected or not.
        for(U32 index = 0; 
            index < XUSER_MAX_COUNT; 
            index += 1){
            XINPUT_STATE controller_state = {};
            if(XInputGetState(index, &controller_state) == ERROR_SUCCESS){
                XINPUT_GAMEPAD *gamepad = &controller_state.Gamepad;
                I32 button = ((gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP) | 
                              (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_START) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_BACK) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) |
                              (gamepad->wButtons & XINPUT_GAMEPAD_A) | 
                              (gamepad->wButtons & XINPUT_GAMEPAD_B) | 
                              (gamepad->wButtons & XINPUT_GAMEPAD_X) | 
                              (gamepad->wButtons & XINPUT_GAMEPAD_Y));
                if(button){
                    Event newevent = {};
                    newevent.kind = EventKind_GamepadButtonPress;
                    newevent.button = button;
                    enqueue_event(&platform->frame_arena, platform->event_queue, newevent);
                }
                
                if((gamepad->sThumbLX < -L_STICK_DEADZONE || gamepad->sThumbLX > L_STICK_DEADZONE) || 
                   (gamepad->sThumbLY < -L_STICK_DEADZONE || gamepad->sThumbLY > L_STICK_DEADZONE)){
                    Event newevent = {};
                    newevent.kind = EventKind_GamepadStickMove;
                    newevent.lstick = V2f((F32)gamepad->sThumbLX, (F32)gamepad->sThumbLY);
                    newevent.lstick.x /= ((gamepad->sThumbLX < 0) ? 32768.0f : 32767.0f);
                    newevent.lstick.y /= ((gamepad->sThumbLY < 0) ? 32768.0f : 32767.0f);
                    enqueue_event(&platform->frame_arena, platform->event_queue, newevent);
                }
                if((gamepad->sThumbRX < -R_STICK_DEADZONE || gamepad->sThumbRX > R_STICK_DEADZONE) ||
                   (gamepad->sThumbRY < -R_STICK_DEADZONE || gamepad->sThumbRY > R_STICK_DEADZONE)){
                    Event newevent = {};
                    newevent.kind = EventKind_GamepadStickMove;
                    newevent.rstick = V2f((F32)gamepad->sThumbRX, (F32)gamepad->sThumbRY);
                    newevent.rstick.x /= ((gamepad->sThumbRX < 0) ? 32768.0f : 32767.0f);
                    newevent.rstick.y /= ((gamepad->sThumbRY < 0) ? 32768.0f : 32767.0f);
                    enqueue_event(&platform->frame_arena, platform->event_queue, newevent);
                }
                
                if(gamepad->bLeftTrigger > TRIGGER_THRESHOLD){
                    Event newevent = {};
                    newevent.kind = EventKind_GamepadTrigger;
                    newevent.ltrigger = (F32)gamepad->bLeftTrigger;
                    enqueue_event(&platform->frame_arena, platform->event_queue, newevent);
                }
                if(gamepad->bRightTrigger > TRIGGER_THRESHOLD){
                    Event newevent = {};
                    newevent.kind = EventKind_GamepadTrigger;
                    newevent.rtrigger = (F32)gamepad->bRightTrigger;
                    enqueue_event(&platform->frame_arena, platform->event_queue, newevent);
                }
            }
            else{}
        }
        
        // TODO(oleksii): We don't need to recompute it all the time. Only when the window was resized, 
        // but the problem is that we cannot handle WM_SIZE message here, we have to do it inside WindowProc
        V2i wnd = w32_get_window_metrics(window);
        global_platform.wnd_width = wnd.width;
        global_platform.wnd_height = wnd.height;
        
        // TODO(oleksii): Figure out what's better, to maintain a global pointer to the platform, 
        // or pass the platform struct to appcode.update() and appcode.render() routines!
        appcode.update(&global_platform);
        appcode.render(&global_platform);
        
        SwapBuffers(hdc);
        temp_arena_end(&tmp_arena);
    }
    ReleaseDC(window, hdc);
    CoUninitialize();
    return(0);
}
