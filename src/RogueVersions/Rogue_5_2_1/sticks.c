/*
 * Functions to implement the various sticks one might find
 * while wandering around the dungeon.
 *
 * @(#)sticks.c	4.22 (Berkeley) 5/19/82
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980, 1981, 1982 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include <curses.h>
#include <ctype.h>
#include <string.h>
#include "rogue.h"

/*
 * fix_stick:
 *	Set up a new stick
 */
fix_stick(cur)
register THING *cur;
{
    if (strcmp(ws_type[cur->o_which], "staff") == 0)
	strcpy(cur->o_damage,"2d3");
    else
	strcpy(cur->o_damage,"1d1");
    strcpy(cur->o_hurldmg,"1d1");

    cur->o_charges = 3 + rnd(5);
    switch (cur->o_which)
    {
	case WS_HIT:
	    cur->o_hplus = 100;
	    cur->o_dplus = 3;
	    strcpy(cur->o_damage,"1d8");
	when WS_LIGHT:
	    cur->o_charges = 10 + rnd(10);
    }
}

/*
 * do_zap:
 *	Perform a zap with a wand
 */
do_zap()
{
    register THING *obj, *tp;
    register int y, x;
    register char *name;

    if ((obj = get_item("zap with", STICK)) == NULL)
	return;
    if (obj->o_type != STICK)
    {
	after = FALSE;
	msg("you can't zap with that!");
	return;
    }
    if (obj->o_charges == 0)
    {
	msg("nothing happens");
	return;
    }
    switch (obj->o_which)
    {
	case WS_LIGHT:
	    /*
	     * Reddy Kilowat wand.  Light up the room
	     */
	    ws_know[WS_LIGHT] = TRUE;
	    if (proom->r_flags & ISGONE)
		msg("the corridor glows and then fades");
	    else
	    {
		proom->r_flags &= ~ISDARK;
		/*
		 * Light the room and put the player back up
		 */
		enter_room(&hero);
		addmsg("the room is lit");
		if (!terse)
		    addmsg(" by a shimmering blue light");
		endmsg();
	    }
	when WS_DRAIN:
	    /*
	     * Take away 1/2 of hero's hit points, then take it away
	     * evenly from the monsters in the room (or next to hero
	     * if he is in a passage)
	     */
	    if (pstats.s_hpt < 2)
	    {
		msg("you are too weak to use it");
		return;
	    }
	    else
		drain();
	when WS_POLYMORPH:
	case WS_TELAWAY:
	case WS_TELTO:
	case WS_CANCEL:
	{
	    register byte monster, oldch;
	    register int rm;

	    y = hero.y;
	    x = hero.x;
	    while (step_ok(winat(y, x)))
	    {
		y += delta.y;
		x += delta.x;
	    }
	    if ((tp = moat(y, x)) != NULL)
	    {
		register byte omonst;

		omonst = monster = tp->t_type;
		if (monster == 'F')
		    player.t_flags &= ~ISHELD;
		if (obj->o_which == WS_POLYMORPH)
		{
		    register THING *pp;

		    pp = tp->t_pack;
		    detach(mlist, tp);
		    if (see_monst(tp))
			MVADDCH(y, x, chat(y, x));
		    oldch = tp->t_oldch;
		    delta.y = y;
		    delta.x = x;
		    new_monster(tp, monster = rnd(26) + 'A', &delta);
		    if (see_monst(tp))
			MVADDCH(y, x, monster);
		    tp->t_oldch = oldch;
		    tp->t_pack = pp;
		    ws_know[WS_POLYMORPH] |= (monster != omonst);
		}
		else if (obj->o_which == WS_CANCEL)
		{
		    tp->t_flags |= ISCANC;
		    tp->t_flags &= ~(ISINVIS|CANHUH);
		    tp->t_disguise = tp->t_type;
		}
		else
		{
		    if (ismonst(MVINCH(y,x)))
			MVADDCH(y, x, tp->t_oldch);
		    if (obj->o_which == WS_TELAWAY)
		    {
			do
			{
			    rm = rnd_room();
			    rnd_pos(&rooms[rm], &tp->t_pos);
			} until (winat(tp->t_pos.y, tp->t_pos.x) == FLOOR);
			tp->t_room = roomin(&tp->t_pos);
			tp->t_oldch = MVINCH(tp->t_pos.y, tp->t_pos.x);
			if (see_monst(tp))
			    MVADDCH(tp->t_pos.y, tp->t_pos.x, tp->t_disguise);
			else if (on(player, SEEMONST))
			{
			    standout();
			    MVADDCH(tp->t_pos.y, tp->t_pos.x, tp->t_disguise);
			    standend();
			}
		    }
		    else
		    {
			tp->t_pos.y = hero.y + delta.y;
			tp->t_pos.x = hero.x + delta.x;
		    
		        if (tp->t_pos.y != y || tp->t_pos.x != x)
			    tp->t_oldch = MVINCH(tp->t_pos.y, tp->t_pos.x);
		    }
		    moat(y, x) = NULL;
		    moat(tp->t_pos.y, tp->t_pos.x) = tp;
		    if (tp->t_type == 'F')
			player.t_flags &= ~ISHELD;
		}
		tp->t_dest = &hero;
		tp->t_flags |= ISRUN;
	    }
	}
	when WS_MISSILE:
	{
	    THING bolt;

	    ws_know[WS_MISSILE] = TRUE;
	    bolt.o_type = '*';
	    strcpy(bolt.o_hurldmg,"1d4");
	    bolt.o_hplus = 100;
	    bolt.o_dplus = 1;
	    bolt.o_flags = ISMISL;
	    if (cur_weapon != NULL)
		bolt.o_launch = cur_weapon->o_which;
	    do_motion(&bolt, delta.y, delta.x);
	    if ((tp = moat(bolt.o_pos.y, bolt.o_pos.x)) != NULL && !save_throw(VS_MAGIC, tp))
		    hit_monster(unc(bolt.o_pos), &bolt);
	    else if (terse)
		msg("missle vanishes");
	    else
		msg("the missle vanishes with a puff of smoke");
	}
	when WS_HIT:
	    delta.y += hero.y;
	    delta.x += hero.x;
	    if ((tp = moat(delta.y, delta.x)) != NULL)
	    {
		if (rnd(20) == 0)
		{
		    strcpy(obj->o_damage,"3d8");
		    obj->o_dplus = 9;
		}
		else
		{
		    strcpy(obj->o_damage,"1d8");
		    obj->o_dplus = 3;
		}
		fight(&delta, tp->t_type, obj, FALSE);
	    }
	when WS_HASTE_M:
	case WS_SLOW_M:
	    y = hero.y;
	    x = hero.x;
	    while (step_ok(winat(y, x)))
	    {
		y += delta.y;
		x += delta.x;
	    }
	    if ((tp = moat(y, x)) != NULL)
	    {
		if (obj->o_which == WS_HASTE_M)
		{
		    if (on(*tp, ISSLOW))
			tp->t_flags &= ~ISSLOW;
		    else
			tp->t_flags |= ISHASTE;
		}
		else
		{
		    if (on(*tp, ISHASTE))
			tp->t_flags &= ~ISHASTE;
		    else
			tp->t_flags |= ISSLOW;
		    tp->t_turn = TRUE;
		}
		delta.y = y;
		delta.x = x;
		runto(&delta, &hero);
	    }
	when WS_ELECT:
	case WS_FIRE:
	case WS_COLD:
	    if (obj->o_which == WS_ELECT)
		name = "bolt";
	    else if (obj->o_which == WS_FIRE)
		name = "flame";
	    else
		name = "ice";
	    fire_bolt(&hero, &delta, name);
	    ws_know[obj->o_which] = TRUE;
	when WS_NOP:
	otherwise:
	    msg("what a bizarre schtick!");
    }
    obj->o_charges--;
}

