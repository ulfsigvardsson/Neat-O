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

#define INVALID_INDEX -1
#define MainMenuChoice toupper(ask_question_menu("Ange ett menyval: ", "KLTRGHAS"))
#define AskName ask_question_string("Ange namn: ")
#define AskDescription ask_question_string("Ange beskrivning: ")
#define AskPrice ask_question_int("Ange pris: ")
#define AskShelf ask_question_shelf("Välj en hylla: ");
#define AskAmount ask_question_int("Välj antal att lagra: ")
#define ConfirmAddition ask_question_menu("\nVill du lägga till varan? [J]a, [N]ej, [R]edigera", "JjNnRr")
#define AskRemoveShelf ask_question_shelf("Välj hylla att ta bort ifrån: ")
#define SHELF_IS_IN_LIST(shelves, info) list_apply(shelves, shelf_is_equal, info)
#define AskWhatToEdit ask_question_menu("\n[N]amn\n"                    \
                                        "[B]eskrivning\n"               \
                                        "[P]ris\n"                      \
                                        "[L]agerhylla\n"                \
                                        "An[t]al\n\n"                   \
                                        "Välj rad eller [a]vbryt: ", "BbPpLlTtAaNn");

bool list_db (tree_t * db);
void list_and_select_item (tree_t * db);
elem_t select_by_index (tree_t * db, int item_count);
void edit_item (tree_t * db, elem_t item, undo_action_t * undo);

/// Skriver ut huvudmenyn
void
print_main_menu ()
{
    printf ("\n[L]ägga till en vara\n"
            "[T]a bort en vara\n"
            "[R]edigera en vara\n"
            "Ån[g]ra senaste ändringen\n"
            "Lista [h]ela varukatalogen\n"
            "[S]ortera databasen\n"
            "[K]ontrollera databasens sortering\n" "[A]vsluta\n\n");
}


/**
 * @brief list_apply-funktion. Skriver ut en enskild hylla.
 * @param elem En hylla i en lista
 * @param data shelf_data-strukt med info om hyllan som vill modifieras
 * @return true
 */
bool
print_shelf (elem_t elem, void *data)
{
    printf ("%s  \t %d\n", shelf_id (elem.p), shelf_amount (elem.p));
    return true;
}


// Hjälpfunktion till print_item(). Skriver ut alla hyllor368
void
print_shelves (list_t * list)
{
    printf ("Hylla \t Antal\n");
    list_apply (list, print_shelf, NULL);
}


/// Skriver ut all info om en vara på skärmen
/// param elem varan att skriva ut
void
print_item (elem_t elem)
{
    item_t *item = elem.p;

    printf ("\n=====VARUSPECIFIKATION====\n"
            "Namn: %s\n"
            "Beskrivning: %s\n"
            "Pris: %d\n",
            item_name (item), item_descr (item), item_price (item));
    print_shelves (item_shelves (item));
}


/// Låter användaren välja en av hyllorna för en vara
/// \param item en vara att editera
/// \param result elem_t-pekare att lagra vald hylla i
/// \returns index på vald hylla i listan
int
select_existing_shelf (elem_t item, elem_t * result)
{
    list_t *shelves = item_shelves (item.p);
    int index = 0;
    elem_t lookup;
    do
        {
            char *shelf_to_edit = AskShelf;
            lookup.p = shelf_new (shelf_to_edit, 0);
            index = list_contains (shelves, lookup);

            if (index == INVALID_INDEX)
                {
                    printf ("Ogiltigt val!\n");
                    shelf_free (lookup);
                }

        }
    while (index == INVALID_INDEX);

    list_get (shelves, index, result);
    shelf_free (lookup);
    return index;
}

/// HJälpfunktion för shelf_is_in_list
/// \param shelf hyllan att jämföra med
/// \param info HI-strukt som innehåller hyllnamnet vi söker efter
bool
shelf_is_equal (elem_t shelf, void *info)
{
    char *lookup_id = info_id (info);
    char *current_id = shelf_id (shelf.p);

    if (strcmp (lookup_id, current_id) == 0)
        {
            set_info_exists (true, info);
            return true;
        }

    return false;
}


