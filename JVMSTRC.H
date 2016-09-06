
#include <jvmmcros.h>

typedef struct _JVM_Class
{
  JVM_PBYTE jpbClassInfo;
 
}_JVM_Class;

typedef struct _JVM_Class_Loader
{
  _JVM_Class *pjcClLdrClass;

}_JVM_Class_Loader;

typedef struct _JVM_Constant_Pool
{
 
}_JVM_Constant_Pool;

typedef struct _JVM_Class_Map 
{
  _JVM_Class * pjcClass;
  _JVM_ClassLoader *pjclClassLoader;

} _JVM_Class_Map;