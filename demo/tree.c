#include "tree.h"
#include <assert.h>

#define Compare(a, b) (*tree)->cmp_f(a, b)
#define Compare_keys tree->cmp_f((*c)->key, key)
#define Comp tree->cmp_f
#define Copy tree->copy_f
#define Free_elem tree->e_free_f
#define Free_key tree->k_free_f
#define Apply_key fun(node->key, node->elem, data)
#define Apply_left node_apply(node->left, order, fun, success, data)
#define Apply_right node_apply(node->right, order, fun, success, data)
#define Is_leaf !((*to_remove)->right || (*to_remove)->left)
#define Leaf !((*to_remove)->right || (*to_remove)->left)
#define Full (*to_remove)->right && (*to_remove)->left
#define Right (*to_remove)->right
#define Left (*to_remove)->left

/**
 * @struct En nod i trädet
 */
struct node
{
  elem_t elem;			/*< Nodens element */
  tree_key_t key;		/*< Nodens nyckel */
  node_t *left;			/*< Nodens vänstra subträd */
  node_t *right;		/*< Nodens högra subträd */
};

/**
 * @struct Strukt för det binära sökträdet
 */
struct tree
{
  node_t *root;			///< Trädets rotnod
  element_copy_fun copy_f;	///< Trädets funktion för att kopiera sina element
  element_free_fun e_free_f;	///< Funktion för att frigöra element i trädet
  key_free_fun k_free_f;	///< Funktion för att frigöra nycklar i trädet
  element_comp_fun cmp_f;	///< Funktion för att jämföra element i trädet
  size_t size;			///< Antal noder i trädet
};

/**
 * @brief Returnerar rotnoden i ett träd, endast för tester
 * @param tree_t Ett binärt sökträd
 */
void *
get_root_elem (tree_t * tree)
{
  return (tree->root->elem.p);
}

/**
 * @brief Defaultfunktion för kopiering av element om användaren inte anger en egen
 * @param elem ett element i trädet
 */
elem_t
tree_no_copy (elem_t elem)
{
  return elem;
}

/**
 * @brief defaultfunktion för frigörning av element och nycklar om användaren inte anger en egen
 * @param elem ett element eller nyckel i trädet
 */
void
tree_no_free (elem_t elem)
{
  return;
}

/**
 * @brief Defaultfunktion för jämförelse av element om användaren inte anger en egen
 * @param elem1 Ett element i trädet
 * @param elem2 Elementet att jämföra med elem1
 */
int
tree_no_compare (elem_t elem1, elem_t elem2)
{
  if (elem1.i == elem2.i)
    return 0;
  else if (elem1.i > elem2.i)
    {
      return 1;
    }
  else
    return -1;
}

/**
 * @brief Skapar och allokerar minnesplats för en ny nod
 * @returns Den nya noden
 */
node_t *
node_new ()
{
  node_t *node = allocate_array (1, sizeof (node_t), NULL);
  retain (node);
  return node;
}

/**
 * Creates a new tree
 *
 * @param copy (may be NULL) a function applied to all elements when stored in the tree
  * @param key_free (may be NULL) used to free keys in tree_delete
  * @param elem_free (may be NULL) used to free elements in tree_delete
  * @param compare (may be NULL) used to compare keys
  * @returns: empty tree
  */
tree_t *
tree_new (element_copy_fun element_copy, key_free_fun key_free,
	  element_free_fun elem_free, element_comp_fun compare)
{
  tree_t *tree = allocate_array (1, sizeof (tree_t), NULL);
  tree->root = NULL;
  retain (tree);

  Copy = element_copy ? element_copy : tree_no_copy;
  Free_key = key_free ? key_free : tree_no_free;
  Free_elem = elem_free ? elem_free : tree_no_free;
  Comp = compare ? compare : tree_no_compare;

  return tree;

}

/**
 * @brief Hjälpfunktion för tree_delete. Frigör ett element i trädet
 * @param key En nyckel tillhörande elem
 * @param elem Elementet att frigöra
 * @param free_elem Trädets frigörningsfunktion för element
 * @returns true
 */
