#include <filesystem>
#include <iostream>

#include "python/python_runtime.h"

namespace fs = std::filesystem;

fs::path GetCurrentDirectory(char *argv[])
{
    fs::path exePath;
    try
    {
        exePath = fs::canonical("/proc/self/exe");
    }
    catch (...)
    {
        exePath = fs::path(argv[0]);
    }
    return exePath.parent_path();
}

int main(int argc, char *argv[])
{
    fs::path exeDir = GetCurrentDirectory(argv);

    if (!python_runtime::Init(exeDir.string()))
    {
        std::cerr << "Не удалось инициализировать Python" << std::endl;
        return 1;
    }

    fs::path autorunPath = exeDir / "main" / "autorun.py";
    if (fs::exists(autorunPath))
    {
        if (!python_runtime::RunFile(autorunPath.string()))
            return 1;
    }
    else
    {
        std::cout << "Файл /main/autorun.py не найден." << std::endl;
        return 1;
    }

    // TODO: Сделать запуск сервера вебсокетов

    python_runtime::Shutdown();
    return 0;
}
