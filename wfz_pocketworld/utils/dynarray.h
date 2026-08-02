#pragma once
#include <stdlib.h>
#include <stdint.h>

#define DEFINE_DYNARRAY(Type, prefix)                                        \
    typedef struct                                                           \
    {                                                                        \
        Type *items;                                                         \
        uint32_t count;                                                      \
        uint32_t capacity;                                                   \
    } prefix##Array;                                                         \
                                                                             \
    static inline int prefix##Array_Init(prefix##Array *arr)                 \
    {                                                                        \
        arr->capacity = 4;                                                   \
        arr->count = 0;                                                      \
        arr->items = (Type *)malloc(sizeof(Type) * arr->capacity);           \
        return arr->items ? 0 : -1;                                          \
    }                                                                        \
                                                                             \
    static inline int prefix##Array_Add(prefix##Array *arr, Type item)       \
    {                                                                        \
        if (arr->count >= arr->capacity)                                     \
        {                                                                    \
            uint32_t new_cap = arr->capacity * 2;                            \
            Type *tmp = (Type *)realloc(arr->items, sizeof(Type) * new_cap); \
            if (!tmp)                                                        \
                return -1;                                                   \
            arr->items = tmp;                                                \
            arr->capacity = new_cap;                                         \
        }                                                                    \
        arr->items[arr->count++] = item;                                     \
        return 0;                                                            \
    }                                                                        \
                                                                             \
    static inline int prefix##Array_Remove(prefix##Array *arr, uint32_t idx) \
    {                                                                        \
        if (idx >= arr->count)                                               \
            return -1;                                                       \
        arr->items[idx] = arr->items[--arr->count];                          \
        return 0;                                                            \
    }                                                                        \
                                                                             \
    static inline void prefix##Array_Free(prefix##Array *arr)                \
    {                                                                        \
        free(arr->items);                                                    \
        arr->items = NULL;                                                   \
        arr->count = arr->capacity = 0;                                      \
    }
