#ifndef WINDOWS_HPP
#define WINDOWS_HPP

#include <nil/environment.hpp>

#ifdef NIL_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define WIN32_MEAN_AND_LEAN
//the winsock include is required because including windows.h prior to winsock2.h and then including winsock2.h results in many redefinition problems
#include <winsock2.h>
#include <windows.h>

#endif

#endif
