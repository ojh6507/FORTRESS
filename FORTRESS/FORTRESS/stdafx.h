#pragma once

#define WIN32_LEAN_AND_MEAN
// Windows ��� ����:
#include <windows.h>

// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#include <algorithm>
#include <random>
#include <string>
#include <wrl.h>
#include <shellapi.h>
#include <assert.h>

#include <fstream>
#include <vector>

#define FRAME_BUFFER_WIDTH		1080
#define FRAME_BUFFER_HEIGHT		720


// D3D ��뿡 �ʿ��� ���̺귯������ ��ũ�մϴ�.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D ��뿡 �ʿ��� ������ϵ��� �����մϴ�.
#include <d3d11.h>
#include <d3dcompiler.h>

