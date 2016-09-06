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

#ifndef  __BIN_TREE_INCLUDE__
#define  __BIN_TREE_INCLUDE__

typedef struct node {
  struct node * left;
  struct node * right;
  unsigned long key;
  unsigned long val;
} node_t;

typedef struct bin_tree {
  node_t * root;
  unsigned long invalid_val;
}bin_tree_t;

typedef int ( * pfn_per_node_action_t )( unsigned long val);
typedef int ( * pfn_traverse_t )( node_t *p , pfn_per_node_action_t pfn_action );

bin_tree_t * bin_tree_get_new_tree();
void   delete_tree( node_t * root );
void   bin_tree_delete_tree( bin_tree_t *tree );
int    bin_tree_add_node( bin_tree_t * tree, unsigned long key, unsigned long val );
node_t * bin_tree_search_node( bin_tree_t * tree, unsigned long val );
int     bin_tree_delete_node( bin_tree_t * tree, unsigned long val );
int    bin_tree_traverse_tree( bin_tree_t *tree, int order, pfn_per_node_action_t pfn_action );

int traverse_tree( node_t * root, int order, pfn_per_node_action_t pfnAction );
int traverse_pre_order( node_t *root, pfn_per_node_action_t pfn_action );
int traverse_in_order( node_t *root, pfn_per_node_action_t pfn_action );
int traverse_post_order( node_t *root, pfn_per_node_action_t pfn_action );

node_t * get_new_node( unsigned long key, unsigned long val );
int    add_node( node_t **root, node_t * n );
node_t * search_node( node_t *root, unsigned long val, node_t * parent, node_t **pparent );
int    delete_node( node_t **root, unsigned long val );

#define PRE_ORDER  1
#define IN_ORDER   2
#define POST_ORDER 3

#endif /* __BIN_TREE_INCLUDE__ */