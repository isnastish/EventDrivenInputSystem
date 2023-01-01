#include "arena.h"

function Arena arena_make_align(U64 size, U64 alignment){
    Arena result = {};
    result.base = platform->reserve_mem(size);
    result.max = (Umm)size;
    result.align = alignment;
    return(result);
}

function Arena arena_make(U64 size){
    return(arena_make_align(size, 4u));
}

function void *arena_push(Arena *arena, Umm size){
    void *result = 0;
    if((arena->pos + size) <= arena->max){
        U8 *base = (U8 *)arena->base;
        Umm align_offset = 0;
        Umm align_mask = (arena->align - 1);
        Umm new_size = (arena->pos + size);
        if(new_size & align_mask){
            align_offset = arena->align - (new_size & align_mask);
            new_size += align_offset;
        }
        result = base + arena->pos;
        arena->pos = new_size;
        Umm new_size_clamped = (arena->max < new_size) ? arena->max : new_size;
        if(new_size_clamped > arena->commited){
            void *cmt_ptr = (base + arena->commited);
            Umm cmt_amount = new_size_clamped - arena->commited;
            Umm cmt_align_offset = 0;
            Umm cmt_align_mask = (ArenaCommitSize - 1);
            if(cmt_amount & cmt_align_mask){
                cmt_align_offset = ArenaCommitSize - (cmt_amount & cmt_align_mask);
                cmt_amount += cmt_align_offset;
            }
            platform->commit_mem(cmt_ptr, cmt_amount);
            arena->commited += cmt_amount;
        }
    }
    return(result);
}

function void arena_zero_chunk(Arena *arena, U64 chunk_size){
    if((arena->pos + chunk_size) <= arena->commited){
        U8 *ptr = (U8 *)(arena->base) + arena->pos;
        ZeroMem(ptr, 0, chunk_size); // TODO(oleksii): Expose this macro
    }
}

function void arena_clear(Arena *arena){
    platform->release_mem(arena->base);
    arena->commited = arena->pos = arena->max = 0;
    arena->base = 0;
}

function TempArena temp_arena_start(Arena *arena){
    TempArena result;
    result.arena = arena;
    result.pos = arena->pos;
    arena->ntemp += 1;
    return(result);
}

function void temp_arena_end(const TempArena *temp){
    Arena *arena = temp->arena;
    Assert(arena->pos >= temp->pos);
    U64 chunk_size = (arena->pos - temp->pos);
    arena->pos = temp->pos;
    arena->ntemp -= 1;
    arena_zero_chunk(arena, chunk_size);
}
