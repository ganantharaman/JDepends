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

#include <mem.h>
#include <linklist.h>

#define STDLIB_H
#define ASSERT_H
#define STRING_H
#define STDIO_H
#include <stdhdrs.h>

#include <cmnmcros.h>


link_list_t * link_list_create(
#ifdef __DEBUG__
   char * pName,
#endif
   unsigned int list_type,
   unsigned int data_type, 
   unsigned int data_size,
   pfn_link_compare_t pfn_compare_link,
   pfn_link_rundown_t pfn_rundown 
)
{
   DECLARE_TRACE_DATA( "link_list_Create" )
   link_list_t * list;

   TRACE_PRINT(__FUNC__)
   
   if ( list = mem_alloc( sizeof( link_list_t)))
   {
#ifdef __DEBUG__
     strcpy( list->name, pName );
#endif
   list->first = list->head = list->tail  = NULL;
	 list->list_type = list_type;
	 list->data_type      = data_type;
	 list->data_size  = data_size ;
	 list->pfn_compare_link = pfn_compare_link;
	 list->pfn_rundown = pfn_rundown;
	 list->is_end_reached = 0;
	 list->count      = 0;
   }

   return list;
}


unsigned int link_list_get_type( link_list_t * list )
{
   DECLARE_TRACE_DATA( "link_list_get_type") 
   TRACE_PRINT(__FUNC__);
   assert( list != NULL );
   return  list->data_type;
}

unsigned int link_list_get_data_size( link_list_t * list )
{
   DECLARE_TRACE_DATA( "link_list_get_data_size" )
   TRACE_PRINT(__FUNC__);
   assert( list != NULL );
   return  list->data_size;
}

void link_list_destroy( link_list_t * list )
{
	DECLARE_TRACE_DATA("link_list_destroy")
	link_node_t * node, *next_node = NULL; 
	TRACE_PRINT(__FUNC__)
	assert( list != NULL );

	node = list->head;

	if ( node )
	{
		do
		{
			next_node = node->next;
 			link_list_delete_node ( list, node);
		}
		while ((node = next_node) != list->head );
	}

	mem_free( list );
}


void link_list_add_data( link_list_t * list, void * p)
{
  DECLARE_TRACE_DATA("link_list_add_data")
  link_node_t * node = NULL;
	TRACE_PRINT(__FUNC__);
	assert( list != NULL );

  if ((node = (link_node_t *) mem_alloc( sizeof( link_node_t ))))
	{	
   	node->next = node;
		node->prev = node;
		node->u.p = NULL;
	  link_list_set_data( list, node, p );
  }
  
  /* Only one node ? */
  if ( list->head && (list->head->next == list->head))
  {
      list->tail = node;
    	node->prev = list->head;
  }

	if ( list->head )
	{
    node->next        = list->head->next;
    node->next->prev  = node;
		node->prev        = list->head;
		list->head->next  = node; 
	}
	else
	  list->tail = list->head =  node;
   
	list->count ++;
}



void * link_list_find_data( link_list_t *list, void * key )
{
  DECLARE_TRACE_DATA("link_list_find_data")
	link_node_t * p = NULL;
	void * data;
	TRACE_PRINT(__FUNC__);
	assert( list != NULL);
	p = list->head;
  do
	{
		data = link_list_compare_data_with_key( list, p, key );
    if ( data ) return data;
		p = p->next;
	}
	while( p != list->head );

	return (void *)0;
}



void link_list_delete_data( link_list_t * list, void * data)
{
	DECLARE_TRACE_DATA("link_list_DeleteData")
	link_node_t * p, *q;
	TRACE_PRINT( __FUNC__ );
	assert( list != NULL );
	p = q = NULL;
	p = list->head;
	if ( !p ) return;
	do
	{
 	 	if ( link_list_compare_data( list, p, data ))
		{
			link_list_invoke_rundown( list, p );
			if ( p->u.p && (list->data_type == LINK_DATA_TYPE_VARIANT)
		        && (list->data_size > sizeof( unsigned long )))
				mem_free( p->u.p );
			p->u.ul = p->next->u.ul;
			p->u.d  = p->next->u.d;
			p->u.p  = p->next->u.p;
			q       = p->next;
			p->next = q->next;
			q->next->prev = p;

			if ( q == list->head )
				list->head = ( p == list->head ) ? NULL : p;
			if ( q == list->tail )
        list->tail = ( p == list->tail ) ? NULL : p;
			mem_free( q );
			break;
		}
		p = p->next;
	} while ( p != list->head );

	list->count --;
}

