#include "util.h"
#include <format>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

class GUI {
public:
  static bool init(SDL_Window *, SDL_Renderer *);
  static void destroy();

  static void drawToolbar();
  static void drawOutput(float *output);
  static void toggleShowOutput() { show_output_ = !show_output_; }

private:
  static ImGuiContext *context_;
  static SDL_Window *window_;
  static SDL_Renderer *renderer_;

  static bool show_output_;
};