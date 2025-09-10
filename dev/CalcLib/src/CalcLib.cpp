// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "CalcLib.h"
#include "CalcManager.h"

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
    return ModelGenerator::CalcManager::GetInstance()
        .GetMagneticLine(buffer);
}

int GetPolarCapNorthOpened(float* buffer)
{
    return ModelGenerator::CalcManager::GetInstance()
        .GetPolarCapNorthOpened(buffer);
}

int GetPolarCapNorthClosed(float* buffer)
{
    return ModelGenerator::CalcManager::GetInstance()
        .GetPolarCapNorthClosed(buffer);
}

int GetPolarCapSouthOpened(float* buffer)
{
    return ModelGenerator::CalcManager::GetInstance()
        .GetPolarCapSouthOpened(buffer);
}

int GetPolarCapSouthClosed(float* buffer)
{
    return ModelGenerator::CalcManager::GetInstance()
        .GetPolarCapSouthClosed(buffer);
}

int GetSkyMap(float* buffer)
{
    return ModelGenerator::CalcManager::GetInstance()
        .GetSkyMap(buffer);
}

int GetPulseProfile(float* buffer, bool normalize)
{
    return ModelGenerator::CalcManager::GetInstance()
        .GetPulseProfile(buffer, normalize);
}
