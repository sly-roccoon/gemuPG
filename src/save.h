#pragma once

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_dialog.h>
#include <json.hpp>

#include "interface.h"

class SaveLoad
{
public:
    static void clearPath()
    {
        save_path = "";
        Interface::getInstance().updateTitle(SaveLoad::getPath());
    }
    static std::string getPath() { return save_path; }

    static void save(bool save_as);
    static void load();
    static void loadPath(std::string path);

    static void saveJSON(std::string path, Grid *grid);
    static void loadJSON(std::string path, Grid *grid);

private:
    static std::string save_path;
    static const inline SDL_DialogFileFilter save_filter = {"gemuPG grid file (*.gemupg)", "gemupg"};
};
