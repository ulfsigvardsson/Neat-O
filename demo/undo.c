#include "undo.h"


struct undo_action
{
  elem_t added;
  elem_t removed;
  elem_t edit_old;
  elem_t edit_new;
  last_action_t last_action;
};

/// Initieringsfunktion för undo_action-strukten.
undo_action_t *undo_new()
{
  undo_action_t *undo = allocate_array(1, sizeof(undo_action_t), NULL);
  retain(undo);
  undo->last_action = NOTHING;
  return undo;
}

/// Återställer undo-strukten till default-värden mellan varven
void undo_reset(undo_action_t *undo)
{
  if (undo->last_action == NOTHING) return; 

  else if (undo->last_action == EDIT)
    {
      if (undo->edit_old.p)
        {
          item_free(undo->edit_old);
          undo->edit_old.p = 0;
        }
      if (undo->edit_new.p)
        {
          item_free(undo->edit_new);
          undo->edit_new.p = 0;
        }
    }
  else if(undo->last_action == REMOVE)
    {
      if (undo->removed.p)
        {
          item_free(undo->removed);
          undo->removed.p = 0;
        }
    }
  else if (undo->last_action == ADD)
    {
      if (undo->added.p)
        {
          item_free(undo->added);
          undo->removed.p = 0;
        }
        
    }
  undo->last_action = NOTHING;
  
}

/// Free-funktion för undo_action-strukten
/// Om sista aktionen var att ta bort en vara är undo->new == NULL
/// och ska därmed INTE friöras
/// TODO: Skilj på hur elementen frigörs beroende på vad som gjordes sist
void undo_free(undo_action_t *undo)
{
  if (undo->last_action == REMOVE) item_free(undo->removed); 
  if (undo->last_action == EDIT)
    {
      item_free(undo->edit_old);
      item_free(undo->edit_new);
    }
  if (undo->last_action == ADD)
    {
      item_free(undo->added);
    } 
  release(undo);
}

last_action_t undo_get_last_action(undo_action_t *undo)
{
  return undo->last_action;
}

void undo_set_last_action(undo_action_t *undo, last_action_t action)
{
  undo->last_action = action;
}

elem_t undo_get_edit_old(undo_action_t *undo)
{
  return undo->edit_old;
}

void undo_set_edit_old(undo_action_t *undo, elem_t added)
{
  undo->edit_old = item_deep_copy(added);
}

elem_t undo_get_edit_new(undo_action_t *undo)
{
  return undo->edit_new;
}

void undo_set_edit_new(undo_action_t *undo, elem_t added)
{
  undo->edit_new = item_deep_copy(added);
}

elem_t undo_get_removed(undo_action_t *undo)
{
  return undo->removed;
}

void undo_set_removed(undo_action_t *undo, elem_t added)
{
  undo->removed = item_deep_copy(added);
}

elem_t undo_get_added(undo_action_t *undo)
{
  return undo->added;
}

void undo_set_added(undo_action_t *undo, elem_t added)
{
  undo->added = item_deep_copy(added);
}

/// Ångrar den senaste handlingen
void undo_last_action(tree_t *db, undo_action_t *undo)
{
  switch (undo->last_action)
    {
    case NOTHING:
      {
        printf("Inget att ångra. \n");
        return;
      }
    case ADD:
      {
        elem_t result;
        elem_t key = { .p = item_name(undo->added.p) }; 
        tree_remove(db, key, &result);
        // key_free(key);
        item_free(result);
        item_free(undo->added);
        break;
      }
    case REMOVE:
      {
        elem_t key  = { .p = allocate_string(item_name(undo->removed.p)) };
        elem_t elem = item_deep_copy(undo->removed); 
        tree_insert(db, key, elem);
        item_free(undo->removed);
        break;
      }
    case EDIT:
      {
        // undo av namnändringar fuckar upp
        elem_t key  = { .p = item_name(undo->edit_new.p)};
        elem_t item_to_undo;
        
        tree_remove(db, key, &item_to_undo); 

        elem_t old_item = item_deep_copy(undo->edit_old);
        elem_t old_key  = { .p = allocate_string(item_name(old_item.p))};

        tree_insert(db, old_key, old_item);

        item_free(item_to_undo);
        item_free(undo->edit_new);
        item_free(undo->edit_old);
        break;
      }
    default:
      break;
    }
  undo->last_action = NOTHING;
}