/// HJälpfunktion för find_available_shelf. Söker igenom en lista av hyllor efter
/// ett specifikt hyllnamn.
/// \param key namnet på varan vars hyllor vi söker igenom
/// \param item ett item som innehåller hyllistan
/// \info HI-strukt som sparar ägarinformation om hyllan.
/// \returns true om hyllan finns i listan, annars false
bool
shelf_is_in_list (elem_t key, elem_t item, void *info)
{
    list_t *shelves = item_shelves (item.p);
    char *lookup_name = info_name (info);
    char *current_name = item_name (item.p);

    if (SHELF_IS_IN_LIST (shelves, info))
        {
            set_info_exists (true, info);
            set_shelf_owner (info, current_name, lookup_name);
        }

    return info_exists (info);
}

/// Söker igenom databasen efter hyllnamn som användaren anger.
/// Returnerar ett hyllnamn som inte är upptaget av en annan vara än den aktuella.
char *
find_available_shelf (tree_t * db, char *name, shelf_info_t * info)
{
    char *chosen_id = "A";
    set_info_name (name, info);

    do
        {
            set_info_exists (false, info);
            set_info_owner (NONE, info);

            chosen_id = ask_question_shelf ("Välj en ny hylla: ");


            set_info_id (chosen_id, info);

            tree_apply (db, preorder, shelf_is_in_list, info);

            if (info_owner (info) == OTHER)
                {
                    printf ("Hyllan är upptagen!\n");
                    release (chosen_id);
                }
        }
    while (info_owner (info) == OTHER);

    return chosen_id;
}

/// Slår ihop en existerande hylla med en annan , aningen ledig eller ockuperad av samma vara.
/// \param info SI-strukt som innehåller det nya hyllnamnet
/// \param shelf_to_edit hyllan som ska ändras
/// \param shelves varan hela lista av hyllor
void
merge_shelves (shelf_info_t * info, elem_t shelf_to_edit, list_t * shelves)
{
    char *chosen_id = info_id (info);
    printf ("Du valde att slå ihop med %s\n", chosen_id);

    int amount = shelf_amount (shelf_to_edit.p);
    elem_t to;
    elem_t shelf_to_merge_with = {.p = shelf_new (chosen_id, 0) };
    int index = list_contains (shelves, shelf_to_merge_with);

    // Lägg till antalet varor från den gamla hyllan på den nya
    list_get (shelves, index, &to);
    shelf_add_amount (to.p, amount);

    if (strcmp (shelf_id (shelf_to_edit.p), chosen_id) != 0)
        {
            // Ta bort den gamla hyllan från listan
            int index_to_remove = list_contains (shelves, shelf_to_edit);
            list_remove (shelves, index_to_remove, true);
        }
    shelf_free (shelf_to_merge_with);
}

/// Redigerar namn på en varas hylla
/// TODO: slå ihop hyllor om man ändrar till en beintlig hylla
void
edit_shelf_name (tree_t * db, elem_t shelf_to_edit, char *name, int index,
                 list_t * shelves)
{
    shelf_info_t *info = info_initiate ();
    char *new_id = find_available_shelf (db, name, info);

    if (info_owner (info) == SELF)
        {
            merge_shelves (info, shelf_to_edit, shelves);
        }
    else
        {
            char *tmp = shelf_id (shelf_to_edit.p);
            set_shelf_id (shelf_to_edit.p, new_id);
            release (tmp);
        }
    release (info);
}

/// Redigerar en hylla tillhörande en vara
/// \param item varan att ändra hylla på
void
edit_shelves (tree_t * db, elem_t item, char edit_choice)
{
    list_t *shelves = item_shelves (item.p);
    print_shelves (shelves);

    elem_t shelf_to_edit;
    int index_in_list = select_existing_shelf (item, &shelf_to_edit);
    char *name = item_name (item.p);

    if (edit_choice == 'L')
        {
            edit_shelf_name (db, shelf_to_edit, name, index_in_list, shelves);
        }
    else
        {
            int new_amount = AskAmount;
            shelf_set_amount (shelf_to_edit.p, new_amount);
        }
}

