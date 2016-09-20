#include <fstream>
#include "rogue.h"
#include "game_state.h"
#include "random.h"
#include "input_interface.h"
#include "stream_input.h"
#include "captured_input.h"
#include "keyboard_input.h"
#include "hero.h"

GameState::GameState(int seed) :
m_seed(seed),
m_input_interface(new CapturedInput(new KeyboardInput())),
m_hero(new Hero)
{
    init_environment();
}

GameState::GameState(Random* random, std::istream& in)
{
    in.read((char*)&m_seed, sizeof(m_seed));
	random->set_seed(m_seed);
    m_input_interface.reset(new CapturedInput(new StreamInput(in, new KeyboardInput())));
    m_hero.reset(new Hero);

    init_environment();
}

GameState::~GameState()
{ }

void GameState::init_environment()
{
    m_environment["name"] = "Rodney";
    m_environment["scorefile"] = "rogue.scr";
    m_environment["savefile"] = "rogue.sav";
    m_environment["macro"] = "v";
    m_environment["fruit"] = "Slime Mold"; //move to Hero?
    m_environment["menu"] = "on";
    m_environment["screen"] = "";
    m_environment["levelnames"] = "on";
    m_environment["monstercfg"] = "monsters.opt";
}

void GameState::save_game(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary | std::ios::out);
    file.write((char*)&m_seed, sizeof(m_seed));
    //todo:serialize env
    m_input_interface->Serialize(file);
}

std::string GameState::get_environment(const std::string& key) const
{
    auto i = m_environment.find(key);
    if (i != m_environment.end()){
        return i->second;
    }
    return "";
}

void GameState::set_environment(const std::string& key, const std::string& value)
{
    m_environment[key] = value;
}

Random& GameState::random()
{
    return *m_random;
}

InputInterface& GameState::input_interface()
{
    return *m_input_interface;
}

Hero& GameState::hero()
{
    return *m_hero;
}

ScrollInfo& GameState::scrolls()
{
	return m_scrolls;
}

PotionInfo& GameState::potions()
{
	return m_potions;
}

RingInfo& GameState::rings()
{
	return m_rings;
}

StickInfo& GameState::sticks()
{
	return m_sticks;
}

//todo:
//monster config
//all extern/global variables
//all static variables