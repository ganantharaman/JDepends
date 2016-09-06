
#ifndef __JVM_MESG_INCLUDE__
#define __JVM_MESG_INCLUDE__

#define  JVM_POST_THREADMESSAGE( thread, type, value )\
	JvmPostThreadMessage_##type( thread, value );
  

typedef struct _JVM_Message
{
	union {
		JVM_CHAR   _jt_c;
		JVM_BOOL   _jt_b;
		JVM_LONG   _jt_l;
		JVM_DOUBLE _jt_d;
		JVM_INT    _jt_i;
		JVM_PVOID _jt_pv;
	};
   JVM_INT _jt_data;  
   JVM_THREAD _jt_orig_thread;
   JVM_THREAD _jt_dest_thread;
}_JVM_Message;


_JVM_Message * JvmCreateMessage(
);

JVM_BOOL  JvmPostThreadMessage_int(
IN  JVM_STRING    jsThreadName,
IN  JVM_INT       jiValue
);

JVM_BOOL  JvmPostThreadMessage_long(
IN  JVM_STRING    jsThreadName,
IN  JVM_LONG       jlValue
);

JVM_BOOL  JvmPostThreadMessage_ptr(
IN  JVM_STRING      jsThreadName,
IN  JVM_PVOID        pjvValue
);

#endif /** __JVM_MESG_INCLUDE__ **/