/// Redigerar beskrivningen på en vara
/// \param item varan att ändra beskrivningen på
void
edit_description (item_t * item)
{
    printf ("Nuvarande beskrivning: %s\n"
            "-----------------------------------------------------\n",
            item_descr (item));
    char *new_descr = ask_question_string ("Ny beskrivning: ");
    item_set_description (item, new_descr);
}

/// Redigerar priset på en vara
/// \param item varan att ändra priset på
void
edit_price (item_t * item)
{
    printf ("Nuvarande pris: %d\n"
            "-----------------------------------------------------\n",
            item_price (item));
    int new_price = ask_question_int ("Nytt pris: ");
    item_set_price (item, new_price);
}

/// Redigerar priset på en vara
/// \param item varan att ändra namnet på
void
edit_name (item_t * item)
{
    printf ("Nuvarande namn: %s\n"
            "-----------------------------------------------------\n",
            item_name (item));
    char *new_name = ask_question_string ("Nytt namn: ");
    item_set_name (item, new_name);
}

/// Redigerar en vara i databasen
/// \param tree databasen
/// \param item varan som ska redigeras
/// \param undo strukt för att ångra
void
edit_item_aux (tree_t * db, elem_t item, undo_action_t * undo,
               char edit_choice)
{
    switch (edit_choice)
        {
        case 'P':
        {
            edit_price (item.p);
            undo_set_edit_new (undo, item);
            //        undo->edit_new = item_deep_copy(item);
            break;
        }
        case 'B':
        {
            edit_description (item.p);
            undo_set_edit_new (undo, item);
            //undo->edit_new = item_deep_copy(item);
            break;
        }
        case 'N':
        {
            elem_t result  = { .p = "dummy"};
            elem_t copy    = item_deep_copy (item);
            elem_t old_key = { .p = item_name (item.p)};
            edit_name (copy.p);

            tree_remove (db, old_key, &result);
            item_free (result);

            elem_t key = { .p = allocate_string (item_name (copy.p))};
            tree_insert (db, key, copy);
            undo_set_edit_new (undo, copy);
            //        undo->edit_new = item_deep_copy(copy);
            break;
        }
        default:
        {
            edit_shelves (db, item, edit_choice);
            undo_set_edit_new (undo, item);
            //undo->edit_new = item_deep_copy(item);
            break;
        }
        }
}

void
remove_shelves (list_t * shelves)
{
    print_shelves (shelves);
    int index_to_remove =
        -1;	// list_contains returnerar -1 om hyllan inte finns i listan

    do
        {
            elem_t shelf_to_remove = {.p = shelf_new (AskRemoveShelf, 0) };
            index_to_remove = list_contains (shelves, shelf_to_remove);

            if (index_to_remove == INVALID_INDEX)
                printf ("Ogiltigt val!\n");
            shelf_free (shelf_to_remove);

        }
    while (index_to_remove == INVALID_INDEX);

    list_remove (shelves, index_to_remove, true);
}


/// Tar bort en vara från databasen.
void
db_remove_item (tree_t * db, undo_action_t * undo)
{
    if (!list_db (db))
        return;

    //Välj vilken vara som ska tas bort
    elem_t item = select_by_index (db, tree_size (db));

    // select_by_index returnerar en nullpekare om man avbryter
    if (item.p)
        {
            // Hittils räknas handlingen som att vi editerar en hylla
            undo_reset (undo);
            undo_set_last_action (undo, EDIT);
            undo_set_edit_old (undo, item);

            elem_t key = {.p = item_name (item.p) };
            elem_t result;

            tree_get (db, key, &result);
            list_t *shelves = item_shelves (result.p);
            remove_shelves (shelves);

            if (list_length (shelves) == 0)
                {
                    // Är det den sista hyllan som tas bort blir det en borttagning av vara
                    elem_t removed = undo_get_edit_old (undo);

                    undo_set_removed (undo, removed);
                    undo_reset (undo);
                    undo_set_last_action (undo, REMOVE);

                    tree_remove (db, key, &result);
                    item_free (item);
                }

            else
                {
                    undo_set_edit_new (undo, item);
                }
        }
}


