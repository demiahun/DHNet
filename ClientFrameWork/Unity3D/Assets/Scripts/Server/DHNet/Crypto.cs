using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DHNet
{
    class Crypto
    {
        /*
	     * 빠른 암호화 버전
	     * XOR 암호화를 베이스로 키를 3개를 이용하여 4바이트 구간마다 다른 키를 적용하자
	     */
        public static List<UInt32> XOR_KEY = new List<UInt32>();

        public static void XOREncrypt( byte[] buffer, int startIndex, int buffLength )
        {
            if( (buffLength - startIndex) < sizeof( Int32 ) )
                return;

            Int32 iKeyIndex = 0;
            Int32 iRemainSize = buffLength - startIndex;
            Int32 iCryptedSize = startIndex;

            while( true )
            {
                if( iRemainSize < sizeof( Int32 ) )
                    break;

                Int32 iBufTemp = BitConverter.ToInt32( buffer, iCryptedSize );

                UInt32 keyCrypt = 0;
                switch( iKeyIndex )
                {
                case 0:
                    keyCrypt = XOR_KEY[0];
                    break;
                case 1:
                    keyCrypt = XOR_KEY[1];
                    break;
                case 2:
                    keyCrypt = XOR_KEY[2];
                    break;
                }

                // 암호화
                Int32 iEncryptData = (Int32)(iBufTemp ^ keyCrypt);

                byte[] v = BitConverter.GetBytes( iEncryptData );
                v.CopyTo( buffer, iCryptedSize );

                // 남은 버퍼 길이 감소
                iRemainSize -= sizeof( Int32 );

                // 복호화한 길이 증가
                iCryptedSize += sizeof( Int32 );

                // 4바이트씩 번갈아 가면서 암호화키 적용
                ++iKeyIndex;

                if( iKeyIndex >= XOR_KEY.Count )
                    iKeyIndex = 0;
            }
        }

        public static void XORDecrypt( byte[] buffer, int startIndex, int bufferLength )
        {
            XOREncrypt( buffer, startIndex, bufferLength );
        }
    }
}
