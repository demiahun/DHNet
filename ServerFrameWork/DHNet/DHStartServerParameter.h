#pragma once

#include "DHString.h"

#pragma pack(push,8)

namespace DHNet
{
	class CStartServerParameter
	{
	public:
		StringW	m_HostName;
		int		m_Port = 0;
		/*
		사용자 정의 루틴이 실행되는 스레드가 이 모듈의 내장 스레드 풀에서 실행될 경우, 그 스레드 풀의 스레드 갯수입니다.
		- 기본값은 0입니다. 0을 지정하면 CPU 갯수로 지정됩니다.
		*/
		UINT	m_ThreadCountWork = 0;
		/*
		내부적으로 I/O 처리를 담당하는 스레드가 있습니다. 이것의 갯수를 지정합니다.
		- 기본값은 0입니다. 0을 지정하면 CPU 갯수로 지정됩니다.
		- IOThread 의 경우 CPU 개수만큼 동작해도 왠만한 모든 처리가 가능하기 때문에 따로 조작이 필요없음.
		*/
		UINT	m_ThreadCountIO = 0;
		/*
		Timer callback 주기 입니다.
		이것을 세팅하면 milisec단위로 한번씩 INetServerEvent.OnTick 가 호출됩니다.
		- 0이면 콜백하지 않습니다.
		- 기본값은 0입니다.
		*/
		DWORD	m_OnTickCallbackIntervalMs = 0;
	};
}

#pragma pack(pop)