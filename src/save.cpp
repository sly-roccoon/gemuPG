#include "save.h"
#include <fstream>

using json = nlohmann::json;

std::string SaveLoad::save_path = "";

void save_dialog_callback(void* userdata, const char* const * filelist, int filter);
void load_dialog_callback(void* userdata, const char* const * filelist, int filter);

void SaveLoad::save(bool save_as)
{
    Grid* grid = &Interface::getInstance().getGrid();
    if (save_as || save_path.empty())
    {
        SDL_Window* window = Interface::getInstance().getWindow();
        SDL_ShowSaveFileDialog(save_dialog_callback, (void*)grid, window, &save_filter, 1, NULL);
    }
    else
    {
        saveJSON(save_path, grid);
    }
}

void SaveLoad::load()
{
    Grid* grid = &Interface::getInstance().getGrid();
    SDL_Window* window = Interface::getInstance().getWindow();
    SDL_ShowOpenFileDialog(load_dialog_callback, (void*)grid, window, &save_filter, 1, NULL, false);
}

void SaveLoad::loadPath(std::string path)
{
    Grid* grid = &Interface::getInstance().getGrid();
    SaveLoad::loadJSON(path, grid);
    Interface::getInstance().updateTitle(SaveLoad::getPath());
}

void SaveLoad::saveJSON(std::string path, Grid* grid)
{
    std::string file_ext = save_filter.pattern;
    if (path.length() < file_ext.length() || path.substr(path.length() - file_ext.length()) != file_ext)
    {
        path += "." + file_ext;
    }

    std::ofstream file(path, std::ios::out | std::ios::trunc);
    if (!file.is_open())
        return;

    save_path = path;

    json j;
    j["areas"] = json::array();
    j["generators"] = json::array();
    j["sequencers"] = json::array();
    j["bpm"] = Clock::getInstance().getBPM();

    for (auto& block : grid->getBlocks())
    {
        json block_json;

        switch (block->getType())
        {
        case BLOCK_GENERATOR:
            {
                BlockGenerator* generator = (BlockGenerator*)block;
                block_json["waveform"] = generator->getWaveForm();
                block_json["amp"] = generator->getDataAmp();
                block_json["freq"] = generator->getFrequency();
                block_json["rel_freq"] = generator->getRelFreq();
                block_json["freq_factor"] = generator->getFreqFactor();
                block_json["sample_path"] = generator->getSample()->getPath();
                block_json["sample_root"] = generator->getSample()->getRoot();
                block_json["sample_play_type"] = generator->getSample()->getPlayType();
                block_json["pos"] = {block->getPos().x, block->getPos().y};

                j["generators"].push_back(block_json);
                break;
            }
        case BLOCK_SEQUENCER:
            {
                BlockSequencer* sequencer = (BlockSequencer*)block;
                block_json["pitch"] = sequencer->getPitch();
                block_json["pitch_type"] = sequencer->getPitchType();
                block_json["interval"] = sequencer->getInterval().first;
                block_json["octave_subdivision"] = sequencer->getInterval().second;
                block_json["pos"] = {block->getPos().x, block->getPos().y};

                j["sequencers"].push_back(block_json);
                break;
            }
        }
    }

    for (auto& area : grid->getAreas())
    {
        json area_json;
        area_json["positions"] = json::array();
        area_json["bpm_subdivision"] = area->getBPMSubdivision();
        area_json["gliss_percent"] = area->getGlissPercent();
        area_json["attack_percent"] = area->getAttackPercent();
        area_json["release_percent"] = area->getReleasePercent();
        area_json["amp"] = area->getAmp();
        for (auto& pos : area->getPositions())
            area_json["positions"].push_back({pos.x, pos.y});

        j["areas"].push_back(area_json);
    }

    file << j.dump(4);
    file.close();
}

void SaveLoad::loadJSON(std::string path, Grid* grid)
{
    std::ifstream file(path, std::ios::in);
    if (!file.is_open())
        return;

    save_path = path;

    json j;
    file >> j;
    file.close();

    grid->clear();
    Interface::getInstance().stop();
    Clock::getInstance().setBPM(j["bpm"]);

    for (auto& area_json : j["areas"])
    {
        Area* area;
        for (auto& pos_json : area_json["positions"])
        {
            Vector2f pos = {pos_json[0], pos_json[1]};
            area = grid->addArea(pos);
        }
        area->setAmp(area_json["amp"]);
        area->setBPMSubdivision(area_json["bpm_subdivision"]);
        area->setGlissPercent(area_json["gliss_percent"]);
        area->setAttackPercent(area_json["attack_percent"]);
        area->setReleasePercent(area_json["release_percent"]);
    }

    for (auto& sequencer_json : j["sequencers"])
    {
        Vector2f pos = {sequencer_json["pos"][0], sequencer_json["pos"][1]};
        BlockSequencer* sequencer = new BlockSequencer(pos);
        sequencer->setPitch(sequencer_json["pitch"]);
        sequencer->setPitchType(sequencer_json["pitch_type"]);
        sequencer->setInterval(sequencer_json["interval"], sequencer_json["octave_subdivision"]);

        grid->addBlock(sequencer);
    }

    for (auto& generator_json : j["generators"])
    {
        Vector2f pos = {generator_json["pos"][0], generator_json["pos"][1]};
        BlockGenerator* generator = new BlockGenerator(pos);

        generator->setWave(generator_json["waveform"]);
        generator->setAmp(generator_json["amp"]);
        generator->setFrequency(generator_json["freq"]);
        generator->setRelFreq(generator_json["rel_freq"]);
        generator->setFreqFactor(generator_json["freq_factor"]);

        generator->getSample()->openPath(generator_json["sample_path"]);
        generator->getSample()->setRoot(generator_json["sample_root"]);
        generator->getSample()->setPlayType(generator_json["sample_play_type"]);

        grid->addBlock(generator);
    }

    Interface::getInstance().updateBlockTimes();
}

void save_dialog_callback(void* userdata, const char* const * filelist, int filter)
{
    if (filelist == NULL || !filelist[0])
        return;

    Grid* grid = (Grid*)userdata;
    std::string path = filelist[0];
    SaveLoad::saveJSON(path, grid);
    Interface::getInstance().updateTitle(SaveLoad::getPath());
}

void load_dialog_callback(void* userdata, const char* const * filelist, int filter)
{
    if (filelist == NULL || !filelist[0])
        return;

    Grid* grid = (Grid*)userdata;
    std::string path = filelist[0];
    SaveLoad::loadJSON(path, grid);
    Interface::getInstance().updateTitle(SaveLoad::getPath());
}
