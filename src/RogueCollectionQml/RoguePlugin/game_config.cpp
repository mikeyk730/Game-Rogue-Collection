#include "game_config.h"
#include "environment.h"

TileConfig tilemap_v1 = { "tilemap_v1.bmp", 61, 2 };
TileConfig tilemap_v2 = { "tilemap_v2.bmp", 61, 2 };
TileConfig tilemap_v3 = { "tilemap_v3.bmp", 61, 2 };
TileConfig tilemap_v4 = { "tilemap_v4.bmp", 61, 2 };

TextConfig alt_text("tilemap_text.bmp", QSize(16, 16));
TextConfig boxy_text("boxy.bmp", QSize(16, 16));

                                 //name      text_cfg        tile_cfg      unix    color   animate
GraphicsConfig unix_gfx =        { "unix",   0,              0,            true,   false,  false };
GraphicsConfig pc_gfx =          { "pc",     0,              0,            false,  true,   true  };
GraphicsConfig tilemap_v1_gfx =  { "tiles",  &tilemap_text,  &tilemap_v1,  false,  false,  false };
GraphicsConfig tilemap_v2_gfx =  { "tiles",  &tilemap_text,  &tilemap_v2,  false,  false,  false };
GraphicsConfig tilemap_v3_gfx =  { "tiles",  &tilemap_text,  &tilemap_v3,  false,  false,  false };
GraphicsConfig tilemap_v4_gfx =  { "tiles",  &tilemap_text,  &tilemap_v4,  false,  false,  false };
GraphicsConfig boxy_gfx =        { "boxy",   &boxy_text,     0,            false,  true,   true  };

std::vector<GameConfig> s_options = {
    { "PC Rogue 1.48",    "Rogue_PC_1_48.dll", {80,25}, {40,25}, true,  false, { pc_gfx, tilemap_v4_gfx, boxy_gfx, unix_gfx } },
    { "PC Rogue 1.1",     "Rogue_PC_1_48.dll", {80,25}, {40,25}, true,  false, { pc_gfx, tilemap_v3_gfx, boxy_gfx, unix_gfx } },
    { "Unix Rogue 5.4.2", "Rogue_5_4_2.dll",   {80,25}, {80,24}, false, true,  { unix_gfx, pc_gfx, tilemap_v2_gfx, boxy_gfx } },
    { "Unix Rogue 5.2.1", "Rogue_5_2_1.dll",   {80,25}, {70,22}, true,  true,  { unix_gfx, pc_gfx, tilemap_v1_gfx, boxy_gfx } },
    { "Unix Rogue 3.6.3", "Rogue_3_6_3.dll",   {80,25}, {70,22}, true,  true,  { unix_gfx, pc_gfx, tilemap_v1_gfx, boxy_gfx } },
};

GameConfig GetGameConfig(int i)
{
    return s_options[i];
}

int GetGameIndex(const std::string& name)
{
    for (int i = 0; i < s_options.size(); ++i){
        if (s_options[i].name == name) {
            return i;
        }
    }
    return -1;
}

TextConfig::TextConfig()
{
}

TextConfig::TextConfig(const QString &imagefile, QSize layout) :
    imagefile_(imagefile),
    layout_(layout)
{
}

QString TextConfig::imagefile() const
{
    return imagefile_;
}

QSize TextConfig::layout() const
{
    return layout_;
}
