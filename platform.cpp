#include "platform.h"

#if defined(_WIN32)
#include "slim_windows.h"
#include <gl/gl.h>
#endif

// NOTE(oleksii): Should use extern "C" modifier only when writing C++ code, not straight C code.
// Because normally, C++ compiler mangles function and variable names.
// So, using extern "C" tells the compiler not to mangle the variable of runction names.
#define DLLEXPORT extern "C" __declspec(dllexport)

function inline B32 iskind(Event *event, I32 kind){
    return(event->kind == kind);
}

function inline B32 cursor_in_rect(V2f cursor, V2f minp, V2f maxp){
    B32 result = ((cursor.x >= minp.x) &&
                  (cursor.x <= maxp.x) &&
                  (cursor.y >= minp.y) &&
                  (cursor.y <= maxp.y));
    return(result);
}

function void gl_draw_rect(V2f minp, V2f maxp, V4f color){
    glBegin(GL_TRIANGLES);
    glColor4f(color.r, color.g, color.b, color.a);
    // lower triangle
    glVertex2f(minp.x, minp.y);
    glVertex2f(maxp.x, minp.y);
    glVertex2f(maxp.x, maxp.y);
    // upper triangle
    glVertex2f(minp.x, minp.y);
    glVertex2f(maxp.x, maxp.y);
    glVertex2f(minp.x, maxp.y);
    glEnd();
}

function inline Event *dequeue_event(Event *sentinel){
    Event *result = QueueGet(sentinel);
    QueuePop(sentinel);
    return(result);
}

