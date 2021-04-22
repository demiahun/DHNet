#pragma once

#include "BasicType.h"



namespace DHNet
{
	/*
	���� ��ȣȭ ����
	XOR ��ȣȭ�� ���̽��� Ű�� 3���� �̿��Ͽ� 4����Ʈ �������� �ٸ� Ű�� ��������
	*/
	const UInt32 XOR_KEY0 = 0xc6f8aa02;
	const UInt32 XOR_KEY1 = 0x2647abdc;
	const UInt32 XOR_KEY2 = 0x9800bbef;

	inline void XOREncrypt( char *pBuffer, DWORD buffSize )
	{
		if( pBuffer == nullptr )
			return;

		if( buffSize < sizeof( Int32 ) )
			return;

		int iKeyIndex = 0;
		int iRemainSize = buffSize;
		int iCryptedSize = 0;

		for( ;; )
		{
			if( iRemainSize < sizeof( Int32 ) )
			{
				break;
			}

			Int32 iBufTemp;
			::memcpy( &iBufTemp, pBuffer + iCryptedSize, sizeof(Int32) );

			UInt32 keyCrypt = 0;
			switch( iKeyIndex )
			{
			case 0:
				keyCrypt = XOR_KEY0;
				break;
			case 1:
				keyCrypt = XOR_KEY1;
				break;
			case 2:
				keyCrypt = XOR_KEY2;
				break;
			}

			// ��ȣȭ
			int iEncryptData = iBufTemp ^ keyCrypt;

			::memcpy( pBuffer + iCryptedSize, &iEncryptData, sizeof(Int32) );			

			// ���� ���� ���� ����
			iRemainSize -= sizeof( Int32 );

			// ��ȣȭ�� ���� ����
			iCryptedSize += sizeof( Int32 );

			// 4����Ʈ�� ������ ���鼭 ��ȣȭŰ ����
			++iKeyIndex;

			if( iKeyIndex >= 3 )
				iKeyIndex = 0;
		}
	}

	inline void XORDecrypt( char *pBuffer, DWORD buffSize )
	{
		XOREncrypt( pBuffer, buffSize );
	}
}