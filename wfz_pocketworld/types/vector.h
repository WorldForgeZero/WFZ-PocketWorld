#pragma once

#include <stdint.h>

typedef struct Vector
{
    int32_t x;
    int32_t y;
} Vector;

/// @brief Создать вектор из компонентов
static inline Vector Vec(int32_t x, int32_t y)
{
    Vector v = {x, y};
    return v;
}

/// @brief Сложение векторов
static inline Vector VecAdd(Vector a, Vector b)
{
    return Vec(a.x + b.x, a.y + b.y);
}

/// @brief Вычитание векторов
static inline Vector VecSub(Vector a, Vector b)
{
    return Vec(a.x - b.x, a.y - b.y);
}

/// @brief Умножение на скаляр (с сохранением знака)
static inline Vector VecScale(Vector v, int32_t scale)
{
    return Vec(v.x * scale, v.y * scale);
}

/// @brief Целочисленное деление на скаляр (с отбрасыванием остатка)
static inline Vector VecDiv(Vector v, int32_t div)
{
    return Vec(v.x / div, v.y / div);
}

/// @brief Квадрат длины вектора (безопасен от переполнения для разумных скоростей)
static inline int64_t VecLengthSq(Vector v)
{
    return (int64_t)v.x * v.x + (int64_t)v.y * v.y;
}

///

typedef struct ShortVector
{
    int16_t x;
    int16_t y;
} ShortVector;

/// @brief Создать вектор из компонентов
static inline ShortVector ShortVec(int16_t x, int16_t y)
{
    ShortVector v = {x, y};
    return v;
}

/// @brief Сложение векторов
static inline ShortVector ShortVecAdd(ShortVector a, ShortVector b)
{
    return ShortVec(a.x + b.x, a.y + b.y);
}

/// @brief Вычитание векторов
static inline ShortVector ShortVecSub(ShortVector a, ShortVector b)
{
    return ShortVec(a.x - b.x, a.y - b.y);
}

/// @brief Умножение на скаляр (с сохранением знака)
static inline ShortVector ShortVecScale(ShortVector v, int16_t scale)
{
    return ShortVec(v.x * scale, v.y * scale);
}

/// @brief Целочисленное деление на скаляр (с отбрасыванием остатка)
static inline ShortVector ShortVecDiv(ShortVector v, int16_t div)
{
    return ShortVec(v.x / div, v.y / div);
}

/// @brief Квадрат длины вектора (безопасен от переполнения для разумных скоростей)
static inline int32_t ShortVecLengthSq(ShortVector v)
{
    return (int32_t)v.x * v.x + (int32_t)v.y * v.y;
}
