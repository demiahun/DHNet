#pragma once




namespace DHNet
{
	class CIOObject
	{
	public:
		CIOObject(){}
		virtual ~CIOObject(){}

		/*
		IOCP 이벤트 발생시 IO,Work 등의 스레드 에서 호출되는 함수
		*/
		virtual void OnIOCallback( bool success, DWORD transferred, LPOVERLAPPED lpoverlapped ){}
		/*
		IOObject 가 속한 IOCP 에 메세지를 넘겨주고 IOOjbect 가 속한 IOCP 의 메세지 처리를 하는 스레드를 통해 pakcet 등 전달
		*/
		void	PostIOCompletion( DWORD transferred, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpoverlapped );
		
		void	SetIOCP( HANDLE iocp );
		HANDLE	GetIOCP();

	private:
		/*
		내가 속한 IOCP 의 핸들
		*/
		HANDLE	m_iocp = nullptr;
	};
}
