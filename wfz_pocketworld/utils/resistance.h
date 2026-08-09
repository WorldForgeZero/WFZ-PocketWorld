#pragma once

#include <cstdint>

struct Resistance
{
    uint8_t gas = 0;
    uint8_t mana = 0;
    uint8_t rad = 0;
    uint8_t light = 0;

    // Сеттеры
    void SetGas(uint8_t val) { gas = (val > 100) ? 100 : val; }
    void SetMana(uint8_t val) { mana = (val > 100) ? 100 : val; }
    void SetRad(uint8_t val) { rad = (val > 100) ? 100 : val; }
    void SetLight(uint8_t val) { light = (val > 100) ? 100 : val; }

    // Геттеры
    uint8_t GetGas() const { return gas; }
    uint8_t GetMana() const { return mana; }
    uint8_t GetRad() const { return rad; }
    uint8_t GetLight() const { return light; }
};
