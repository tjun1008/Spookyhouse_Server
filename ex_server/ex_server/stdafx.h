// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include <iostream>
#include <map>
#include <thread>
#include <vector>

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mutex>
#include <unordered_set> //�º��� �� ������ ����
#include <array>
#include <fstream>
#include "protocol.h"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")