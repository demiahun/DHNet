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
		����� ���� ��ƾ�� ����Ǵ� �����尡 �� ����� ���� ������ Ǯ���� ����� ���, �� ������ Ǯ�� ������ �����Դϴ�.
		- �⺻���� 0�Դϴ�. 0�� �����ϸ� CPU ������ �����˴ϴ�.
		*/
		UINT	m_ThreadCountWork = 0;
		/*
		���������� I/O ó���� ����ϴ� �����尡 �ֽ��ϴ�. �̰��� ������ �����մϴ�.
		- �⺻���� 0�Դϴ�. 0�� �����ϸ� CPU ������ �����˴ϴ�.
		- IOThread �� ��� CPU ������ŭ �����ص� �ظ��� ��� ó���� �����ϱ� ������ ���� ������ �ʿ����.
		*/
		UINT	m_ThreadCountIO = 0;
		/*
		Timer callback �ֱ� �Դϴ�.
		�̰��� �����ϸ� milisec������ �ѹ��� INetServerEvent.OnTick �� ȣ��˴ϴ�.
		- 0�̸� �ݹ����� �ʽ��ϴ�.
		- �⺻���� 0�Դϴ�.
		*/
		DWORD	m_OnTickCallbackIntervalMs = 0;
	};
}

#pragma pack(pop)