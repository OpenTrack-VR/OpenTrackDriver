#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
  #define DRIVER_EXPORT __declspec(dllexport)
#else
  #define DRIVER_EXPORT __attribute__((visibility("default")))
#endif 