#pragma once

#define WIN32_LEAN_AND_MEAN
// Windows 헤더 파일:
#include <windows.h>

// C의 런타임 헤더 파일입니다.
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


// D3D 사용에 필요한 라이브러리들을 링크합니다.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D 사용에 필요한 헤더파일들을 포함합니다.
#include <d3d11.h>
#include <d3dcompiler.h>

