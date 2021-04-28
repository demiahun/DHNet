#pragma once

#include "CoreMinimal.h"
#include "BasicType.h"



namespace DHNet
{
	static UInt32 XOR_KEY0 = 0xc6f8aa02;
	static UInt32 XOR_KEY1 = 0x2647abdc;
	static UInt32 XOR_KEY2 = 0x9800bbef;

	inline void XOREncrypt( char *pBuffer, DWORD buffSize )
	{
		if( pBuffer == nullptr )
			return;

		if( buffSize < sizeof( Int32 ) )
			return;

		Int32 iKeyIndex = 0;
		Int32 iRemainSize = buffSize;
		Int32 iCryptedSize = 0;

		while(true)
		{
			if( iRemainSize < sizeof( Int32 ) )
			{
				break;
			}

			Int32 iBufTemp;
			FMemory::Memcpy( &iBufTemp, pBuffer + iCryptedSize, sizeof( Int32 ) );

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

			FMemory::Memcpy( pBuffer + iCryptedSize, &iEncryptData, sizeof( Int32 ) );

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
