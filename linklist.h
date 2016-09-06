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

#ifndef __LINK_LIST_INCLUDE__
#define __LINK_LIST_INCLUDE__

/* Link Data Types */
enum {
   LINK_DATA_TYPE_ULONG = 0,
   LINK_DATA_TYPE_DOUBLE, 
   LINK_DATA_TYPE_PTR,
   LINK_DATA_TYPE_VARIANT
};

/* Link List Types */
enum {
	LINK_LIST_REGULAR=0,
	LINK_LIST_QUEUE
};

typedef void ( * pfn_link_rundown_t )( void * p );
typedef int  ( * pfn_link_compare_t )( void * p1, void * p2 );

typedef struct link_node
{
   struct link_node * next;
   struct link_node * prev;
   union
   {
	  double         d;
	  unsigned long ul;
	  void *         p;
   }u;
} link_node_t;

typedef struct link_list
{
#ifdef __DEBUG__
	char   name[ 256 ];
#endif
	link_node_t *   head;
	link_node_t *   tail;
	link_node_t *   first;
	unsigned int  list_type;
	unsigned int  data_size;
	unsigned int  count;
	int           data_type;
	int           is_end_reached;
	pfn_link_rundown_t pfn_rundown;
	pfn_link_compare_t pfn_compare_link;
}link_list_t;


link_list_t * link_list_create
( 
#ifdef __DEBUG__
    char * name,
#endif
  unsigned int link_type,
	unsigned int data_type,
	unsigned int data_size,
	pfn_link_compare_t pfn_link_compare,
	pfn_link_rundown_t pfn_rundown
);

unsigned int link_list_get_type
( 
	link_list_t * list 
);

unsigned int link_list_get_data_size
( 
	link_list_t * list 
);

void link_list_destroy
( 
	link_list_t * list 
);

void link_list_add_data
( 
	link_list_t * list,
	void * p
);

void * link_list_find_data
( 
	link_list_t *list,
	void * key
);

void link_list_delete_data
( 
	link_list_t * list,
	void * data
);

int link_list_set_data
(
	link_list_t * list,
	link_node_t * node,
	void * p
);

int  link_list_compare_data
(	
	link_list_t * list, 
	link_node_t * node,
	void * p
);

void * link_list_compare_data_with_key
(	
	link_list_t * list, 
	link_node_t * node,
	void * p
);

int link_list_get_next_data_ex
(
	link_list_t * list,
	void * p 
);

int link_list_get_first_data
(
	link_list_t * list,
	void * p
);

int link_list_get_next_data
(
	link_list_t * list,
	void * p
);

int link_list_get_data
(	
	link_list_t * list,
	link_node_t * node,
	void * p
);

void link_list_delete_node
( 
	link_list_t * list,
	link_node_t * node
);

void link_list_invoke_rundown
( 
	link_list_t * list,
	link_node_t * node
);

int link_list_move_node( link_list_t * from_list, link_list_t * to_list, void * key);


#endif  /* __LINK_LIST_INCLUDE__ */
