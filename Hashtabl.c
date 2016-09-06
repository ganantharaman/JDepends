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
#include <hashtabl.h>
#include <cmnmcros.h>

#define STDLIB_H
#define ASSERT_H
#define STRING_H
#define STDIO_H
#include <stdhdrs.h>


/*
   This is a typical Bucket chain
   hash table implementation.  
   The key could be a string or a unsigned32 val
   The val could be any thing cast to unsigned32.
   when You give the key we get you the val
   We expose a set of functions for accessing a
   given hash_table_t as a dictionary(where the key need
   not be a string but just an unsigned32 number ) or
   as a hash_table_t itself.
  
   So we have, for all functions taking a flag
   is_dict = 1   if phash_table is a dictionary
                 = 0  if phash_table is a hash_table_t
   
*/


#define HT_VALIDATE( x , y) if (!(x)) return y

int dict_create(
#ifdef __DEBUG__
  char * name,
#endif
  int key_type,
  int val_type,
  hash_type_t * invalid_val_p,
  hash_table_t ** htp
)
{
  int rc;
  hash_type_t invalid_ret;

  if ( invalid_val_p )
    invalid_ret = *invalid_val_p;
  else {
   if (( key_type == HASH_TYPE_PTR   && 
        val_type == HASH_TYPE_PTR ) || 
      ( key_type == HASH_TYPE_INT && 
        val_type == HASH_TYPE_PTR ))
     invalid_ret.u.pv = NULL;
   else if ( key_type == HASH_TYPE_INT &&
            val_type == HASH_TYPE_INT )
     invalid_ret.u.ul = 0;
  }
 
  rc = hash_table_create( 
#ifdef __DEBUG__
    name,
#endif
    1,
    htp,
    invalid_ret,
    key_type,
    val_type,
    NULL
  );
  return rc;
}

 
/* Create a new hash_table */
/* Re-entrant */
int hash_table_create(
#ifdef __DEBUG__
  char * name,
#endif
int is_dict,
hash_table_t **htp, 
hash_type_t  invalid_ret,
int key_type,
int val_type,
pfn_hash_rundown_t pfn_rundown
) 
{
    hash_table_t *ht = 
    (hash_table_t * ) mem_alloc(sizeof(hash_table_t));
   if ( ht )
     memset( ht->hash_val, 0, sizeof( hash_node_t * ) * BUCKET_SIZE);
#ifdef __DEBUG__
	strcpy( ht->name, name );
#endif
	ht->key_type = key_type;
	ht->val_type = val_type;
	/** This val must be guaranteed not to be a storage val **/
	ht->invalid_ret = invalid_ret;
	ht->pfn_rundown = pfn_rundown;
  ht->is_dict = is_dict;
  *htp = ht;

	return (ht != NULL);
}

/* Re-entrant */
void hash_table_destroy( 
 IN  hash_table_t *ht
)
{
	hash_node_t * node = NULL;
	hash_node_t * curr_node = NULL;
	int index = 0;

	if ( !ht ) return;
	/* For each bucket */
  for ( ;index < BUCKET_SIZE; index ++ ) {
    node = ht->hash_val[index];
		if ( node ) {
      /* Free all the links in the chain */ 
      do {
				 curr_node = node->next;
         if (!ht->is_dict)
            if ( node->key.u.pv )
              free ( node->key.u.pv );
				 /** Call the free func**/
         if (  ht->pfn_rundown )
    				(* ht->pfn_rundown )( node->val );
				 mem_free ( node );
			} while ( node = curr_node );
		}
	}
	 /* Free the hash table itself */
   mem_free( ht );
}


int hash_table_add_ex( 
 IN hash_table_t *ht, 
 IN hash_type_t key,
 IN hash_type_t val
)
{
	char          *  str     = NULL;
	hash_node_t   *  node    = NULL;    
	hash_node_t   *  new_hash_node = NULL;
	unsigned32       hash_val  = 0;

  assert( ht != NULL );
	new_hash_node = create_hash_node();
	HT_VALIDATE( new_hash_node, 0);

	new_hash_node->key = key;
	new_hash_node->val = val;

	if ( !ht->is_dict )	{
	  str     = (char *)key.u.pv;
	  if (!str ) return 0;
      hash_val  = hash( str, strlen( str ));
	  new_hash_node->key.u.pv = strdup( str );
	}
  node  =  ht->hash_val[hash_val];
	if ( node )	{
	  new_hash_node->next =	node;
	  new_hash_node->prev = node->prev;
	  node->prev    = new_hash_node;
	}
  ht->hash_val[hash_val] = new_hash_node;
	return 1;
}


