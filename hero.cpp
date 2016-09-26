#include <string.h>

#include "rogue.h"
#include "game_state.h"
#include "hero.h"
#include "rip.h"
#include "main.h"
#include "misc.h"
#include "pack.h"
#include "armor.h"
#include "weapons.h"
#include "io.h"
#include "thing.h"
#include "rings.h"
#include "agent.h"
#include "food.h"


Hero::Hero()
{
    init_player();
}

std::string Hero::get_name()
{
    return m_name;
}

void Hero::set_name(const std::string& name)
{
    m_name = name;
}

int Hero::get_purse()
{
  return m_purse;
}

void Hero::adjust_purse(int delta)
{
  m_purse += delta;
  if (m_purse < 0) 
    m_purse = 0;
}

void Hero::toggle_wizard()
{
    m_wizard = !m_wizard;
    if (m_wizard)
        m_cheated = true;
    msg(is_wizard() ? "You are now a wizard!" : "You feel your magic powers fade away");
}

bool Hero::is_wizard() const
{
  return m_wizard;
}

bool Hero::did_cheat() const
{
  return m_cheated;
}

void Hero::ingest()
{
  if (food_left<0)
    food_left = 0;
  if (food_left>(STOMACH_SIZE-20))
    sleep_timer += 2+rnd(5);
  if ((food_left += HUNGER_TIME-200+rnd(400))>STOMACH_SIZE)
    food_left = STOMACH_SIZE;
  hungry_state = 0;
}

void Hero::digest()
{
  int oldfood, deltafood;

  if (food_left<=0)
  {
    if (food_left--<-STARVE_TIME) death('s');
    //the hero is fainting
    if (sleep_timer || rnd(5)!=0)
        return;
    sleep_timer += rnd(8)+4;
    game->hero().set_running(false);
    running = false;
    repeat_cmd_count = 0;
    hungry_state = 3;
    msg("%syou faint from lack of food", noterse("you feel very weak. "));
  }
  else
  {
    oldfood = food_left;
    //If you are in 40 column mode use food twice as fast (e.g. 3-(80/40) = 1, 3-(40/40) = 2 : pretty gross huh?)
    deltafood = ring_eat(LEFT)+ring_eat(RIGHT)+1;
    if (in_small_screen_mode()) deltafood *= 2;
    food_left -= deltafood;
    if (food_left<MORE_TIME && oldfood>=MORE_TIME) {
        hungry_state = 2;
        msg("you are starting to feel weak");
    }
    else if (food_left<2*MORE_TIME && oldfood>=2*MORE_TIME) {
        hungry_state = 1;
        msg("you are starting to get hungry");
    }
  }
}

//init_player: Roll up the rogue
void Hero::init_player()
{
  stats = { 16, 0, 1, 10, 12, "1d4", 12, 16 };
  food_left = HUNGER_TIME;

  //Give the rogue his weaponry.  First a mace.
  Item *obj;
  obj = new Weapon(MACE, 1, 1);
  obj->set_known();
  add_to_pack(obj, true);
  set_current_weapon(obj);

  //Now a +1 bow
  obj = new Weapon(BOW, 1, 0);
  obj->set_known();
  add_to_pack(obj, true);

  //Now some arrows
  obj = new Weapon(ARROW, 0, 0);
  obj->count = rnd(15)+25;
  obj->set_known();
  add_to_pack(obj, true);

  //And his suit of armor
  obj = new Armor(RING_MAIL, -1);
  obj->set_known();
  set_current_armor(obj);
  add_to_pack(obj, true);

  //Give him some food too
  obj = new Food(0);
  add_to_pack(obj, true);
}

int Hero::get_hungry_state()
{
  return hungry_state;
}

int Hero::get_food_left()
{
    return food_left;
}