DLLEXPORT APP_UPDATE(app_update){
    platform = _platform;
    AppState *appstate = (AppState *)platform->arena.base;
    
    // TODO(oleksii): There is not reason to recompute the ratio all the time, only when the size of the window has changed
    // So, probably we can enqueue an event when the window has been resized with the new size of the window, 
    // and only in that case we will have to recompute the aspect ratio. But for now let it be here, not a big deal!
    // WM_SIZE is received through the WindowProc, so to wrap it into the event, we will have to maintain the global_platform variable, or
    // we will have to extract the user data (Platform) inside WindowProc routine
    appstate->ratio = (F32)platform->wnd_width/(F32)platform->wnd_height;
    if(!appstate->initialized){
        appstate->initialized = true;
        appstate->rect0 = ui_make_rect(V2f(0, -0.5f), V2f(0.5f, 0.3f), V4f(0.0f, 1.0f, 0.45f, 0.0f));
    }
    
    for(Event *event = dequeue_event(platform->event_queue);
        event != platform->event_queue;
        event = dequeue_event(platform->event_queue)){
        if(iskind(event, EventKind_MouseMove)){
            appstate->cursor.x = (2.0f*event->pos.x/platform->wnd_width) - 1.0f;
            appstate->cursor.y = (2.0f*(-event->pos.y)/platform->wnd_height) + 1.0f;
            if((event->modifier & ButtonModifier_Left) && 
               (event->modifier & ButtonModifier_Ctrl)){
                if(!appstate->rect0.move && 
                   cursor_in_rect(appstate->cursor, appstate->rect0.minp, appstate->rect0.maxp)){
                    // NOTE(oleksii): compute the distance from the cursor to min and max points of the rectangle
                    // The distance has to be computed once and stored in the AppState struct.
                    appstate->rect0.dist2minp = (appstate->cursor - appstate->rect0.minp);
                    appstate->rect0.dist2maxp = (appstate->rect0.maxp - appstate->cursor);
                    
                    appstate->rect0.move = 1;
                }
                if(appstate->rect0.move){
                    appstate->rect0.minp = (appstate->cursor - appstate->rect0.dist2minp);
                    appstate->rect0.maxp = (appstate->cursor + appstate->rect0.dist2maxp);
                }
            }
        } 
        else if(iskind(event, EventKind_MouseButtonPress)){
            if(event->button == MouseButton_Left){}
            else if(event->button == MouseButton_Right){}
            else if(event->button == MouseButton_Middle){}
            else if(event->button == MouseButton_X1){} 
            else if(event->button == MouseButton_X2){}
        }
        else if(iskind(event, EventKind_MouseButtonRelease)){
            if(event->button == MouseButton_Left){
                if(appstate->rect0.move){
                    appstate->rect0.move = 0;
                }
            }
            else if(event->button == MouseButton_Right){}
            else if(event->button == MouseButton_Middle){}
            else if(event->button == MouseButton_X1){} 
            else if(event->button == MouseButton_X2){}
        }
        else if(iskind(event, EventKind_MouseScroll)){
            if(event->button == MouseButton_VWheel){
                if(event->modifier & ButtonModifier_Ctrl){}
                if(event->modifier & ButtonModifier_Shift){}
            }
            else if(event->button == MouseButton_HWheel){
                if(event->modifier & ButtonModifier_Ctrl){}
                if(event->modifier & ButtonModifier_Shift){}
            }
        }
        else if(iskind(event, EventKind_GamepadButtonPress)){
            if(event->button & GamepadButton_Start){}
            if(event->button & GamepadButton_Back){}
            if(event->button & GamepadButton_LeftThumb){}
            if(event->button & GamepadButton_RightThumb){} 
            if(event->button & GamepadButton_LeftShoulder){}
            if(event->button & GamepadButton_RightShoulder){}
            if((event->button & GamepadButton_A) || 
               (event->button & GamepadButton_DpadDown)){
                appstate->rect0.minp.y -= 0.02f;
                appstate->rect0.maxp.y -= 0.02f;
            }
            if((event->button & GamepadButton_B) ||
               (event->button & GamepadButton_DpadRight)){
                appstate->rect0.minp.x += 0.02f;
                appstate->rect0.maxp.x += 0.02f;
            }
            if((event->button & GamepadButton_X) || 
               (event->button & GamepadButton_DpadLeft)){
                appstate->rect0.minp.x -= 0.02f;
                appstate->rect0.maxp.x -= 0.02f;
            }
            if((event->button & GamepadButton_Y) ||
               (event->button & GamepadButton_DpadUp)){
                appstate->rect0.minp.y += 0.02f;
                appstate->rect0.maxp.y += 0.02f;
            }
        }
        else if(iskind(event, EventKind_GamepadTrigger)){
            if(event->ltrigger){
                appstate->rect0.minp.y -= event->ltrigger /  255.0f / 50.0f;
                appstate->rect0.maxp.y -= event->ltrigger /  255.0f / 50.0f;
            }
            else{
                appstate->rect0.minp.y += event->rtrigger /  255.0f / 50.0f;
                appstate->rect0.maxp.y += event->rtrigger /  255.0f / 50.0f;
            }
        }
        else if(iskind(event, EventKind_GamepadStickMove)){
            if(event->lstick.x || event->lstick.y){
                appstate->rect0.minp.x += event->lstick.x / 50.0f;
                appstate->rect0.minp.y += event->lstick.y / 50.0f;
                appstate->rect0.maxp.x += event->lstick.x / 50.0f;
                appstate->rect0.maxp.y += event->lstick.y / 50.0f;
            }
            else{
                appstate->rect0.minp.x += event->rstick.x / 50.0f;
                appstate->rect0.minp.y += event->rstick.y / 50.0f;
                appstate->rect0.maxp.x += event->rstick.x / 50.0f;
                appstate->rect0.maxp.y += event->rstick.y / 50.0f;
            }
        }
        else if(iskind(event, EventKind_KeyPress)){
            char buf[256];
            if(event->button >= KbdButton_A && event->button <= KbdButton_Z){
                if(event->modifier & ButtonModifier_Shift)
                    sprintf_s(buf, sizeof(buf), "%c\n", (U8)('A' + event->button-KbdButton_A));
                else
                    sprintf_s(buf, sizeof(buf), "%c\n", (U8)('a' + event->button-KbdButton_A));
                OutputDebugStringA((LPCSTR)buf);
            }
            else if(event->button >= KbdButton_0 && event->button <= KbdButton_9){
                if(event->modifier & ButtonModifier_Shift)
                    switch(event->button){
                    case KbdButton_0:{OutputDebugStringA(")\n");}break; 
                    case KbdButton_1:{OutputDebugStringA("!\n");}break;
                    case KbdButton_2:{OutputDebugStringA("@\n");}break;
                    case KbdButton_3:{OutputDebugStringA("#\n");}break;
                    case KbdButton_4:{OutputDebugStringA("$\n");}break; 
                    case KbdButton_5:{OutputDebugStringA("%\n");}break;
                    case KbdButton_6:{OutputDebugStringA("^\n");}break; 
                    case KbdButton_7:{OutputDebugStringA("&\n");}break;
                    case KbdButton_8:{OutputDebugStringA("*\n");}break;
                    case KbdButton_9:{OutputDebugStringA("(\n");}break;
                }
                else{
                    sprintf_s(buf, sizeof(buf), "%s\n", keymap[event->button].name.data);
                    OutputDebugStringA((LPCSTR)buf);
                }
            }
            else if((event->button >= KbdButton_Minus && event->button <= KbdButton_Tilde) ||
                    (event->button >= KbdButton_Left && event->button <= KbdButton_Escape) ||
                    (event->button >= KbdButton_F1 && event->button <= KbdButton_F12)){
                sprintf_s(buf, sizeof(buf), "%s\n", keymap[event->button].name.data);
                OutputDebugStringA((LPCSTR)buf);
            }
        }
        else if(iskind(event, EventKind_KeyRelease)){}
    }
}

DLLEXPORT APP_RENDER(app_render){
    platform = _platform;
    AppState *appstate = (AppState *)platform->arena.base;
    
    glViewport(0, 0, platform->wnd_width, platform->wnd_height);
    glClearColor(22.0f / 255.0f, 22.0f / 255.0f, 22.0f / 255.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_draw_rect(appstate->rect0.minp, appstate->rect0.maxp, appstate->rect0.color);
}