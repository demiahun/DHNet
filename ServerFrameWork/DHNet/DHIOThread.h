#pragma once

#include "DHThread.h"
#include "DHIOObject.h"
#include "DHPtr.h"

/*
CSocket �� Send,Recv ó���� ���� ���ִ� ������
	CSocket �� �����Ǿ� IO �� ���� �Ҵ޵Ǵ� IOCP ����	
*/
namespace DHNet
{
	class CIOThread : public CDHThread, public CIOObject
	{
	public:
		CIOThread();
		virtual ~CIOThread(){}

		virtual bool Loop() override;

	public:
		/*
		I/O ��Ʈ, Thread ����
		this::End() -> IOCP -> this::Loop ���� ����ó��
		*/
		void	End();

		void	SetIndex( UINT index );

	private:
		/*
		IOThread index
			DHNetServer ���� IOThread ������ ���������� �Ҵ�
		*/
		UINT	m_index = 0;
	};

	REFCOUNT( CIOThread );
}


