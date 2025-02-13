#pragma once

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_dialog.h>
#include <json.hpp>

#include "interface.h"

static std::string save_path;
constexpr SDL_DialogFileFilter save_filter = {"gemuPG grid file (*.gemupg)", "gemupg"};

void save(bool save_as);
void load();