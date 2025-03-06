# GemuPG

A Generative Music Poly* Grid

### Compile Instructions
CMake >= 3.25
GCC >= 13
SDL >= 3.2.0 (via vcpkg or system package manager)

```
git clone https://git.iem.at/aronpetritz/gemupg --recurse-submodules
cd gemupg
include/SDL_ttf/external/download.sh (on Linux)
include/SDL_ttf/external/Get-GitModules.ps1 (on Windows)
mkdir build
cd build
cmake -S .. -B .
make -j$(nproc)
```

## Controls
Scroll Wheel = Zoom
Middle Click / Ctrl + Left Click = Pan\
1/2/3 select area/generator block/sequencer block\
Left Click = Place Block/Area\
Right Click = Remove Block/Area\
Left Click on Block/Area = Edit Block/Area

----------------------------
**Edit and Remove only works with respective tool**\
On Hover:\
Ctrl + C = Copy Block\
Ctrl + V = Paste Block

----------------------------
Ctrl + S = Save\
Ctrl + Shift + S = Save As\
Ctrl + O = Open\
Ctrl + N = New File (NO SAVE PROMPT)

----------------------------
SPACE = Play/Pause\
Q = Close All Edit Windows\
D = Toggle Output Window