#define CHUNK_SHIFT 4                 //< Смещение чанка (степень 2)
#define CHUNK_SIZE (1 << CHUNK_SHIFT) //< Размер чанка (16)

#define GRID_AT(grid, x, y) ((grid)[(y) * CHUNK_SIZE + (x)])        //< Обращение к одномерному массиву как к двумерному
#define CALLOC_GRID(size) (calloc(CHUNK_SIZE * CHUNK_SIZE, (size))) //< Выделение памяти под сетку
// Free писать не буду. Там нечего писать
