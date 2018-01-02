
#ifndef ITEM_H
#define ITEM_H
#include "list.h"
#include "common.h"
#include "../src/refmem.h"

//! \brief Typedef för lagervaru-strukten
typedef struct item item_t;

//! \brief Typedef för hyll-strukten
typedef struct shelf shelf_t;


/// Allokerar plats för en ny vara och sätter dess information
/// \param name: namn på varan
/// \param descr: beskrivning på varan
/// \param price: pris på varan
/// \param shelf_id: hyllplatsen på formen xy där x är en versal och y ett heltal
/// \param amount: antalet varor av denna typ
item_t *item_new(char *name, char *descr, int price);

item_t *item_empty();
shelf_t *shelf_empty();


//! Skapar en ny hylla
/*!
 * Allokerar plats för en ny hylla på heapen med id och antal varor enligt parametrar.
 * \param id Hyllans id
 * \param amount Antal varor att lagra på hyllan
 * \returns pekare till den nya hyllan 
 */
shelf_t *shelf_new(char *id, int amount); //Done

//! Ändrar namnet för ett vara
/*! \param item varan att editera
 *  \param descr ny beskrivning på varan
 */
void item_set_name(item_t *item, char *name); //Done

//! Ändrar varasbeskrivningen för ett vara
/*! \param item varan att editera
 *  \param descr ny beskrivning på varan
 */
void item_set_description(item_t *item, char *descr); //Done

//! Ändrar priset för en vara
/*! \param ite: varan att editera
 *  \param price nytt pris på varan
 */
void item_set_price(item_t *item, int price); //Done

//! Ändrar antalet lagrade varor av en viss typ
/*! \param item varan att editera
 *  \param amount det nya antalet varor 
 */ 
void item_set_amount(shelf_t *shelf, int amount); //cunit hittar inte denna funktion

/// Lägger till en mängd på en hylla
///
/// \param shelf the shelf
/// \param amount den mängd som ska läggas till
void shelf_add_amount(shelf_t *shelf, int amount); //Done


//! Ändrar hyllplats för en vara
/*! \param item varan att editera
 *  \param id det nya hyllnamnet för varan
 */
void item_set_shelf(shelf_t *shelf, char *id); //Done

/// Lägger en lista med shelves till ett item
///
/// \param item the item
/// \param listan med shelves som ska läggas till

void item_set_shelves(item_t *item, list_t *shelves); //Done

//! Returnerar namnet på en vara
/*! \param item varan att returnera namnet för
 *  \returns varans namn
 */
char *item_name(item_t *item); //Done

//! Returnerar beskrivningen på en vara
/*! \param item varan att returnera beskrivningen för
 *  \returns varans beskrivning 
 */
char *item_descr(item_t *item); //Done

//! Returnerar priset på ett item
/*! \param item varan att returnera priset för
 *  \returns varans pris 
 */
int item_price(item_t *item); //Done

/// Returnerar en lista av hyllor där en vara finns
/// \param item: varan att lista hyllplatser för
list_t *item_shelves(item_t *item); //Done, testas i item_set_shelves_test

//! Returnerar namnet på en hylla
/*! \param shelf hyllan att returnera namnet på
 *  \returns hyllans namn
 */
char *shelf_id(shelf_t *shelf); //Done
void shelf_set_amount(shelf_t *shelf, int amount);

void set_shelf_id(shelf_t *shelf, char *id);

//! Returnerar antalet lagrade varor på en hylla
/*! \param shelf hyllan för vilken att returnera antalet varor 
 *  \returns antal varor på hyllan
 */
int shelf_amount(shelf_t *shelf); //Done

elem_t shelf_deep_copy(elem_t shelf);
int item_compare(elem_t elem1, elem_t elem2);
elem_t item_copy(elem_t item);
void item_free(elem_t elem);

/// Frigör minne för en hylla
///
/// \param shelf hyllan som ska tas bort
void shelf_free(elem_t elem);

void shelf_free_aux(shelf_t *shelf);

/// Jämför id på två hyllor
///
/// \param elem1 dent första element där .p är en shelf
/// \param elem dent första element där .p är en shelf
/// \returns int 0 ifall lika, -1 eller 1 annars
int shelf_compare(elem_t elem1, elem_t elem2); //Done

/// Kopierar en shelf och lägger den i ett element
///
/// \param shelf hyllan som ska kopieras
elem_t shelf_copy(elem_t shelf);

/// Allokerar minne för och kopierar ett item
///
/// \param from det item som ska kopieras 
elem_t item_deep_copy(elem_t item);

void key_free(elem_t elem);
void item_add_shelf(item_t *item, elem_t shelf);

#endif
