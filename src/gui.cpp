#include "gui.h"
#include "command.h"
#include "audio.h"

ImGuiContext *GUI::context_ = nullptr;
SDL_Window *GUI::window_ = nullptr;
SDL_Renderer *GUI::renderer_ = nullptr;
bool GUI::show_output_ = true;

bool GUI::init(SDL_Window *window, SDL_Renderer *renderer)
{
	IMGUI_CHECKVERSION();
	context_ = ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = "";
	io.LogFilename = "";
	ImGui::StyleColorsClassic();
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.9f);
	ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;

	Interface::updateScaling();

	window_ = window;
	renderer_ = renderer;

	if (!ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_))
		return false;

	if (!ImGui_ImplSDLRenderer3_Init(renderer_))
		return false;

	return true;
}

void GUI::destroy()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

void GUI::drawOutput(float *output)
{
	if (!show_output_)
		return;

	ImGui::SetNextWindowPos({ImGui::GetMainViewport()->Size.x - (ICON_SIZE * RENDER_SCALE * 4), ImGui::GetMainViewport()->Size.y - (ICON_SIZE * RENDER_SCALE * 2)});
	ImGui::SetNextWindowSize({ICON_SIZE * RENDER_SCALE * 4, ICON_SIZE * RENDER_SCALE * 2});
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
	ImGui::Begin("Output", &show_output_, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	ImGui::PlotLines("##", output, BUFFER_SIZE, 0, nullptr, -1.0f, 1.0f, {ICON_SIZE * RENDER_SCALE * 4, ICON_SIZE * RENDER_SCALE * 2});

	ImGui::End();
	ImGui::PopStyleVar();
}

void GUI::drawToolbar()
{
	ImGui::SetNextWindowPos({0, ImGui::GetMainViewport()->GetCenter().y - (ICON_SIZE * RENDER_SCALE / 2)});
	ImGui::SetNextWindowSize({ICON_SIZE * RENDER_SCALE + ICON_SIZE * RENDER_SCALE / 4, ICON_SIZE * RENDER_SCALE / 2 + ICON_SIZE * RENDER_SCALE * 3}); // TODO: find better way to adjust margins of button border
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(ICON_SIZE * RENDER_SCALE / 8, ICON_SIZE * RENDER_SCALE / 8));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ICON_SIZE * RENDER_SCALE / 8, ICON_SIZE * RENDER_SCALE / 8));
	ImGui::Begin("Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	if (ImGui::ColorButton("Area", toImVec4(AREA_COLOR), 0, {ICON_SIZE * RENDER_SCALE, ICON_SIZE * RENDER_SCALE}))
		Interface::getInstance().setSelection(AREA);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("area");

	if (ImGui::ColorButton("Generator Block", toImVec4(GENERATOR_COLOR), 0, {ICON_SIZE * RENDER_SCALE, ICON_SIZE * RENDER_SCALE}))
		Interface::getInstance().setSelection(BLOCK_GENERATOR);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("generator");

	if (ImGui::ColorButton("Sequencer", toImVec4(SEQUENCER_COLOR), 0, {ICON_SIZE * RENDER_SCALE, ICON_SIZE * RENDER_SCALE}))
		Interface::getInstance().setSelection(BLOCK_SEQUENCER);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("sequencer");

	ImGui::End();
	ImGui::PopStyleVar(2);

	//------------------------------
	ImGui::SetNextWindowPos({ImGui::GetMainViewport()->GetCenter().x - ICON_SIZE * RENDER_SCALE * 2, ImGui::GetMainViewport()->Size.y - ICON_SIZE * RENDER_SCALE});
	ImGui::SetNextWindowSize({ICON_SIZE * RENDER_SCALE * 2, ICON_SIZE * RENDER_SCALE});
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGui::Begin("Bottom Left", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	if (Clock::getInstance().isRunning())
	{
		if (ImGui::Button("pause", {ICON_SIZE * RENDER_SCALE, ICON_SIZE * RENDER_SCALE}))
			Interface::getInstance().togglePlayPause();
	}
	else
	{
		if (ImGui::Button("play", {ICON_SIZE * RENDER_SCALE, ICON_SIZE * RENDER_SCALE}))
			Interface::getInstance().togglePlayPause();
	}

	ImGui::SameLine();
	if (ImGui::Button("stop", {ICON_SIZE * RENDER_SCALE, ICON_SIZE * RENDER_SCALE}))
		Interface::getInstance().stop();

	ImGui::End();
	ImGui::PopStyleVar(2);

	//-----------------------------

	ImGui::SetNextWindowPos({ImGui::GetMainViewport()->GetCenter().x, ImGui::GetMainViewport()->Size.y - ICON_SIZE * RENDER_SCALE});
	ImGui::SetNextWindowSize({ICON_SIZE * RENDER_SCALE * 4, ICON_SIZE * RENDER_SCALE});
	ImGui::Begin("Bottom Right", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	float bpm = Clock::getInstance().getBPM();
	float vol = AudioEngine::getInstance().getAmp();

	if (ImGui::InputFloat("BPM", &bpm, 1.0f, 5.0f, "%.1f"))
	{
		if (bpm < 0.0f)
			bpm = 1.0f;
		Clock::getInstance().setBPM(bpm);
		Interface::getInstance().updateBlockTimes();
	}

	if (ImGui::DragFloat("volume", &vol, 0.001f, 0.0f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic))
		AudioEngine::getInstance().setVolume(vol);

	ImGui::End();
}