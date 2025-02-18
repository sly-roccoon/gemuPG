#pragma once
#include "block.h"
#include "util.h"
#include "clock.h"
#include <SDL3/SDL.h>
#include <vector>

class Area
{
public:
    auto getPositions() { return positions_; }
    void addPositions(std::vector<Vector2f>);
    bool addPosition(Vector2f);
    void removePosition(Vector2f);
    bool isInside(Vector2f);

    Block *addBlock(Block *);
    void removeBlock(Block *);
    bool removeBlock(Vector2f);
    auto getBlocks() { return blocks_; }
    Block *getBlock(Vector2f);

    Block *addSequencer(BlockSequencer *);
    void removeSequencer(BlockSequencer *);
    bool removeSequencer(Vector2f);
    void updateSequence();
    std::vector<BlockSequencer *> getSequence() { return sequence_; }
    BlockSequencer *getSequencer(Vector2f);
    void stepSequence();
    void stopSequence();

    int getBPMSubdivision() { return bpm_subdivision_; }
    void setBPMSubdivision(int bpm_subdivision) { bpm_subdivision_ = bpm_subdivision; }

    float getAmp() { return amp_; }
    void setAmp(float amp) { amp_ = amp; }

    void drawGUI();
    void toggleGUI() { viewGUI_ = !viewGUI_; }
    void setGUI(bool view) { viewGUI_ = view; }

    void updateGlissando(BlockGenerator *block = nullptr);
    void updateAttack(BlockGenerator *block = nullptr);

    float getGlissPercent() { return gliss_percent_; }
    void setGlissPercent(float gliss_percent)
    {
        gliss_percent_ = gliss_percent;
        updateGlissando();
    }

    float getAttackPercent() { return attack_percent_; }
    void setAttackPercent(float attack_percent)
    {
        attack_percent_ = attack_percent;
        updateAttack();
    }

private:
    Vector2f getTopLeft();
    void removeDanglingSequencers();
    void cleanupSequence();

    void setNotes(pitch_t);

    std::vector<Vector2f> positions_;
    std::vector<Block *> blocks_;
    std::vector<BlockSequencer *> sequence_;

    int cur_note_idx_ = 0;
    int last_note_idx_ = 0;
    pitch_t last_freq_ = 0.0f;

    float amp_ = 1.0f;

    float gliss_percent_ = 0.0f;
    float attack_percent_ = 25.0f;

    bool viewGUI_ = false;
    int bpm_subdivision_ = 4;
};