bool
tree_delete_elems (tree_key_t key, elem_t elem, void *free_elem)
{
  element_free_fun fun = free_elem;
  fun (elem);
  return true;
}

/**
 * @brief Hjälpfunktion för tree_delete. Frigör en key i trädet
 * @param key En nyckel att frigöra
 * @param elem Ett element i trädet tillhörande key
 * @param free_key Trädets frigörningsfunktion för key
 * @returns true
 */
bool
tree_delete_keys (tree_key_t key, elem_t elem, void *free_key)
{
  element_free_fun fun = free_key;
  fun (key);
  return true;
}

/**
 * @brief Hjälpfunktion för tree_delete. frigör en nod och dess subträd
 * @param node Dubbelpekare till en nod
 */
void
tree_delete_nodes (node_t ** node)
{
  if (*node)
    {
      tree_delete_nodes (&(*node)->left);
      tree_delete_nodes (&(*node)->right);
      release (*node);
      (*node) = NULL;
    }
  else
    {
      return;
    }
}

/**
 * Remove a tree along with all elem_t elements.
 *
 * @param tree The tree
 * @param delete_keys If true, run tree's key_free function on all keys
 * @param delete_elements If true, run tree's elem_free function on all elements
 */
void
tree_delete (tree_t * tree, bool delete_keys, bool delete_elements)
{
  if (delete_elements)
    {
      tree_apply (tree, inorder, tree_delete_elems, Free_elem);
    }
  if (delete_keys)
    {
      tree_apply (tree, inorder, tree_delete_keys, Free_key);
    }

  tree_delete_nodes (&tree->root);
  tree->size = 0;
  release (tree);
  return;
}

/**
 * Get the size of the tree
 *
 * @returns The number of nodes in the tree
 */
int
tree_size (tree_t * tree)
{
  return tree->size;
}

/**
 * Returns the greatest of two integers
 */
int
max (int a, int b)
{
  return a > b ? a : b;
}

/**
 * @brief Hjälpfunktion för  tree_depth
 *
 * @param node Node for wich to decide the depth
 */
int
tree_depth_aux (node_t * node)
{
  if (node && (node->right || node->left))
    {
      return 1 + max (tree_depth_aux (node->left),
		      tree_depth_aux (node->right));
    }

  return 0;
}

/**
 * Get the depth of the tree
 *
 * @returns The depth of the deepest subtree
 */
int
tree_depth (tree_t * tree)
{
  if (tree)
    {
      return tree_depth_aux (tree->root);
    }

  return 0;
}

/**
 * Allmän traverseringsfunktion som används för insättning, sökning, djup och borttagning.
 * Returnerar en dubbelpekare till ett träd som antingen är det som matchar 'key' eller ett
 * tomt subträd där vi borde ha hittat den.
 *
 * @param tree The tree to traverse
 * @param key The key to look for
 * @returns node_t The node
 */
node_t **
tree_traverse (tree_t * tree, elem_t key)
{
  node_t **c = &(tree->root);

  while (*c)
    {
      if (Compare_keys == 0)
	return c;

      else if (Compare_keys < 0)
	{
	  c = &((*c)->right);
	}
      else if (Compare_keys > 0)
	{
	  c = &((*c)->left);
	}
    }
  return c;
}

bool
get_balance_aux (node_t * node)
{
  if (node)
    {
      return (abs (tree_depth_aux (node->left) - tree_depth_aux (node->right))
	      < 2 && get_balance_aux (node->left)
	      && get_balance_aux (node->right));
    }
  return true;

}

bool
get_balance (tree_t * tree)
{
  return (get_balance_aux (tree->root));
}

// Balanserar ett träd
void
build_tree_aux (tree_t * tree, elem_t * element_list, tree_key_t * key_list,
		int low, int high)
{
  int index = (high + low) / 2;
  elem_t element = element_list[index];
  tree_key_t key = key_list[index];
  tree_insert (tree, key, element);

  if (index > low)
    {
      build_tree_aux (tree, element_list, key_list, low, index - 1);
    }
  if (index < high)
    {
      build_tree_aux (tree, element_list, key_list, index + 1, high);
    }
}


