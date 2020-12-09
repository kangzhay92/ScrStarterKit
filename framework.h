#pragma once

#include "targetver.h"

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// Disable min and max macros from Windows headers, since its
// very annoying. (STL library has inline min and max function,
// compilation may be failed if we not disable the macros) 
#define NOMINMAX             

// Windows Header Files
#include <Windows.h>
#include <CommCtrl.h>

// STL C++ Runtime Header Files
#include <string>
#include <vector>
#include <random>
#include <chrono>
