/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser General Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/

#include <cmnmcros.h>

#define STDIO_H
#define STDLIB_H
#define STRING_H
#include <stdhdrs.h>

#include <cmdproc.h>


int parse_command_line_arguments( 
IN     int argc,
IN     char ** argv,
IN     arg_table_t * arg_table,
IN     void * cfg_info,
OUT    int *  status
)
{
  /** Assumes argv[0] == <.exe> **/
  int arg_count = 1;
  int index = 0;
  int count;
  cmd_config_info_t table;
  assert( arg_table != NULL );

  /** First element is special */
  table.pv    = cfg_info;
  count = atoi( arg_table[0].arg_name );

  if ( argc <= 0 || NULL == argv )
   	 return 0;

  for ( ; arg_count < argc; arg_count ++) {
     table.arg = argv[ arg_count ];
     /** Known Argument with a preceding '-' or '/' -- Action : Call appropriate handler **/
     if ( argv[ arg_count ][0] == '-' || argv[ arg_count ][0] == '/')  {
       for ( index = 1; index <  count; index++ ) {
	        if ( !stricmp( &argv[ arg_count ][1], arg_table[index].arg_name ))  
           (* arg_table[index].pfn_arg_handler)(&table);
	     }

     /** Unknown argument with a preceding '-' -- Action : Call Default handler**/
       if ( index == count )
         (* arg_table[0].pfn_arg_handler)(&table);

     } else  /** Unknown argument without '/' or '-' : Action : Call Default handler **/
      (* arg_table[0].pfn_arg_handler)(&table);
  }
  return 1;
}