/// Lägger till en antal av en redan existerande vara i databasen
void
add_existing_item (tree_t * db, elem_t name)
{
    elem_t item;
    tree_get (db, name, &item);
    list_t *shelves = item_shelves ((item_t *) item.p);

    printf ("Varan finns redan i databasen.\n");
    print_item (item);

    shelf_info_t *info = info_initiate ();
    char *id = find_available_shelf (db, name.p, info);
    int amount = AskAmount;

    // Varan finns redan på vald hylla
    if (info_owner (info) == SELF)
        {
            elem_t target_shelf;
            elem_t temp = {.p = shelf_new (id, amount) };
            int index = list_contains (shelves, temp);

            list_get (shelves, index, &target_shelf);
            shelf_add_amount (target_shelf.p, amount);
            shelf_free (temp);
        }
    // Hyllan är ledig
    else
        {
            elem_t shelf = {.p = shelf_new (id, amount) };
            list_append (shelves, shelf);
        }
    release (info);
}

int
ask_new_amount ()
{
    int amount = 0;

    while (amount < 1)
        {
            amount = AskAmount;
            if (amount < 1)
                printf ("Antal varor måste vara positivt!\n");
        }
    return amount;
}

int
ask_new_price ()
{
    int price = -1;

    while (price < 0)
        {
            price = AskPrice;
            if (price < 0)
                printf ("Priset kan inte vara negativt!\n");
        }
    return price;
}

/// Lägger till en vara i databasen. Om varan redan finns väljer användaren
/// en hylla att lägga till på. Om hyllan redan innehåller varan läggs det nya
/// antalet till det existerande.
void
db_add_item (tree_t * db, undo_action_t * undo)
{
    printf ("\n=====NY VARA====\n");
    elem_t key = {.p = AskName };

    // Om varan redan finns i databasen
    if (tree_has_key (db, key))
        {
            elem_t item;
            tree_get (db, key, &item);

            undo_reset (undo);
            undo_set_last_action (undo, EDIT);
            undo_set_edit_old (undo, item);

            add_existing_item (db, key);
            undo_set_edit_new (undo, item);
            release (key.p);
        }
    // Om varan inte redan finns gör vi en ny vara
    else
        {
            shelf_info_t *info = info_initiate ();
            char *descr = AskDescription;
            int price = ask_new_price ();
            char *new_id = find_available_shelf (db, key.p, info);
            int amount = ask_new_amount ();

            elem_t item  = { .p = item_new (allocate_string (key.p), descr, price)};
            elem_t shelf = { .p = shelf_new (new_id, amount) };
            item_add_shelf (item.p, shelf);

            print_item (item);
            char answer = ConfirmAddition;

            switch (answer)
                {
                case 'N':
                {
                    item_free (item);
                    release (key.p);
                    break;
                }
                case 'J':
                {
                    undo_reset (undo);
                    undo_set_last_action (undo, ADD);
                    //            undo->last_action = ADD;
                    tree_insert (db, key, item);
                    undo_set_added (undo, item);
                    //            undo->added = item_deep_copy(item);
                    break;
                }

                default:
                    tree_insert (db, key, item);
                    edit_item (db, item, undo);
                    // Programmet återkommer från edit_item så vi kopierar
                    /// vad som ligger i edit_new och resetar undo.
                    undo_set_added (undo, undo_get_edit_new (undo));
                    //          undo->added = item_deep_copy(undo->edit_new);
                    undo_reset (undo);
                    undo_set_last_action (undo, ADD);
                    //          undo->last_action = ADD;
                    break;
                }
            release (info);
        }
}

/// Listar 20 varor i databasen med tillhörande index
void
list_20_items (int *counter, int size, tree_key_t * keys)
{
    for (int i = 0; i < 20 && *counter < size; ++i)
        {
            printf ("%d: %s\n", *counter + 1, (char *) keys[*counter].p);
            ++*counter;
        }
    printf ("\n");
}


