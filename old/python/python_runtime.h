#pragma once

#include <string>

namespace python_runtime
{
    /// @brief Инициализатор python
    /// @param scriptDir Путь до файла с exe сервера
    /// @return Возвращает true при успехе
    bool Init(const std::string &scriptDir);

    /// @brief Выполнить python файл
    /// @param path Путь до python файла
    /// @return Возвращает true при успехе
    bool RunFile(const std::string &path);

    /// @brief Финализатор
    void Shutdown();
}
