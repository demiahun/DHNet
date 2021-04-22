#include "stdafx.h"
#include "DHIOBuffer.h"



namespace DHNet
{

	void IOBuffer::Clear()
	{
		m_HostID = 0;
		m_Length = 0;
		::ZeroMemory( m_buffer, sizeof(char) * MAX_PACKET_SIZE );
	}

}