/*
 * drain:
 *	Do drain hit points from player shtick
 */
drain()
{
    register THING *mp;
    register int cnt;
    register struct room *corp;
    register THING **dp;
    register bool inpass;
    static THING *drainee[40];

    /*
     * First cnt how many things we need to spread the hit points among
     */
    cnt = 0;
    if (chat(hero.y, hero.x) == DOOR)
	corp = &passages[flat(hero.y, hero.x) & F_PNUM];
    else
	corp = NULL;
    inpass = (proom->r_flags & ISGONE);
    dp = drainee;
    for (mp = mlist; mp != NULL; mp = next(mp))
	if (mp->t_room == proom || mp->t_room == corp ||
	    (inpass && chat(mp->t_pos.y, mp->t_pos.x) == DOOR &&
	    &passages[flat(mp->t_pos.y, mp->t_pos.x) & F_PNUM] == proom))
		*dp++ = mp;
    if ((cnt = dp - drainee) == 0)
    {
	msg("you have a tingling feeling");
	return;
    }
    *dp = NULL;
    pstats.s_hpt /= 2;
    cnt = pstats.s_hpt / cnt;
    /*
     * Now zot all of the monsters
     */
    for (dp = drainee; *dp; dp++)
    {
	mp = *dp;
	if ((mp->t_stats.s_hpt -= cnt) <= 0)
	    killed(mp, see_monst(mp));
	else
	    runto(&mp->t_pos, &hero);
    }
}

