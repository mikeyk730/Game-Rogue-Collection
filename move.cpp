//Hero movement commands
//move.c      1.4 (A.I. Design)       12/22/84
#include <ctype.h>

#include "rogue.h"
#include "move.h"
#include "weapons.h"
#include "output_interface.h"
#include "io.h"
#include "rip.h"
#include "wizard.h"
#include "monsters.h"
#include "rooms.h"
#include "misc.h"
#include "main.h"
#include "fight.h"
#include "mach_dep.h"
#include "level.h"
#include "rings.h"
#include "scrolls.h"
#include "pack.h"
#include "room.h"
#include "game_state.h"
#include "hero.h"
#include "monster.h"

//Used to hold the new hero position
Coord new_position;

//do_run: Start the hero running
bool do_run(byte ch)
{
    game->modifiers.m_running = true;
    game->run_character = ch;
    return false;
}

bool is_gone(Room* rp)
{
    return ((rp->is_gone()) && (rp->is_maze()) == 0);
}

//diag_ok: Check to see if the move is legal if it is diagonal
int diag_ok(const Coord orig_pos, const Coord new_pos)
{
    if (new_pos.x == orig_pos.x || new_pos.y == orig_pos.y)
        return true;
    return (step_ok(game->level().get_tile({ orig_pos.x,new_pos.y })) && step_ok(game->level().get_tile({ new_pos.x,orig_pos.y })));
}


void finish_do_move(int fl)
{
    game->level().draw_char(game->hero().pos);
    if ((fl&F_PASS) && (game->level().get_tile(game->oldpos) == DOOR || (game->level().is_maze(game->oldpos))))
        leave_room(new_position);
    if ((fl&F_MAZE) && (game->level().is_maze(game->oldpos)) == 0)
        enter_room(new_position);
    game->hero().pos = new_position;
}

bool is_passage_or_door(Coord p)
{
    return (game->level().is_passage(p) || game->level().get_tile(p) == DOOR);
}

bool continue_vertical() {
    int dy;

    Coord n = north(game->hero().pos);
    Coord s = south(game->hero().pos);

    bool up = (game->hero().pos.y > 1 && is_passage_or_door(n));
    bool down = (game->hero().pos.y < maxrow() - 1 && is_passage_or_door(s));

    if (!(up^down))
        return false;

    if (up) {
        game->run_character = 'k';
        dy = -1;
    }
    else {
        game->run_character = 'j';
        dy = 1;
    }

    new_position.y = game->hero().pos.y + dy;
    new_position.x = game->hero().pos.x;
    return true;
}

bool continue_horizontal()
{
    int dx;
    const int COLS = game->screen().columns();

    Coord w = west(game->hero().pos);
    Coord e = east(game->hero().pos);

    bool left = (game->hero().pos.x > 1 && is_passage_or_door(w));
    bool right = (game->hero().pos.x < COLS - 2 && is_passage_or_door(e));

    if (!(left^right))
        return false;

    if (left) {
        game->run_character = 'h';
        dx = -1;
    }
    else {
        game->run_character = 'l';
        dx = 1;
    }

    new_position.y = game->hero().pos.y;
    new_position.x = game->hero().pos.x + dx;
    return true;
}

bool this_move_counts; //todo: remove

bool do_hit_boundary()
{
    if (game->modifiers.is_running() && is_gone(game->hero().room) && !game->hero().is_blind())
    {
        switch (game->run_character)
        {
        case 'h': case 'l':
        {
            if (continue_vertical())
                return false;
            break;
        }
        case 'j': case 'k':
        {
            if (continue_horizontal())
                return false;
            break;
        }
        }
    }
    this_move_counts = false;
    game->modifiers.m_running = false;
    return true;
}

