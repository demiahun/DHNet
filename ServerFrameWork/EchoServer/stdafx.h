// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

#include <atlbase.h>
#include <atlstr.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <iostream>

#include "DHNetCommon.h"

using namespace DHNet;

#include "packet/Vars.h"
#include "Packet/Test_common.h"
#include "Packet/Test_proxy.h"
#include "Packet/Test_stub.h"