void
tree_balance (tree_t ** tree)
{
  if ((*tree)->size < 3)
    {
      return;
    }
  elem_t *element_list = tree_elements (*tree);
  tree_key_t *key_list = tree_keys (*tree);

  tree_t *new_tree = tree_new ((*tree)->copy_f, (*tree)->k_free_f,
			       (*tree)->e_free_f, (*tree)->cmp_f);

  build_tree_aux (new_tree, element_list, key_list, 0, (*tree)->size - 1);

  release (element_list);
  release (key_list);

  tree_delete (*tree, false, false);
  *tree = new_tree;
}

/// Insert element into the tree. Returns false if the key is already used.
///
/// Uses the tree's compare function to compare keys.
///
/// If tree's copy function is non-NULL, it will be applied to
/// elem and its result stored in the tree. Otherwise, elem will
/// be stored in the tree. Note that keys are never copied and are
/// assumed to be immutable. (They may however be freed by the
/// tree.)
///
/// \param tree pointer to the tree
/// \param key the key of element to be appended -- this is assumed to be an immutable value
/// \param elem the element
/// \returns: true if successful, else false
bool
tree_insert (tree_t * tree, tree_key_t key, elem_t elem)
{
// Corner case för tomma träd
  if (tree->size == 0)
    {
      tree->root = node_new ();
      tree->root->elem = Copy (elem);
      tree->root->key = key;
      tree->size++;
    }

  // Trädet innehåller redan nyckeln
  else if (tree_has_key (tree, key))
    return false;

  else
    {
      node_t **c = tree_traverse (tree, key);
      *c = node_new ();
      (*c)->elem = Copy (elem);
      (*c)->key = key;
      tree->size++;
    }
  return true;
}

/// Checks whether a key is used in a tree
///
/// Uses the tree's compare function to compare keys.
///
/// \param tree pointer to the tree
/// \param key the key to check for inclusion in the tree
/// \returns: true if key is a key in the tree
bool
tree_has_key (tree_t * tree, tree_key_t key)
{
  node_t **c = tree_traverse (tree, key);
  return *c != NULL;
}

/// Finds the element for a given key in tree.
///
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \param result a pointer to where result can be stored (only defined when result is true)
/// \returns: true if key is a key in the tree
bool
tree_get (tree_t * tree, tree_key_t key, elem_t * result)
{
  node_t **c = tree_traverse (tree, key);

  if (*c)
    {
      *result = ((*c)->elem);
      return true;
    }

  else
    return false;
}

// Frigör en nod i trädet
void
free_node (tree_t * tree, node_t * node)
{
  Free_key (node->key);
  Free_elem (node->elem);
  release (node);
}

// Hittar den minsta efterträdaren till ett element i trädet
node_t **
find_smallest_successor (node_t ** node)
{
  while ((*node)->left)
    node = &(*node)->left;

  return node;
}

/// Removes the element for a given key in tree.
///
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \param result a pointer to where result can be stored (only defined when result is true)
/// \returns: true if key is a key in the tree
bool
tree_remove (tree_t * tree, tree_key_t key, elem_t * result)
{
  node_t **to_remove = tree_traverse (tree, key);

  if (*to_remove)
    {
      if ((*to_remove)->right && (*to_remove)->left)
	{
	  node_t **minimum = find_smallest_successor (&(*to_remove)->right);	// Elementet att ersätta med

	  *result = Copy ((*to_remove)->elem);
	  (*to_remove)->elem = Copy ((*minimum)->elem);	// Kopiera data
	  Free_key ((*to_remove)->key);
	  (*to_remove)->key = (*minimum)->key;
	  release (*minimum);
	  *minimum = NULL;	// Nolla pekaren till den minsta efterföljaren (?)
	}

      else if ((*to_remove)->right)
	{
	  *result = Copy ((*to_remove)->elem);
	  node_t *temp = *to_remove;
	  *to_remove = (*to_remove)->right;

	  Free_key (temp->key);
	  release (temp);
	}

      else if ((*to_remove)->left)
	{
	  *result = Copy ((*to_remove)->elem);
	  node_t *temp = *to_remove;
	  *to_remove = (*to_remove)->left;

	  Free_key (temp->key);
	  release (temp);
	}

      else if (Is_leaf)
	{
	  *result = Copy ((*to_remove)->elem);

	  Free_key ((*to_remove)->key);
	  release (*to_remove);
	  (*to_remove) = NULL;
	}

      tree->size--;
      return true;
    }

  return false;
}

