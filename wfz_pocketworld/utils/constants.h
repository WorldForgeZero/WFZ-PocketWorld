#pragma once

#define CHUNK_SHIFT 4                 //< Смещение чанка (степень 2)
#define CHUNK_SIZE (1 << CHUNK_SHIFT) //< Размер чанка (16)

#define VECTOR2D_FIXED_SCALE 4096 //< Для плавности обычного VECTOR2D

#define MAX_SOURCE_LIMIT 255 //< Лимит источника для света/радиации
// Пояснение для MAX_SOURCE_LIMIT для тех кто надумает его поднять. Не стоит.
// Данный лимит был использован чтобы просто блять не сдохнуть от количества проверок
// которые пришлось бы проводить для перерасчёта изменения источника света или же радиации.
// Будьте лапочками, НЕ ТРОГАЙТЕ ЕСЛИ НЕ ХОТИТЕ CPU В СОБСТВЕННОМ СОКУ!

// Стороны света (хуета)
#define DIR_NORTH 0     // ↑
#define DIR_NORTHEAST 1 // ↗
#define DIR_EAST 2      // →
#define DIR_SOUTHEAST 3 // ↘
#define DIR_SOUTH 4     // ↓
#define DIR_SOUTHWEST 5 // ↙
#define DIR_WEST 6      // ←
#define DIR_NORTHWEST 7 // ↖
