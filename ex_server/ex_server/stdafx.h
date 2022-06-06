// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include <iostream>
#include <map>
#include <thread>
#include <vector>

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mutex>
#include <unordered_set> //셋보다 더 성능이 좋음
#include <array>
#include <fstream>
#include "protocol.h"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")