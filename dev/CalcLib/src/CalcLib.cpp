// DLL アプリケーションのエントリ ポイントを定義
#include "pch.h"
#include "CalcLib.h"
#include "CalcManager.h"

#include <vector>
#include <iostream>


BOOL APIENTRY DllMain(HMODULE hModule,
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


void setInclinationAngle(int degree)
{
    CalcLib::CalcManager::getInstance()
        .setInclinationAngle(degree);
}


int getMagneticLine(float* buffer)
{
    return CalcLib::CalcManager::getInstance()
        .getMagneticLine(buffer);
}


int getPolarCapNorthOpened(float* buffer)
{
    return CalcLib::CalcManager::getInstance()
        .getPolarCapNorthOpened(buffer);
}


int getPolarCapNorthClosed(float* buffer)
{
    return CalcLib::CalcManager::getInstance()
        .getPolarCapNorthClosed(buffer);
}


int getPolarCapSouthOpened(float* buffer)
{
    return CalcLib::CalcManager::getInstance()
        .getPolarCapSouthOpened(buffer);
}


int getPolarCapSouthClosed(float* buffer)
{
    return CalcLib::CalcManager::getInstance()
        .getPolarCapSouthClosed(buffer);
}


int getSkyMap(float* buffer)
{
    return CalcLib::CalcManager::getInstance()
        .getSkyMap(buffer);
}


int getPulseProfile(float* buffer, bool normalize)
{
    return CalcLib::CalcManager::getInstance()
        .getPulseProfile(buffer, normalize);
}