int link_list_get_first_data( link_list_t * list, void * p )
{
  DECLARE_TRACE_DATA("link_list_get_first_data")
  TRACE_PRINT( __FUNC__ );
  list->first = NULL;
	list->is_end_reached = 0;
	if ( list->list_type == LINK_LIST_QUEUE )
	{
	  if ( list->tail )
	     list->first = list->tail;
 	}
	else if ( list->head )
		list->first = list->head;

	if ( list->first )
       link_list_get_data( list, list->first, p );

	return !!(list->first); 
}


int link_list_get_next_data( link_list_t * list, void * p )
{
	DECLARE_TRACE_DATA("LinkListget_next_data")
	link_node_t * node = list->first;
	TRACE_PRINT( __FUNC__ );

	if ( !list->is_end_reached )
	{
		if ( node )
		{
			list->first = ( list->list_type == LINK_LIST_QUEUE ) ?
          				 node->prev : node->next;
			if ( list->first == list->head )
			{
				list->is_end_reached = 1;
				list->first = NULL;
			}
		}
		else if ( list->head )
		{
			if ( list->list_type == LINK_LIST_QUEUE )
			{
				if ( list->tail )
					list->first = list->tail;
			}
			else if ( list->head )
					list->first = list->head;
		}
	}

	return list->first ? link_list_get_data( list, list->first, p ) : 0;
}

int link_list_get_next_data_ex( link_list_t *list, void * p )
{
	DECLARE_TRACE_DATA("link_list_get_next_data_ex")
	link_node_t * node = NULL;

	TRACE_PRINT( __FUNC__ );
  if ( list->list_type == LINK_LIST_QUEUE )
	{
	  if ( list->tail )
	     node = list->tail;
 	}
	else if ( list->head )
		node = list->head;

	if ( node )
	  link_list_get_data( list, node, p );

	return !!node;
}


int link_list_set_data(link_list_t * list, link_node_t * node, void * p )
{
	DECLARE_TRACE_DATA("link_list_set_data")
    TRACE_PRINT( __FUNC__ );
	if ( !node || !p || !list ) return 0;

	switch ( list->data_type )
	{
	case LINK_DATA_TYPE_ULONG:
		node->u.ul = *(unsigned long *)p;
		break;
	case LINK_DATA_TYPE_DOUBLE: 
		node->u.d = *(double *)p;
		break;
    case LINK_DATA_TYPE_PTR:
		node->u.p = p;
		break;
	case LINK_DATA_TYPE_VARIANT: 
		if ( list->data_size <= sizeof( unsigned long ))
			memcpy( &node->u.ul, p, list->data_size);
		else
		{
			node->u.p = mem_alloc( list->data_size );
			memcpy( node->u.p, p, list->data_size );
		}
		break;
	default:
		break;
	}

	return 1;
}


void * link_list_compare_data_with_key( link_list_t * list, link_node_t * p, void * key )
{
   DECLARE_TRACE_DATA("link_list_compare_data_with_key")
   void *data_address[4] = 	{
		&p->u.ul, 
    &p->u.d,
		&p->u.p,
		&p->u.p
	};
	TRACE_PRINT( __FUNC__ );
	assert( list != NULL );
  assert( p != NULL);

	if ( list->pfn_compare_link )
	{
    int ret;
    ret = (* (list->pfn_compare_link))( data_address[ list->data_type ], key ); 
	  if ( ret )
	 	  return  (list->data_type == LINK_DATA_TYPE_PTR) ? p->u.p :
		         data_address[ list->data_type ];
	}

	return (void *)0;
}


int  link_list_compare_data( link_list_t * list, link_node_t * node, void * p )
{
	DECLARE_TRACE_DATA("link_list_compare_data")
	TRACE_PRINT( __FUNC__ );
	assert( list != NULL );

	switch( list->data_type )
	{
    case LINK_DATA_TYPE_ULONG:
  		return *((unsigned long *)p) == node->u.ul;
	  case LINK_DATA_TYPE_DOUBLE: 
	  	return (*(double *)p) == node->u.d;
	  case LINK_DATA_TYPE_PTR:
		  return  p == node->u.p;
	  case LINK_DATA_TYPE_VARIANT: 
		  if ( list->data_size <= sizeof( unsigned long ))
			  return memcmp( p, &node->u.ul, list->data_size);
		  else
		  {
			  if ( node->u.p && p )
		    	return memcmp( p, node->u.p, list->data_size );
		  }
		break;
	}

	return 0;
}