bool do_move_impl(bool can_pickup)
{
    byte ch;
    int fl;

    //Check if he tried to move off the screen or make an illegal diagonal move, and stop him if he did. fudge it for 40/80 jll -- 2/7/84
    if (offmap(new_position))
        return !do_hit_boundary();
    if (!diag_ok(game->hero().pos, new_position)) {
        this_move_counts = false;
        game->modifiers.m_running = false;
        return false;
    }
    //If you are running and the move does not get you anywhere stop running
    if (game->modifiers.is_running() && equal(game->hero().pos, new_position)) {
        this_move_counts = false;
        game->modifiers.m_running = false;
    }
    fl = game->level().get_flags(new_position);
    ch = game->level().get_tile_or_monster(new_position);
    //When the hero is on the door do not allow him to run until he enters the room all the way
    if ((game->level().get_tile(game->hero().pos) == DOOR) && (ch == FLOOR))
        game->modifiers.m_running = false;
    if (!(fl&F_REAL) && ch == FLOOR) {
        ch = TRAP;
        game->level().set_tile(new_position, TRAP);
        game->level().set_flag(new_position, F_REAL);
    }
    else if (game->hero().is_held() && ch != 'F') { //TODO: can only attack the monster holding you, remove direct check for 'F'
        msg("you are being held");
        return false;
    }
    switch (ch)
    {
    case ' ': case VWALL: case HWALL: case ULWALL: case URWALL: case LLWALL: case LRWALL:
        return !do_hit_boundary();

    case DOOR:
        game->modifiers.m_running = false;
        if (game->level().is_passage(game->hero().pos))
            enter_room(new_position);
        finish_do_move(fl);
        return false;

    case TRAP:
        ch = handle_trap(new_position);
        if (ch == T_DOOR || ch == T_TELEP)
            return false;

    case PASSAGE:
        finish_do_move(fl);
        return false;

    case FLOOR:
        if (!(fl&F_REAL))
            handle_trap(game->hero().pos);
        finish_do_move(fl);
        return false;

    default:
        game->modifiers.m_running = false;
        if (isupper(ch) || game->level().monster_at(new_position))
            game->hero().fight(new_position, game->hero().get_current_weapon(), false);
        else
        {
            game->modifiers.m_running = false;
            finish_do_move(fl);

            if (ch != STAIRS && can_pickup)
                pick_up(ch);
        }
    }
    return false;
}

//do_move: Check to see that a move is legal.  If it is handle the consequences (fighting, picking up, etc.)
bool do_move(Coord delta, bool can_pickup)
{
    this_move_counts = true;

    game->modifiers.m_first_move = false;

    //if something went wrong, bail out on this level
    if (game->invalid_position) {
        game->invalid_position = false;
        msg("the crack widens ... ");
        descend("");
        return this_move_counts;
    }

    //skip the turn if the hero is stuck in a bear trap
    if (game->bear_trap_turns) {
        game->bear_trap_turns--;
        msg("you are still stuck in the bear trap");
        return this_move_counts;
    }

    //Do a confused move (maybe)
    if (game->hero().is_confused() && rnd(5) != 0)
        rndmove(&game->hero(), &new_position);
    else
    {
        new_position = game->hero().pos + delta;
    }

    bool more;
    do {
        more = do_move_impl(can_pickup);
    } while (more);

    return this_move_counts;
}


//door_open: Called to illuminate a room.  If it is dark, remove anything that might move.
void door_open(Room *room)
{
    int j, k;
    byte ch;
    Monster* monster;

    if (!(room->is_gone()) && !game->hero().is_blind())
        for (j = room->pos.y; j < room->pos.y + room->size.y; j++)
            for (k = room->pos.x; k < room->pos.x + room->size.x; k++)
            {
                ch = game->level().get_tile_or_monster({ k,j });
                if (isupper(ch))
                {
                    monster = wake_monster({ k,j });
                    if (monster->tile_beneath() == ' ' && !(room->is_dark()) && !game->hero().is_blind())
                        monster->reload_tile_beneath();
                }
            }
}

