# 🎵 gemuPG 🎵

### **a generative music poly\* grid**

[go straight to the releases, do not collect 200$](https://github.com/sly-roccoon/gemupg/releases/)

*an interactive grid-based music sequencer for creating evolving, generative compositions*

<p align="center">
    <img src="https://aronrocco.at/pictures/gemupg/gemupg.gif" alt="GemuPG Screenshot" width="600"/>
</p>

**More details on my website:** [https://aronrocco.at/gemupg.html](https://aronrocco.at/gemupg.html)

---

## what is it?

gemupg is a visual music creation tool that lets you build generative compositions using a flexible grid system. place **generator blocks** to create sounds, **sequencer blocks** to control patterns, and **areas** to group and sequence your musical elements. perfect for experimental music, ambient soundscapes, and algorithmic composition.

---

## building from source

**requirements:**
- CMake >= 3.25
- gcc >= 13 (or compatible c++23 compiler)
- sdl3 build dependencies ([see sdl3 docs](https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md#build-dependencies))

```bash
git clone https://git.iem.at/aronpetritz/gemupg --recurse-submodules
cd gemupg
mkdir build && cd build
cmake -S .. -B .
make -j$(nproc)
```

> **note:** use `USE_SHARED_LIBS=ON` to attempt using system sdl3 & sdl3_ttf instead of building from source

---

## 🎮 controls

### navigation
| action | control |
|--------|---------|
| **zoom** | scroll wheel |
| **pan** | middle click / ctrl + left click |

### tools & placement
| action | control |
|--------|---------|
| **select area tool** | `1` |
| **select generator tool** | `2` |  
| **select sequencer tool** | `3` |
| **place block/area** | left click |
| **remove block/area** | right click |
| **edit block/area** | left click on existing element |

> ⚠️ **important:** edit and remove only work with the respective tool selected

### block operations
| action | control |
|--------|---------|
| **copy block** | `ctrl + c` (while hovering) |
| **paste block** | `ctrl + v` |

### file operations
| action | control |
|--------|---------|
| **save** | `ctrl + s` |
| **save as** | `ctrl + shift + s` |
| **open** | `ctrl + o` |
| **new file** | `ctrl + n` ⚠️ *no save prompt* |

### playback & interface
| action | control |
|--------|---------|
| **play/pause** | `space` |
| **close all edit windows** | `q` |
| **toggle output window** | `d` |

---

## 🎼 workflow overview

1. **create areas** - define regions on the grid where your sequences will play
2. **add generators** - place generator blocks with different waveforms
3. **add sequencers** - place sequencer blocks of four types around the sides of the areaas, control timing and pitch
4. **fine-tune** - adjust parameters of blocks and areas in real-time

---

- **source repository:** [git.iem.at/aronpetritz/gemupg](https://git.iem.at/aronpetritz/gemupg)

