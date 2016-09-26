#pragma once
#include "agent.h"

//runners: Make all the running monsters move.
void runners();


//get_room_from_position: Find what room some coordinates are in. NULL means they aren't in any room.
struct Room *get_room_from_position(Coord *cp);

//diag_ok: Check to see if the move is legal if it is diagonal
int diag_ok(const Coord *sp, const Coord *ep);


//find_dest: find the proper destination for the monster
Coord *find_dest(Agent *monster);
