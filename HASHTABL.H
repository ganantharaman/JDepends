#ifndef __HASH_INCLUDE__
#define __HASH_INCLUDE__

/* Code macros */
#define IN   
#define OUT 

/* Bucket size */
#define BUCKET_SIZE  211

/* This is OS specific */
typedef unsigned long unsigned32;

typedef void (*pfn_hash_rundown_t)(hash_type_t);

#define HT_ISVALID( x ) ((x) != -1 )

#define HASH_TYPE_INT  1
#define HASH_TYPE_PTR  2

typedef struct hash_type
{
	union{
		unsigned32 ul;
		void   * pv;
	}u;
}hash_type_t;

typedef struct hash_node
{
   struct hash_node *next;
   struct hash_node *prev;
   hash_type_t  key;
   hash_type_t  val;
}hash_node_t;

typedef struct hash_table
{
#ifdef __DEBUG__
   char   name[ 256 ];
#endif
   hash_node_t  * hash_val[BUCKET_SIZE];//aHashValueArray[ BUCKET_SIZE ];
   int          key_type; //nKeyType;
   int          val_type; //nValueType;
   /** Used for error returns **/
   hash_type_t  invalid_ret; //InvalidReturn;
   /** Used to free ulValues **/
   pfn_hash_rundown_t  pfn_rundown; //RunDown;
   int          is_dict;
}hash_table_t,dict_t;


int dict_create(
#ifdef __DEBUG__
  char * name,
#endif
  int key_type,
  int val_type,
  hash_type_t * invalid,
  hash_table_t ** htp
);


int  hash_table_create(
#ifdef __DEBUG__
 IN  char * name,
#endif
 IN  int is_dict,
 OUT hash_table_t ** ht,
 hash_type_t  invalid_ret,
 int        key_type,
 int        val_type,
 pfn_hash_rundown_t pfn_free
);


int hash_table_add_ex( 
 IN  hash_table_t * ht, 
 IN  hash_type_t  key,
 IN  hash_type_t  val
);

hash_type_t hash_table_get_ex(
 IN  hash_table_t * ht,
 IN  hash_type_t   key 
);

int hash_table_remove_ex(
 IN  hash_table_t *hash_table,
 IN  hash_type_t   key
);

hash_type_t dict_getval(
 IN  hash_node_t *hash_node,
 IN  hash_type_t  key,
 IN  hash_type_t  invalid_val
);

hash_type_t hash_table_getval(
 IN  hash_node_t *hash_node,
 IN  char * pString,
 IN  hash_type_t  invalid_val
);

unsigned32 hash( 
 IN  unsigned char * str, 
 IN  unsigned32      size
);


hash_node_t * create_hash_node();

int  destroy_hash_node(
 IN  hash_node_t * hash_node 
);

void hash_table_destroy(
 IN  hash_table_t *hash_table 
);


#define dict_add(pht, key, val)  hash_table_add_ex(pht,key,val)
#define dict_get(pht, key)       hash_table_get_ex(pht,key)
#define dict_remove(pht, key)    hash_table_remove_ex(pht,key)
#define hash_table_add( pht, key, val )   hash_table_add_ex(pht,key,val)
#define hash_table_get( pht, key )      hash_table_get_ex(pht,key)
#define hash_table_remove( pht, key )   hash_table_remove_ex(pht,key) 
#define dict_destroy(x) hash_table_destroy((x))


#ifdef __DEBUG__
#define CreateHashTable(n,h,i,k,v,p)  hash_table_create(n,0,h,i,k,v,p)
#define CreateDictionary(n,h,i,k,v,p) hash_table_create(n,1,h,i,k,v,p)
#define CreatePtrDictionary(n,h)      dict_create(n,HASH_TYPE_PTR,HASH_TYPE_PTR,NULL,h)
#define CreateIntDictionary(n,h)      dict_create(n,HASH_TYPE_INT,HASH_TYPE_INT,NULL,h)
#else
#define CreateHashTable(h,i,k,v,p)  hash_table_create(0,h,i,k,v,p)
#define CreateDictionary(h,i,k,v,p) hash_table_create(1,h,i,k,v,p)
#define CreatePtrDictionary(h)      dict_create(HASH_TYPE_PTR,HASH_TYPE_PTR,NULL,h)
#define CreateIntDictionary(h)      dict_create(HASH_TYPE_INT,HASH_TYPE_INT,NULL,h)
#endif
 

#define AddToDictionary(pht, key, val)    hash_table_add_ex(pht,key,val)
#define GetFromDictionary(pht, key)       hash_table_get_ex(pht,key)
#define RemoveFromDictionary(pht, key)    hash_table_remove_ex(pht,key)
#define AddToHashTable( pht, key, val )   hash_table_add_ex(pht,key,val)
#define GetFromHashTable( pht, key )      hash_table_get_ex(pht,key)
#define RemoveFromHashTable( pht, key )   hash_table_remove_ex(pht,key)
#define DestroyHashTable(x)               hash_table_destroy(x)
#define DestroyDictionary(x)              hash_table_destroy(x)

#define Dictionary  dict_t
#define Hash_Table  hash_table_t
#define Hash_Type   hash_type_t

 
#endif /* __HASH_INCLUDE__ */