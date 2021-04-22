#pragma once




namespace DHNet
{
	class CIOObject
	{
	public:
		CIOObject(){}
		virtual ~CIOObject(){}

		/*
		IOCP �̺�Ʈ �߻��� IO,Work ���� ������ ���� ȣ��Ǵ� �Լ�
		*/
		virtual void OnIOCallback( bool success, DWORD transferred, LPOVERLAPPED lpoverlapped ){}
		/*
		IOObject �� ���� IOCP �� �޼����� �Ѱ��ְ� IOOjbect �� ���� IOCP �� �޼��� ó���� �ϴ� �����带 ���� pakcet �� ����
		*/
		void	PostIOCompletion( DWORD transferred, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpoverlapped );
		
		void	SetIOCP( HANDLE iocp );
		HANDLE	GetIOCP();

	private:
		/*
		���� ���� IOCP �� �ڵ�
		*/
		HANDLE	m_iocp = nullptr;
	};
}
