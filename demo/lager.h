#ifndef __lager_h__
#define __lager_h__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "shelfinfo.h"
#include "utils.h"
#include "common.h"
#include "list.h"
#include "tree.h"
#include "item.h"
#include "undo.h"


/// Skapar en ny undo-strukt
undo_action_t *undo_new();

/// Återställer undo strukten till default-värden mellan varven
///
/// \param undo undo-strukten som ska återställas
void undo_reset(undo_action_t *undo);

/// HJälpfunktion för shelf_is_in_list
///
/// \param shelf hyllan att jämföra med
/// \param info HI-strukt som innehåller hyllnamnet vi söker efter
bool shelf_is_equal(elem_t shelf, void *info);


/// HJälpfunktion för find_available_shelf. Söker igenom en lista av hyllor efter
/// ett specifikt hyllnamn.
///
/// \param key namnet på varan vars hyllor vi söker igenom
/// \param item ett item som innehåller hyllistan
/// \info HI-strukt som sparar ägarinformation om hyllan.
/// \returns true om hyllan finns i listan, annars false
bool shelf_is_in_list(elem_t key, elem_t item, void *info);


void merge_shelves(shelf_info_t *info, elem_t shelf_to_edit, list_t *shelves);


/// Avänds vid redigering av lagerhylla när två hyllor ska slås ihop
///
/// \param info info strukten för hyllan som ska merge:a
/// \param shelf_to_edit den gamla hyllan
/// \param shelves listan med alla hyllor 
void merge_shelves(shelf_info_t *info, elem_t shelf_to_edit, list_t *shelves);

/// Frigör alla hyllor i en lista
///
/// \param shelves lista med alla hyllor
void remove_shelves(list_t *shelves);

/// Tar bort en vara från databasen.
///
/// \param db lagret
/// \param undo undo strukten som sparas ifall man vill ångra
void db_remove_item(tree_t *db, undo_action_t *undo);

// Konverterar ett intervall av heltal till en sträng och lägger till 'a' samt 'A' i slutet av den.
/// Strängen används för att välja ett intervall av index i menyn.
///
/// \param item_count antalet index som ska finnas
/// \returns lista med intar innehållande antalet valida svar (får konverteras till char sedan)
int *index_menu_choices(int item_count);

/// Kollar att en given index är valid
///
/// \param valid_indexes lista med antalet valida index
/// \param array_length längden på listan med valida index
/// \param index index som användaren matar in
/// \returns true eller false beroende på om index är valid eller inte
bool valid_index(int *valid_indexes, int array_length, int index);

#endif













  
