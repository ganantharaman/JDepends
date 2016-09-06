
#include <jvmmesg.h>

_JVM_Message * JvmCreateMessage()
{
	_JVM_Message * pjmMessage = JVM_NULL;
	pjmMessage = JvmMemCalloc( sizeof( _JVM_Message ));
	JVM_ALLOC_CHECK( pjmMessage )
	return pjmMessage;
}


JVM_BOOL JvmPostThreadMessage_int( 
 IN  JVM_STRING jsThreadName,
 IN  JVM_INT    jiValue 
)
{
   	_JVM_Message * pjmMessage = JvmCreateMessage();
	pjmMessage->_jt_data = 'i';
	pjmMessage->_jt_i = jiValue;
	pjmMessage->_jt_orig_thread = JvmSysGetCurrentThread();
	pjmMessage->_jt_dest_thread = JvmSysGetThreadFromName( jsThreadName );

	JvmSysPostThreadMessage( pjmMessage );
}
	
