// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "CalcLib.h"
#include "ModelGenerator.h"

#include <vector>
#include <iostream>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

int GetMagneticLine(float* buffer)
{
    int size(0);
    try {
        size = ModelGenerator::CalcManager::GetInstance().GetMagneticLine(buffer);
    }
    catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return size;
}

int GetMagneticLineVerticesCount()
{
    bool size(0);
    try {
        size = ModelGenerator::CalcManager::GetInstance().GetMagneticLineVerticesCount();
    }
    catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return size;
}

bool GetMagneticLineVertices(float* buffer)
{   
    bool result(false);
    try {
        ModelGenerator::CalcManager::GetInstance().GetMagneticLineVertices(buffer);
        result = true;
    }
    catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
    return result;
}
