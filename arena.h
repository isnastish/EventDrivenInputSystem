/* date = December 28th 2022 10:59 am */
#ifndef ARENA_H

struct Arena{
    void *base;
    Umm max;
    Umm pos;
    Umm commited;
    Umm align;
    I32 ntemp; // amount of temp arenas
};

struct TempArena{
    Arena *arena;
    Umm pos;
};

#define ArenaReserveSize (8ull*(1ull << 30)) // 8GB
#define ArenaCommitSize  (1ull << 20)        // 1MB

#define push_array(arena, type, count) (type *)arena_push((arena), sizeof(type)*(count))
#define push_struct(arena, type) (type *)arena_push((arena), sizeof(type))
#define push_size(arena, count) arena_push(arena, sizeof(U8)*(count))
#define zero_chunk(arena, type, count) arena_zero_chunk(arena, sizeof(type)*count)

function Arena arena_make(U64 size);
function Arena arena_make_align(U64 size, U64 alignment);
function void    *arena_push(Arena *arena, U64 size);
function void     arena_zero_chunk(Arena *arena, U64 chunk_size);
function void     arena_clear(Arena *arena);

function TempArena temp_arena_start(Arena *arena);
function void      temp_arena_end(const TempArena *temp);

#define ARENA_H
#endif //ARENA_H
