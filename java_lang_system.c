#include <time.h>

_declspec(dllexport) void registerNatives() {
   
}

_declspec(dllexport) void IdentityHashCode() {

}

_declspec(dllexport) void InitProperties() {

}

_declspec(dllexport) void mapLibraryName() {

}

_declspec(dllexport) __int64 currentTimeMillis() {
  __time64_t ltime;
  ltime = _time64(NULL);
  return ltime * 1000;
}

_declspec(dllexport) void setIn0() {

}
_declspec(dllexport) void setOut0() {

}

_declspec(dllexport) void setErr0() {

}


