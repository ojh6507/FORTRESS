#pragma once
class Graphics;
class GuiController {
public:
	GuiController(HWND hWnd, Graphics* graphics);
	~GuiController();

	void NewFrame();
	void RenderFrame();
private:
	ImGuiContext* _context;
	ImGuiIO* _io;
};