/*
 * fire_bolt:
 *	Fire a bolt in a given direction from a specific starting place
 */
fire_bolt(start, dir, name)
coord *start, *dir;
char *name;
{
    register byte dirch, ch;
    register THING *tp;
    register bool hit_hero, used, changed;
    register int i, j;
    coord pos;
    coord spotpos[BOLT_LENGTH];
    THING bolt;

    bolt.o_type = WEAPON;
    bolt.o_which = FLAME;
    strcpy(bolt.o_hurldmg,"6d6");
    bolt.o_hplus = 100;
    bolt.o_dplus = 0;
    bolt.o_flags = 0;
    w_names[FLAME] = name;
    switch (dir->y + dir->x)
    {
	case 0: dirch = '/';
	when 1: case -1: dirch = (dir->y == 0 ? '-' : '|');
	when 2: case -2: dirch = '\\';
    }
    pos = *start;
    hit_hero = (start != &hero);
    used = FALSE;
    changed = FALSE;
    for (i = 0; i < BOLT_LENGTH && !used; i++)
    {
	pos.y += dir->y;
	pos.x += dir->x;
	ch = winat(pos.y, pos.x);
	spotpos[i] = pos;
	switch (ch)
	{
	    case DOOR:
		/*
 		 * this code is necessary if the hero is on a door
 		 * and he fires at the wall the door is in, it would
 		 * otherwise loop infinitely
 		 * It is also needed if a dragon flames at the hero.
 		 * If the hero is at a door, the dragon flame would bounce
 		 * and could kill other monsters inadvertly.
 		 */
 		if (ce(hero, pos))
 		    goto def;
 		/* FALLTHROUGH */

	    case VWALL:
	    case HWALL:
#ifdef USE_PC_GFX
        case LLWALL: case LRWALL: case URWALL: case ULWALL:
#endif
	    case ' ':
		if (!changed)
		    hit_hero = !hit_hero;
		changed = FALSE;
		dir->y = -dir->y;
		dir->x = -dir->x;
		i--;
		msg("the %s bounces", name);
		break;
	    default:
def:
		if (!hit_hero && (tp = moat(pos.y, pos.x)) != NULL)
		{
		    hit_hero = TRUE;
		    changed = !changed;
		    tp->t_oldch = chat(pos.y, pos.x);
		    if (!save_throw(VS_MAGIC, tp))
		    {
			bolt.o_pos = pos;
			used = TRUE;
			if (tp->t_type == 'D' && strcmp(name, "flame") == 0)
			{
			    addmsg("the flame bounces");
			    if (!terse)
				msg("off the dragon");
			    endmsg();
			}
			else
			    hit_monster(unc(pos), &bolt);
		    }
		    else if (ch != 'M' || tp->t_disguise == 'M')
		    {
			if (start == &hero)
			    runto(&pos, &hero);
			if (terse)
			    msg("%s misses", name);
			else
			    msg("the %s whizzes past the %s", name, monsters[ch-'A'].m_name);
		    }
		}
		else if (hit_hero && ce(pos, hero))
		{
		    hit_hero = FALSE;
		    changed = !changed;
		    if (!save(VS_MAGIC))
		    {
			if ((pstats.s_hpt -= roll(6, 6)) <= 0)
			    if (start == &hero)
				death('b');
			    else
				death(moat(start->y, start->x)->t_type);
			used = TRUE;
			if (terse)
			    msg("the %s hits", name);
			else
			    msg("you are hit by the %s", name);
		    }
		    else
			msg("the %s whizzes by you", name);
		}
		MVADDCH(pos.y, pos.x, dirch);
		refresh();
	}
    }
    for (j = 0; j < i; j++)
	MVADDCH(spotpos[j].y, spotpos[j].x, chat(spotpos[j].y, spotpos[j].x));
}

/*
 * charge_str:
 *	Return an appropriate string for a wand charge
 */
char *
charge_str(obj)
register THING *obj;
{
    static char buf[20];

    if (!(obj->o_flags & ISKNOW))
	buf[0] = '\0';
    else if (terse)
	sprintf(buf, " [%d]", obj->o_charges);
    else
	sprintf(buf, " [%d charges]", obj->o_charges);
    return buf;
}
