#include <iostream>
#include "LogOnDesktop.h"

int main()
{

    LogOnDesktop log;

    for (size_t i = 0; i < 10; i++)
    {
        log.WriteLine(std::to_wstring(i));
    }










}

