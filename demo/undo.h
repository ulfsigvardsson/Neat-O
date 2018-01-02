#ifndef __undo_h__
#define __undo_h__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "tree.h"
#include "item.h"
#include "../src/refmem.h"

typedef struct undo_action undo_action_t;

typedef enum last_action {NOTHING, ADD, REMOVE, EDIT} last_action_t;

/// Initieringsfunktion för undo_action-strukten.
undo_action_t *undo_new();

/// Återställer undo-strukten till default-värden mellan varven
void undo_reset(undo_action_t *undo);

void undo_last_action(tree_t *db, undo_action_t *undo);

last_action_t undo_get_last_action(undo_action_t *undo);

/// Free-funktion för undo_action-strukten
/// Om sista aktionen var att ta bort en vara är undo->new == NULL
/// och ska därmed INTE friöras
/// TODO: Skilj på hur elementen frigörs beroende på vad som gjordes sist
void undo_free(undo_action_t *undo);

void undo_set_last_action(undo_action_t *undo, last_action_t action);

elem_t undo_get_edit_old(undo_action_t *undo);

void undo_set_edit_old(undo_action_t *undo, elem_t added);

elem_t undo_get_edit_new(undo_action_t *undo);

void undo_set_edit_new(undo_action_t *undo, elem_t added); 

elem_t undo_get_removed(undo_action_t *undo);

void undo_set_removed(undo_action_t *undo, elem_t added);
elem_t undo_get_added(undo_action_t *undo);

void undo_set_added(undo_action_t *undo, elem_t added);
#endif
