#include <iostream>

#include "utils/logger.h"

int main(int argc, char *argv[])
{
    if (!wfz::logger::Init())
    {
        std::cerr << "Logger fail to init" << std::endl;
        return 1;
    }

    wfz::logger::Debug("fuck u world!");

    wfz::logger::Shutdown();
    return 0;
}
