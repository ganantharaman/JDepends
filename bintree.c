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


#define STDLIB_H
#define ASSERT_H
#define STRING_H
#include <stdhdrs.h>

#include <mem.h>
#include <bintree.h>


#ifdef __DEBUG__
#define BT_ASSERT( x ) assert((x))
#else
#define BT_ASSERT( x ) ((void)0)
#endif


bin_tree_t * bin_tree_get_new_tree(unsigned long invalid_val)
{
	bin_tree_t * tree = ( bin_tree_t *)mem_alloc( sizeof( bin_tree_t ));
	if ( tree )	{
	 tree->root = NULL;
	 tree->invalid_val = invalid_val;
	}
  return tree;
}

void delete_tree( node_t *node )
{
	if ( !node ) 	return ;

  delete_tree( node->left  );
  delete_tree( node->right );
 	mem_free( node );
}

void  bin_tree_delete_tree( bin_tree_t *tree )
{
	BT_ASSERT( tree != NULL );
	delete_tree( tree->root );
	mem_free ( tree );
}


int bin_tree_add_node( bin_tree_t * tree, unsigned long key, unsigned long val )
{
	node_t *node = NULL;
	BT_ASSERT( tree != NULL );
	node = get_new_node( key, val );
	if ( node ) add_node( &tree->root, node );
	return node ? 1 : 0;
}


unsigned long bin_tree_search_tree( bin_tree_t * tree, unsigned long key )
{
	node_t * node = NULL;
	BT_ASSERT( tree != NULL );
	node = search_node( tree->root, key, tree->root, NULL); 
	return node ? node->val : tree->invalid_val; 
}


int  bin_tree_DeleteNode( bin_tree_t * tree, unsigned long key )
{
	BT_ASSERT( tree != NULL );
	return delete_node( &tree->root, key ); 
}

int bin_tree_traverse_tree( bin_tree_t * tree, int order, pfn_per_node_action_t  per_node_callback )
{
	/** nOrder - PREORDER(0), INORDER(1), POSTORDER(2) **/
	return traverse_tree( tree->root, order, per_node_callback );
}


node_t * get_new_node( unsigned long key, unsigned long val )
{
	node_t * p = ( node_t * )mem_alloc( sizeof( node_t ));
	if ( p ) {
    p->right = p->left = NULL;
	  p->key   = key;
	  p->val   = val;
	}
	return p;
}

int traverse_tree( node_t * root, int order, pfn_per_node_action_t action )
{
	pfn_traverse_t traverse[] = 
	{ traverse_pre_order, traverse_in_order, traverse_post_order };

	return ( *traverse[ order - 1])( root, action );
}


int traverse_pre_order( node_t *root, pfn_per_node_action_t action )
{
	if ( root == NULL )
		return 0;
	if ( action ) (* action )( root->key );
	traverse_pre_order( root->left, action );
	traverse_pre_order( root->right, action );
	return 1;
}

int traverse_in_order( node_t *root, pfn_per_node_action_t action )
{
	if ( root == NULL )
		return 0;
	traverse_in_order( root->left, action );
	if ( action ) (* action )( root->key );
	traverse_in_order( root->right, action );
	return 1;
}

int traverse_post_order( node_t *root, pfn_per_node_action_t action )
{
	if ( root == NULL )
		return 0;
	traverse_post_order( root->left, action );
	traverse_post_order( root->right, action );
	if ( action ) (* action )( root->key );
	return 1;
}



int add_node( node_t **root_node, node_t * new_node )
{
	node_t * root = *root_node;
	if ( root == NULL )	{	
		*root_node = new_node;
		 return 0;
	}
	if ( new_node->key >= root->key )	{
		if ( root->right )
			add_node( &root->right , new_node );
		else
			root->right = new_node;
	}	else	{
		if ( root->left ) 
			add_node( &root->left, new_node );
		else
			root->left = new_node;
	}
	return 1;
}


node_t * search_node( node_t *root , unsigned long key, node_t *parent_node, node_t **parent )
{
	node_t *ret_node = NULL;
	if ( root == NULL )
		 return NULL;
	if ( root->key == key )	{
      if ( parent )  *parent = parent_node;
      return root;
	}
	if ( key > root->key ) {
    if ( root->right )
		 ret_node = search_node( root->right, key, root, parent );
	}	else {
    if ( root->left )
		    ret_node = search_node( root->left, key, root, parent ); 
	}
	if ( parent ) *parent = parent_node;
	return ret_node;
}

/** CHECK,CHECK Use proper balancing techniques  **/
int delete_node ( node_t **root_node , unsigned long key )
{
  node_t *found_node = NULL;
	node_t *parent_node = NULL;
	
  if ( *root_node == NULL )
 	  return 0;   

	found_node = search_node( *root_node, key, *root_node, &parent_node ); 

	if ( !found_node )  return 0;
	if ( found_node->left ) {
	   if ( found_node->right )
	     add_node( &found_node->left,
	                found_node->right );
	   else
         found_node->right = found_node->left;
  }

  if ( parent_node->right == found_node )
       parent_node->right = found_node->right;

  if ( parent_node->left == found_node )
       parent_node->left = found_node->right;

  if ( parent_node == found_node )
	     *root_node = parent_node->right;

  mem_free( found_node );

  return 1;
}
