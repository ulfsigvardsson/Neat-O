#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "../src/refmem.h"

enum owner
{ OTHER, NONE, SELF };


typedef struct shelf_info shelf_info_t;

/// Skapar en ny instans av hyllinfo-strukten
shelf_info_t *shelf_info_new ();

/// Returnerar en HI-strukts existens-värde.
bool info_exists (shelf_info_t * info);

/// Ändrar HI-struktens existensvärde
void set_info_exists (bool val, shelf_info_t * info);

/// Returnerar vilket objekt som "äger" hyllan, tills det att
/// programmet har hittat en ägare är värdet NONE.
enum owner info_owner (shelf_info_t * info);

/// Ändrar 'hårt' på ägarstatusen för en hylla med en owner-enum som parameter
void set_info_owner (enum owner owner, shelf_info_t * info);

/// Fastställer ägaren till en hylla beroende på om parametersträngarna
/// är lika, enbart applicerbart om hyllan faktiskt finns
void set_shelf_owner (shelf_info_t * info, char *current_name,
                      char *lookup_name);

/// Returnerar namnet på varan som ska läggas till
char *info_name (shelf_info_t * info);

/// Ändrar informationen om namnet på varan som ska läggas till
void set_info_name (char *name, shelf_info_t * info);

/// Returnerar namnet på hyllan som försöker läggas till
char *info_id (shelf_info_t * info);

/// Ändrar informationen om namnet på hyllan som ska läggas till
void set_info_id (char *id, shelf_info_t * info);

/// Initierar en HI-strukt med standardvärden där owner == NULL
shelf_info_t *info_initiate (void);
