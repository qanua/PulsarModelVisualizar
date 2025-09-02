#pragma once

#ifdef CALCLIB_EXPORTS
#define CALCLIB_API __declspec(dllexport)
#else
#define CALCLIB_API __declspec(dllimport)
#endif

extern "C" {
    CALCLIB_API int GetMagneticLine(float* buffer);
    CALCLIB_API int GetMagneticLineVerticesCount();
    CALCLIB_API bool GetMagneticLineVertices(float* buffer);
}