/// Listar 20 varor åt gången tills användaren väljer en specifik vara
/// att inspektera eller avbryter
bool
list_db (tree_t * db)
{

    printf ("=====DATABAS====\n");
    if (tree_size (db) == 0)
        {
            printf ("Tom databas!\n");
            return false;
        }

    else
        {
            tree_key_t *keys = tree_keys (db);
            int size = tree_size (db);
            int counter = 0;
            char answer;

            while (counter < size)
                {
                    list_20_items (&counter, size, keys);

                    if (counter < size)
                        {
                            answer =
                                ask_question_menu ("Vill du se fler varor? [J]a, [N]ej",
                                                   "JjNn");

                            if (answer == 'J')
                                continue;
                            else
                                break;
                        }
                }
            release (keys);
            return true;
        }
}


// Konverterar ett intervall av heltal till en sträng och lägger till 'a' samt 'A' i slutet av den.
/// Strängen används för att välja ett intervall av index i menyn.
int *
index_menu_choices (int item_count)
{
    //Heltals-motsvarigheten till '1'
    int *menu_choices = (int *) allocate_array (item_count, sizeof (int *),
                        NULL);	// Char-array för alla index med två extra platser för 'A' och 'a'
    retain (menu_choices);
    for (int i = 0; i < item_count; ++i)
        {
            * (menu_choices + i) = i + 1;
        }
    return menu_choices;
}

bool
valid_index (int *valid_indexes, int array_length, int index)
{
    for (int i = 0; i < array_length; i++)
        {
            if (valid_indexes[i] == index)
                return true;
        }

    return false;
}

/// Låter användaren välja en vara i listan efter index eller avbryta och återgå
/// till huvudmenyn
elem_t
select_by_index (tree_t * db, int item_count)
{
    int *menu_choices;
    int answer;

    do
        {
            menu_choices = index_menu_choices (item_count);
            answer = ask_question_int ("Välj index: ");

        }
    while (!valid_index (menu_choices, item_count, answer));

    elem_t result = {.p = NULL };

    elem_t *elems = tree_elements (db);
    result = elems[answer - 1];
    release (elems);


    release (menu_choices);	// eventuellt minnesläckage här
    return result;
}

/// Listar databasens varor med index på formen
///
/// 1. vara1
/// 2. vara2
/// .
/// .
/// N. varaN
void
list_and_select_item (tree_t * db)
{
    list_db (db);
    if (tree_size (db) != 0)
        {
            elem_t item = select_by_index (db, tree_size (db));
            if (item.p)
                print_item (item);
        }
}


void
edit_item (tree_t * db, elem_t item, undo_action_t * undo)
{
    print_item (item);
    char edit_choice = AskWhatToEdit;

    if (edit_choice == 'A')
        return;

    else
        {
            undo_reset (undo);
            undo_set_last_action (undo, EDIT);
            //      undo->last_action = EDIT;
            undo_set_edit_old (undo, item);
            //y      undo->edit_old = item_deep_copy(item);
            edit_item_aux (db, item, undo, edit_choice);

        }
}

/// Huvudfunktion för att redigera en vara i databasen
void
edit_db (tree_t * db, undo_action_t * undo)
{
    if (!list_db (db))
        return;

    // undo->last_action = EDIT;
    elem_t item = select_by_index (db, tree_size (db));

    if (item.p)
        {
            edit_item (db, item, undo);
        }
}

void
db_check_sorting (tree_t ** db)
{
    if (!get_balance (*db))
        {
            printf ("Trädet är inte balanserat.\n");
        }
    else
        {
            printf ("Trädet är balanserat.\n");
        }
}


/// Returnerar antalet poster i databasfilen
int
db_size (FILE * f)
{
    int size = 0;
    fscanf (f, "%d ", &size);
    return size;
}

