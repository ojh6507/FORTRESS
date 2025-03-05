#include "stdafx.h"
#include "GuiController.h"

GuiController::GuiController(HWND hWnd, Graphics* graphics) {
	IMGUI_CHECKVERSION();
	_context = ImGui::CreateContext();
	_io = &ImGui::GetIO();
	ImGui_ImplDX11_Init(graphics->GetDevice(), graphics->GetDeviceContext());
	ImGui_ImplWin32_Init(hWnd);
}

GuiController::~GuiController() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GuiController::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	int mx, my;
	Input::Instance()->GetMouseLocation(mx, my);
	_io->MousePos.x = static_cast<float>(mx);
	_io->MousePos.y = static_cast<float>(my);
	_io->MouseDown[0] = Input::Instance()->IsMouseButtonDown(0);
	_io->MouseDown[1] = Input::Instance()->IsMouseButtonDown(1);
}

void GuiController::RenderFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
