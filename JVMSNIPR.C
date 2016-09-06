#include <jvmutil.h>




JVM_STRING JvmSniperMessage( JVM_ULONG julMessageBit )
{
  return  _aajbSniperMessage[ JvmBitIndex( julMessageBit ) ];
}


