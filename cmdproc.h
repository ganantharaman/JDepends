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

#ifndef __CMDPROC_HDR_INCLUDE__
#define __CMDPROC_HDR_INCLUDE__

#define IN 
#define OUT 


typedef struct cmd_config_info {
	char  * arg;
	void  * pv;
}cmd_config_info_t;


typedef struct arg_table {
	/* No more than 7 byte arg names */
	char arg_name[8]; 
	void (*pfn_arg_handler)(cmd_config_info_t *);
}arg_table_t;


/** Interface to World **/

int parse_command_line_arguments( 
IN     int argc,
IN     char ** argv,
IN     arg_table_t * arg_table,
IN     void * cfg_info,
OUT    int *  status
);


#endif /** __CMDPROC_HDR_INCLUDE__ **/