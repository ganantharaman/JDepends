/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/
#include <jvmmcros.h>

/** CHECK,CHECK whether it matches the code **/

JVM_BYTE *_aajbExcToStr[] = { 
	  			"No Exception",
				"Low on memory",
	            "Class Not Present",
	            "Class Incorrect Syntax",
                "Class General Error",
				"ArithmeticException",
	         	"ArrayStoreException",
                "ClassCastException",
				"IllegalMonitorStateException",
	            "IndexOutOfBoundsException",
	            "NegativeArraySizeException",
	            "NullPointerException",
	            "SecurityException",
	            "ClassFormatError",
				"ClassCircularityError",
				"NoClassDefFoundError",
				"StackOverflowError",
				"InstantiationException",
				"VirtualMachineError"
				"AllExceptions"
};


JVM_BYTE *_aajbErrorMessage[] = {
	 "7",
	 "Successful",
	 "Parsing Error : Incorrect Method Descriptor",
	 "Parsing Error : Error in Return type",
	 "Parsing Error : Error in Parameter",
     "Parsing Error : Object type name too long",
	 "Parsing Error : Incorrect Field Descriptor",
	 "OS Error" 
};
