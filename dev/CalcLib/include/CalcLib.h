#pragma once

#ifdef CALCLIB_EXPORTS
#define CALCLIB_API __declspec(dllexport)
#else
#define CALCLIB_API __declspec(dllimport)
#endif

extern "C" {
    CALCLIB_API int GetMagneticLine(float* buffer);
    CALCLIB_API int GetPolarCapNorthOpened(float* buffer);
    CALCLIB_API int GetPolarCapNorthClosed(float* buffer);
    CALCLIB_API int GetPolarCapSouthOpened(float* buffer);
    CALCLIB_API int GetPolarCapSouthClosed(float* buffer);
    CALCLIB_API int GetSkyMap(float* buffer);
    CALCLIB_API int GetPulseProfile(float* buffer, bool normalize);
}