//handle_trap: The guy stepped on a trap.... Make him pay.
int handle_trap(Coord tc)
{
    byte tr;
    const int COLS = game->screen().columns();

    game->repeat_cmd_count = game->modifiers.m_running = false;
    game->level().set_tile(tc, TRAP);
    tr = game->level().get_trap_type(tc);
    game->was_trapped = 1;
    switch (tr)
    {
    case T_DOOR:
        descend("you fell into a trap!");
        break;

    case T_BEAR:
        game->bear_trap_turns += BEAR_TIME;
        msg("you are caught in a bear trap");
        break;

    case T_SLEEP:
        game->sleep_timer += SLEEP_TIME;
        game->hero().set_running(false);
        msg("a %smist envelops you and you fall asleep", noterse("strange white "));
        break;

    case T_ARROW:
        game->log("battle", "Arrow trap 1d6 attack on player");
        if (attempt_swing(game->hero().stats.level - 1, game->hero().stats.ac, 1))
        {
            if (!game->hero().decrease_hp(roll(1, 6), true)) {
                msg("an arrow killed you");
                death('a');
            }
            else
                msg("oh no! An arrow shot you");
        }
        else
        {
            Item *arrow;

            if ((arrow = new Weapon(ARROW, 0, 0)) != NULL)
            {
                arrow->count = 1;
                arrow->pos = game->hero().pos;
                fall(arrow, false);
            }
            msg("an arrow shoots past you");
        }
        break;

    case T_TELEP:
        game->hero().teleport();
        game->screen().mvaddch(tc, TRAP); //since the hero's leaving, look() won't put it on for us
        game->was_trapped++;//todo:look at this
        break;

    case T_DART:
        game->log("battle", "Dart trap 1d4 attack on player");
        if (attempt_swing(game->hero().stats.level + 1, game->hero().stats.ac, 1))
        {
            if (!game->hero().decrease_hp(roll(1, 4), true)) {
                msg("a poisoned dart killed you");
                death('d');
            }
            if (!game->hero().is_wearing_ring(R_SUSTSTR) && !save(VS_POISON))
                game->hero().adjust_strength(-1);
            msg("a dart just hit you in the shoulder");
        }
        else
            msg("a dart whizzes by your ear and vanishes");
        break;
    }
    clear_typeahead_buffer();
    return tr;
}

void descend(char *mesg)
{
    next_level();
    if (*mesg == 0) msg(" ");
    game->level().new_level(true);
    msg("");
    msg(mesg);
    if (!save(VS_LUCK))
    {
        msg("you are damaged by the fall");
        if (!game->hero().decrease_hp(roll(1, 8), true))
            death('f');
    }
}

//rndmove: Move in a random direction if the monster/person is confused
void rndmove(Agent *who, Coord *newmv)
{
    int x, y;
    byte ch;
    Item *obj;
    const int COLS = game->screen().columns();

    y = newmv->y = who->pos.y + rnd(3) - 1;
    x = newmv->x = who->pos.x + rnd(3) - 1;
    //Now check to see if that's a legal move.  If not, don't move. (I.e., bump into the wall or whatever)
    if (y == who->pos.y && x == who->pos.x) return;
    if ((y < 1 || y >= maxrow()) || (x < 0 || x >= COLS))
        goto bad;
    else if (!diag_ok(who->pos, *newmv))
        goto bad;
    else
    {
        ch = game->level().get_tile_or_monster({ x, y });
        if (!step_ok(ch))
            goto bad;
        if (ch == SCROLL)
        {
            for (auto it = game->level().items.begin(); it != game->level().items.end(); ++it) {
                obj = *it;
                if (y == obj->pos.y && x == obj->pos.x)
                    break;
            }
            if (is_scare_monster_scroll(obj))
                goto bad;
        }
    }
    return;

bad:

    (*newmv) = who->pos;
    return;
}