int link_list_get_data(link_list_t * list, link_node_t * node, void * p )
{
	DECLARE_TRACE_DATA("link_list_get_data")
	TRACE_PRINT( __FUNC__ );
	if ( !node || !p || !list ) return 0;

	switch ( list->data_type )
	{
  	case LINK_DATA_TYPE_ULONG:
		  *((unsigned long *)p) = node->u.ul;
		  break;
    case LINK_DATA_TYPE_DOUBLE: 
		  *(double *)p = node->u.d;
		  break;
    case LINK_DATA_TYPE_PTR:
		  *(void **)p = node->u.p;
		  break;
	  case LINK_DATA_TYPE_VARIANT: 
		  if ( list->data_size <= sizeof( unsigned long ))
			  memcpy( p, &node->u.ul, list->data_size);
		  else
		  {
			  if ( node->u.p && p )
		      	 memcpy( p, node->u.p, list->data_size );
		  }
		  break;
	  default:
		  break;
	}

	return 1;
}

void link_list_delete_node( link_list_t * list, link_node_t * node  )
{
	DECLARE_TRACE_DATA("link_list_DeleteNode")
	TRACE_PRINT( __FUNC__ )
	assert( list != NULL );
	assert( node != NULL );

	link_list_invoke_rundown( list, node );

	if (  node->u.p && (list->data_type == LINK_DATA_TYPE_VARIANT)
		  && (list->data_size > sizeof( unsigned long )))
         mem_free( node->u.p );

	mem_free( node );
	list->count -- ;
}

int link_list_move_node( link_list_t * from_list, link_list_t * to_list, void * key)
{
  link_node_t * p = NULL, *prev = NULL;
  void * data;
	TRACE_PRINT(__FUNC__);
	assert(from_list && to_list);
	p = from_list->head;

  do
	{
		data = link_list_compare_data_with_key( from_list, p, key );
    if (data) {
      /* Remove Node from FromList */
      if ( p == from_list->head) {
         from_list->head = p->next;
         p->next->prev =  p->prev;
         p->prev->next =  p->next;
         p->next = p->prev = p;
      } else {
         prev->next = p->next;
         p->next->prev = prev;
         p->prev = p->next = p;
      }
      from_list->count--;

      /* Add to To toList */
      if ( to_list->head && (to_list->head->next == to_list->head))
      {  
        to_list->tail = p;
    	  p->prev = to_list->head;
      }
    	if ( to_list->head )
	    {
         p->next        = to_list->head->next;
         p->next->prev  = p;
		     p->prev        = to_list->head;
		     to_list->head->next  = p; 
	    } else
	        to_list->tail = to_list->head =  p;
      to_list->count ++; 
      break;
    }
    prev = p;
		p = p->next;
	}	while( p != from_list->head );  
  
  return 1;
}

void link_list_invoke_rundown( link_list_t * list, link_node_t * node  )
{
	DECLARE_TRACE_DATA("link_list_invoke_rundown")
	void * p;
	void * data_address[4] = { 
		&node->u.ul, 
		&node->u.d, 
		&node->u.p,
		&node->u.p 
	};

	TRACE_PRINT( __FUNC__ )
	assert( list != NULL );
	assert( node != NULL );

	p =  data_address[ list->data_type ];
	if (  list->pfn_rundown )
     (*(list->pfn_rundown))( p ); 

}


#ifdef __DEBUG__
int link_list__debug_dump_ex( link_list_t * list )
{
	link_node_t * node = NULL;
	size_t count = 0; 
	assert( list != NULL );
	node = list->head;

	TRACE_PRINT1( "List : %s\r\n", list->Name );
	TRACE_PRINT("---------------------------------------------------------");

	if ( !node )
	{
		TRACE_PRINT( " Number of nodes : 0")
	}
	else
	{
		node = node->next;
		count = 1;
		for ( ;node != list->head; node = node->next )
		      count ++;
		TRACE_PRINT1( " Number of Nodes  : %d", count );
	}

	TRACE_PRINT("---------------------------------------------------------");

	return 1;
}
#endif /** __DEBUG__ **/
