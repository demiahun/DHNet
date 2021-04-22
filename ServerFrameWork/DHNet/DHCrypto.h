#pragma once

#include "BasicType.h"



namespace DHNet
{
	/*
	빠른 암호화 버전
	XOR 암호화를 베이스로 키를 3개를 이용하여 4바이트 구간마다 다른 키를 적용하자
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

			// 암호화
			int iEncryptData = iBufTemp ^ keyCrypt;

			::memcpy( pBuffer + iCryptedSize, &iEncryptData, sizeof(Int32) );			

			// 남은 버퍼 길이 감소
			iRemainSize -= sizeof( Int32 );

			// 복호화한 길이 증가
			iCryptedSize += sizeof( Int32 );

			// 4바이트씩 번갈아 가면서 암호화키 적용
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