bool
write_item_to_file (tree_key_t key, elem_t elem, void *f)
{
    char *name = item_name ((item_t *) elem.p);
    char *descr = item_descr ((item_t *) elem.p);
    int price = item_price ((item_t *) elem.p);
    list_t *shelves = item_shelves ((item_t *) elem.p);

    int number_of_shelves = list_length (shelves);
    fprintf (f, "%s\n", name);
    fprintf (f, "%s\n", descr);
    fprintf (f, "%d\n", price);
    fprintf (f, "%d\n", number_of_shelves);

    elem_t shelf;
    for (int i = 0; i < number_of_shelves; ++i)
        {
            list_get (shelves, i, &shelf);
            fprintf (f, "%s\n %d\n", shelf_id (shelf.p), shelf_amount (shelf.p));
        }
    return true;
}

void
save_db (tree_t * db, char *path)
{
    FILE *f = fopen (path, "w");

    if (f)
        {
            int size = tree_size (db);
            fprintf (f, "%d\n", size);
            tree_apply (db, preorder, write_item_to_file, f);
        }

    else
        {
            printf ("Error opening file: %s\n", path);
        }

    fclose (f);
}

void
stripline (char *c)
{
    char *x = strchr (c, '\n');
    *x = '\0';
}

item_t *
load_item_from_file (FILE * f)
{
    char *name = allocate_array (100, sizeof (char), NULL);
    char *descr = allocate_array (100, sizeof (char), NULL);
    char *cost = allocate_array (20, sizeof (char), NULL);

    fgets (name, 100, f);
    fgets (descr, 100, f);
    fgets (cost, 20, f);

    stripline (name);
    stripline (descr);

    item_t *item = item_new (allocate_string (name), allocate_string (descr),
                             atoi (cost));
    int shelf_count = 0;
    fscanf (f, "%d\n", &shelf_count);
    list_t *shelves = item_shelves (item);

    for (int i = 0; i < shelf_count; ++i)
        {

            char *shelf_name   = allocate_array (4, sizeof (char), NULL);
            char *shelf_amount = allocate_array (20, sizeof (char), NULL);

            fgets (shelf_name, 4, f);
            fgets (shelf_amount, 20, f);

            stripline (shelf_name);
            stripline (shelf_amount);

            elem_t shelf = { .p = shelf_new (allocate_string (shelf_name), atoi (shelf_amount))};
            list_append (shelves, shelf);
            release (shelf_name);
            release (shelf_amount);
        }
    deallocate (name);
    deallocate (descr);
    deallocate (cost);
    return item;
}

tree_t *
read_db (char *path)
{
    FILE *f = fopen (path, "r");
    int size = db_size (f);
    tree_t *db = tree_new (item_copy, key_free, item_free, item_compare);

    for (int i = 0; i < size; ++i)
        {
            elem_t item = { .p = load_item_from_file (f) };
            elem_t key  = { .p = allocate_string (item_name (item.p))};
            tree_insert (db, key, item);
        }

    fclose (f);
    return db;
}

/// Huvudloop för programmet.
void
event_loop (char *path)
{
    tree_t *db = read_db (path);
    char choice;
    undo_action_t *undo = undo_new ();

    do
        {
            cleanup ();
            print_main_menu ();

            choice = MainMenuChoice;

            switch (choice)
                {
                case 'L':
                {
                    db_add_item (db, undo);
                    break;
                }
                case 'T':
                {
                    db_remove_item (db, undo);
                    break;
                }
                case 'R':
                {
                    edit_db (db, undo);
                    break;
                }
                case 'G':
                {
                    undo_last_action (db, undo);
                    break;
                }
                case 'H':
                {
                    list_and_select_item (db);
                    break;
                }
                case 'S':
                {
                    tree_balance (&db);
                    break;
                }
                case 'K':
                {
                    db_check_sorting (&db);
                    break;
                }
                default:
                    printf ("Avslutar...\n");
                    break;
                }

        }
    while (choice != 'A');


    save_db (db, path);
    undo_free (undo);
    tree_delete (db, true, true);
    shutdown ();
}


int
main (int argc, char *argv[])
{
    event_loop (argv[1]);
    return 0;
}
