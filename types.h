/* date = December 26th 2022 0:45 pm */
#ifndef TYPES_H
#include <stdio.h>
#include <stdint.h>
#include <memory.h>

#define Assert(expr)\
if((expr)){}\
else{\
*(I32 *)0 = 0x1;\
}
#define ZeroMem(s, c, n) memset(s, c, n) 
#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define global static
#define function static
#define local_persist static

typedef int8_t    I8;
typedef int16_t   I16;
typedef int32_t   I32;
typedef int64_t   I64;
typedef uint8_t   U8;
typedef uint16_t  U16;
typedef uint32_t  U32;
typedef uint64_t  U64;
typedef uintptr_t Umm;
typedef float     F32;
typedef double    F64;
typedef I32       B32;
typedef B32       B32x;

#define Kilobytes(n) ((n)*(1024llu))
#define Megabtyes(n) (Kilobytes(n)*(1024llu))
#define Gigabytes(n) (Megabtyes(n)*(1024llu))
#define Terabytes(n) (Gigabytes(n)*(1024llu))

// TODO(oleksii): Maybe, a better approach would be to create a separate union for V2i, and another one for V2f. 
// Will see how it affects the compilation time in the future when we have tons computations.
template <typename T>
union V2{
    struct{
        T x;
        T y;
    };
    struct{
        T width;
        T height;
    };
    T e[2];
    
    V2(): x(0), y(0){}
    V2(T _x, T _y): x(_x), y(_y){}
    friend V2<T> operator-(const V2<T>& a, const V2<T>& b){
        V2<T> result;
        result.x = a.x-b.x;
        result.y = a.y-b.y;
        return(result);
    }
    friend V2<T> operator+(const V2<T>& a, const V2<T>& b){
        V2<T> result;
        result.x = a.x+b.x;
        result.y = a.y+b.y;
        return(result);
    }
    B32 operator!=(const F32 v){
        B32 result = ((x != v) && (y != v));
        return(result);
    }
};
typedef V2<I32> V2i;
typedef V2<F32> V2f;

template<typename T>
union V3{
    struct{
        T x, y, z;
    };
    struct{
        T r, g, b;
    };
    T e[3];
    V3(): x(0), y(0), z(0){}
    V3(T _x, T _y, T _z):x(_x), y(_y), z(_z){}
};
typedef V3<I32> V3i;
typedef V3<F32> V3f;

template<typename T>
union V4{
    struct{
        T x, y, z, w;
    };
    struct{
        T r, g, b, a;
    };
    T e[4];
    V4(): x(0), y(0), z(0), w(0){}
    V4(T _x, T _y, T _z, T _w):x(_x), y(_y), z(_z), w(_w){}
};
typedef V4<I32> V4i;
typedef V4<F32> V4f;

////////////////////////////////
// NOTE(oleksii): String8 
struct String8{
    U8 *data;
    U64 size;
};

static String8 make_string8(U8 *str, U64 size){
    String8 result;
    result.data = str;
    result.size = size;
    return(result);
}

#define S8Literal(s) make_string8((U8 *)s, (sizeof(s) - 1))
#define S8LiteralComp(s) {(U8 *)s, (sizeof(s) - 1)}

// TODO(oleksii): Revisit and make it fit our purposes.
#define DLLInit(sentinel)\
(sentinel)->next = (sentinel);\
(sentinel)->prev = (sentinel);

#define DLLFront(sentinel) (sentinel->next)
#define DLLBack(sentinel) (sentinel->prev)

#define DLLPushFront(sentinel, node)\
{\
if(!(sentinel->next) && !(sentinel->prev)){\
(sentinel)->next = (sentinel)->prev = (sentinel);\
}\
(node)->next = (sentinel)->next;\
(node)->prev = (sentinel);\
(sentinel)->next->prev = (node);\
(sentinel)->next = (node);\
}
#define DLLPushBack(sentinel, node)\
{\
if(!(sentinel->next) && !(sentinel->prev)){\
(sentinel)->next = (sentinel)->prev = (sentinel);\
}\
(node)->next = (sentinel);\
(node)->prev = (sentinel)->prev;\
(sentinel)->prev->next = (node);\
}
#define DLLPopFront(sentinel)\
if((sentinel)->next != (sentinel)){\
(sentinel)->next->next->prev = (sentinel);\
(sentinel)->next = (sentinel)->next->next;\
}
#define DLLPopBack(sentinel)\
if((sentinel)->next != (sentinel)){\
(sentinel)->prev->prev->next = (sentinel);\
(sentinel)->prev = (sentinel)->prev->prev;\
}
#define DLLPopNode(sentinel, node)\
{\
(node)->next->prev = (node)->prev;\
(node)->prev->next = (node)->next;\
}

// Queue implementation using DLL.
#define QueueInit(sentinel)       DLLInit(sentinel)
#define QueuePush(sentinel, node) DLLPushFront(sentinel, node)
#define QueuePop(sentinel)        DLLPopBack(sentinel)
#define QueueGet(sentinel)        DLLBack(sentinel)

// Stack implementaion using DLL.
#define StackInit(sentinel)       DLLInit(sentinel)
#define StackPush(sentinel, node) DLLPushFront(sentinel, node)
#define StackPop(sentinel)        DLLPopFront(sentinel)
#define StackGet(sentinel)        DLLFront(sentinel);

#define TYPES_H
#endif //TYPES_H
