#include <filesystem>
#include <iostream>

#include "python/python_runtime.h"
#include "utils/logger.h"

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
    (void)argc;

    Logger::instance().setLevel(LogLevel::Debug);

    fs::path exeDir = GetCurrentDirectory(argv);

    if (!python_runtime::Init(exeDir.string()))
    {
        LOG_ERROR("server.main", "Не удалось инициализировать Python");
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
        LOG_WARNING("server.main", "Файл /main/autorun.py не найден");
    }

    // TODO: Сделать запуск сервера вебсокетов

    python_runtime::Shutdown();
    return 0;
}