hash_type_t hash_table_get_ex(
 IN  hash_table_t * ht,
 IN  hash_type_t   key 
)
{
 	register char *  str            = NULL;
	register hash_node_t  *  node    = NULL;    
	register unsigned32 hash_val     = 0;
  assert( ht != NULL );
	if (!ht->is_dict )	{
	  str      =  (char *)key.u.pv;
	  hash_val =  hash( str, strlen( str ));
	}
	node   =  ht->hash_val[ hash_val ];
  HT_VALIDATE( node , ht->invalid_ret );

	return ht->is_dict ? dict_getval( node, key, ht->invalid_ret ):
	        hash_table_getval( node, str, ht->invalid_ret );
}

hash_type_t dict_getval(
 IN  hash_node_t  * node,
 IN  hash_type_t key, 
 IN  hash_type_t invalid_ret
)
{
	do {
		if ( node->key.u.ul == key.u.ul )
			return node->val;
	}	while ( node = node->next );
  return invalid_ret;
}

hash_type_t hash_table_getval(
 IN  hash_node_t *node,
 IN  char * str, 
 IN  hash_type_t invalid_ret
)
{
  do {
     if ( stricmp( (char *)node->key.u.pv, str ) == 0 )
	    return node->val;
	} while ( node = node->next );
  return invalid_ret;
}


int hash_table_remove_ex(
 IN  hash_table_t * ht,
 IN  hash_type_t    key
)
{
 	char *  str               = NULL;
	hash_node_t  * start_node  = NULL;
	hash_node_t  * node        = NULL;    
	unsigned32 hash_val        = 0;
	int    found_node          = 0;

  assert( ht != NULL );
	if ( !ht->is_dict ) {
	  str      =  ( char * )key.u.pv;
	  hash_val =  hash( str, strlen( str ));
	}
	start_node = node  =  ht->hash_val[ hash_val ];
	HT_VALIDATE( node, ht->invalid_ret.u.ul );
	do {
		found_node =  ht->is_dict ? (node->key.u.ul == key.u.ul) :
	                   !stricmp( (char *)node->key.u.pv , str ) ;
	   if ( found_node )  {
   	    if ( node == start_node )
			    ht->hash_val[hash_val] = node->next;

    	  if ( node->next  )
			    node->next->prev = node->prev;

   		  if ( node->prev )
			    node->prev->next = node->next;

   		  if ( !ht->is_dict  && node->key.u.pv )
			    free( (void *)node->key.u.pv );
        /* Last Node */ 
  		  if ( (!node->prev) && (!node->next))
              ht->hash_val[ hash_val ] = 0;
   		  /** Call the free func**/
        if ( ht->pfn_rundown )
  			  (* ht->pfn_rundown )( node->val );
  		   destroy_hash_node( node );
  		  return 1;
	   }
	} while ( node = node->next );
	return 0;
}




unsigned32 hash( 
 IN  unsigned char * str,
 IN  unsigned32      size
)
{
	char * p = NULL;
	unsigned32 	hash_val = 0, u;
	p = str;
	for ( ;size && *p; size--, p++ )
	{
	   hash_val = ( hash_val << 4 ) + *p;
     /* intentional assignment */
	   if ( u =  hash_val & 0xf0000000 )
	   {
		   hash_val =  hash_val ^ ( u >> 24 );
		   hash_val =  hash_val ^ u;
	   }
	}
	return hash_val % BUCKET_SIZE;
}


hash_node_t * create_hash_node()
{
    hash_node_t * node = mem_alloc( sizeof( hash_node_t ) );
    if ( node ) {
       memset( &node->val, 0, sizeof( hash_type_t ));  
	     memset( &node->key, 0, sizeof( hash_type_t ));  
	     node->next      = NULL;
	     node->prev      = NULL;
    }
    return node;
}

int destroy_hash_node( 
 IN  hash_node_t * node
)
{
	HT_VALIDATE( node, 0 );
  mem_free ( node );
  return 1;
}


#ifdef __DEBUG__
int  hash_table_debug_dump_ex(  hash_table_t * ht )
{
  int size = BUCKET_SIZE - 1;
  hash_node_t * node = NULL;
  int count = 0;

  assert( ht != NULL );
  TRACE_PRINT1( "Hash Table : %s\r\n", ht->name );
  TRACE_PRINT("---------------------------------------------------------");
 
  for ( ;size; size--) {
 	  node  =  ht->hash_val[ size ];
	  if ( node ){
		  count = 0;
		  /** Count nodes **/
		  for ( ;node; node = node->next )
			  count ++;
  		/** Print **/
	  	TRACE_PRINT2("Bucket %d has %d nodes\r\n", size, count)
  	}
  }

  TRACE_PRINT("---------------------------------------------------------");
  return 1;
}
#endif



