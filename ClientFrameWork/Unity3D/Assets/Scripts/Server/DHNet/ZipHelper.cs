using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;

namespace DHNet
{
    /*
     * Ionic.Zlib 를 이용한 압축 기능지원
     */
    public class ZipHelper
    {
        public static void CompressToMessage( ref Message msg )
        {
            if( (msg.Length - BasicType.HEADSIZE) <= 0 )
                return;

            using( var ms = new MemoryStream() )
            {
                byte[] buffer = new byte[msg.Length - BasicType.HEADSIZE];

                Array.Copy( msg.buffer, BasicType.HEADSIZE, buffer, 0, buffer.Length );
                
                var compress = ZlibStream.CompressBuffer( buffer, CompressionLevel.BestSpeed );

                int sizeOrg = msg.Length - BasicType.HEADSIZE;

                msg.position = BasicType.HEADSIZE;
                msg.Write( sizeOrg );   // 압축전 크기를 넣어서 받는 쪽에서 검사한다.(msg.Write( sizeOrg ) 처리시 msg.position 이 int 만큼 증가한다)

                compress.CopyTo( msg.buffer, msg.position );
                msg.Length = msg.position + (int)compress.Length;
            }
        }
        public static void UncompressToMessage( ref Message msg )
        {
            if( (msg.Length - BasicType.HEADSIZE) <= 0 )
                return;

            int sizeOrg = 0;
            msg.Read( out sizeOrg );

            byte[] buffer = new byte[msg.Length - msg.position];

            Array.Copy( msg.buffer, msg.position, buffer, 0, buffer.Length );

            var uncompress = ZlibStream.UncompressBuffer( buffer );

            if( sizeOrg != uncompress.Length )
            {
                NGLog.Log( string.Format( "uncompress size fail : sizeRet({0}), sizeOrg({1}), packetID({2})", sizeOrg, uncompress.Length, msg.ID ) );
                return;
            }

            uncompress.CopyTo( msg.buffer, BasicType.HEADSIZE );
            msg.Length = BasicType.HEADSIZE + (int)uncompress.Length;

            msg.position = BasicType.HEADSIZE;
        }

        public static string CompressToString( string src )
        {
            byte[] buffer = Encoding.UTF8.GetBytes( src );

            var compress = ZlibStream.CompressBuffer( buffer, CompressionLevel.BestSpeed );

            if( compress.Length > (BasicType.MAX_PACKET_SIZE) )
            {
                //Array.Resize( ref compress, (int)(BasicType.MAX_PACKET_SIZE) );
                return "";
            }

            return Convert.ToBase64String( compress );
        }

        public static string UncompressToString( string src )
        {
            byte[] buffer = Convert.FromBase64String( src );

            var decompress = ZlibStream.UncompressBuffer( buffer );

            return Encoding.UTF8.GetString( decompress );
        }
    }
}