/// HJälpfunktion för tree_keys
/// \param node den aktuella noden vars key läggs till i keys inorder
/// \param keys en array av samma storlek som antal element i trädet där nycklar läggs till
/// \param index heltalspekare som anger indexplatsen att lägga till på i keys
void
tree_keys_aux (node_t * node, elem_t * keys, int *index)
{
  if (!node)
    return;
  else
    {
      tree_keys_aux (node->left, keys, index);
      keys[*index] = node->key;
      ++(*index);
      tree_keys_aux (node->right, keys, index);
    }
}

/// Returns an array holding all the keys in the tree
/// in ascending order.
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() keys
tree_key_t *
tree_keys (tree_t * tree)
{
  int size = tree_size (tree);
  elem_t *keys = allocate_array (size, sizeof (elem_t), NULL);
  int index = 0;
  tree_keys_aux (tree->root, keys, &index);
  return keys;
}

/// Hjälpfunktion för tree_elements
/// \param node den aktuella noden vars element läggs till i elems inorder
/// \param elems en array av samma storlek som antal element i trädet där element läggs till
/// \param index heltalspekare som anger indexplatsen att lägga till på i elems
void
tree_elements_aux (node_t * node, elem_t * elems, int *index)
{
  if (!node)
    return;
  else
    {
      tree_elements_aux (node->left, elems, index);
      elems[*index] = node->elem;
      (*index)++;
      tree_elements_aux (node->right, elems, index);
    }
}

/// Returns an array holding all the elements in the tree
/// in ascending order of their keys (which are not part
/// of the value).
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() elements
elem_t *
tree_elements (tree_t * tree)
{
  int size = tree_size (tree);
  elem_t *elems = allocate_array (size, sizeof (elem_t), NULL);
  int index = 0;
  tree_elements_aux (tree->root, elems, &index);
  return elems;
}


/// Hjälpfunktion för tree_apply. Applicerar en key_elem_apply_fun på nycklar och element i en nod och dess subträd.
/// \param node den aktuella noden
/// \tree_order enum som avgör i vilken ordning funktionen ska appliceras på noden, dess vänstra samt högra subträd.
/// \fun funktionen som ska appliceras
/// \success boolpekare som sätts till true om funktionen lyckas genomföra sitt arbete på noden eller på någon av noderna i subträden
bool
node_apply (node_t * node, enum tree_order order, key_elem_apply_fun fun,
	    bool * success, void *data)
{
  if (node)
    {

      switch (order)
	{
	case inorder:
	  Apply_left;
	  if (Apply_key)
	    {
	      *success = true;
	    }
	  Apply_right;
	  break;

	case preorder:
	  if (Apply_key)
	    {
	      *success = true;
	    }
	  Apply_left;
	  Apply_right;
	  break;

	default:
	  Apply_left;
	  Apply_right;
	  if (Apply_key)
	    {
	      *success = true;
	    }
	  break;
	}
    }

  return *success;
}

/// Applies a function to all elements in the tree in a specified order.
/// Example (using shelf as key):
///
///     tree_t *t = tree_new();
///     tree_insert(t, "A25", some_item);
///     int number = 0;
///     tree_apply(t, inorder, print_item, &number);
///
/// where print_item is a function that prints the number and increments it,
/// and prints the item passed to it.
///
/// \param tree the tree
/// \param order the order in which the elements will be visited
/// \param fun the function to apply to all elements
/// \param data an extra argument passed to each call to fun (may be NULL)
/// \returns the result of all fun calls, combined with OR (||)
bool
tree_apply (tree_t * tree, enum tree_order order, key_elem_apply_fun fun,
	    void *data)
{
  if (tree)
    {
      bool success = false;
      return node_apply (tree->root, order, fun, &success, data);
    }
  return false;
}
