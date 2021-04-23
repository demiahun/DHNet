using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

/*
 * 패킷을 전송할 데이터 버퍼(Serialize)
 * 지원해야하는 기본 데이터 형이 있으면 직접추가 하기 위해 템플릿 선언을 하지 않음
 * 구조체, 클래스 등의 데이터 는 MessageOperator 인터페이스를 통해 read, write 하여 사용하도록 함
 * 
 */

namespace DHNet
{
    public class Message
    {
        public byte[] buffer = null;
        public Int32 Length
        {
            get
            {
                Int32 v = BitConverter.ToInt32( buffer, BasicType.HEAD_POS_PACKET_SIZE );
                return v;
            }
            set
            {
                byte[] v = BitConverter.GetBytes( value );
                v.CopyTo( buffer, BasicType.HEAD_POS_PACKET_SIZE );
            }
        }
        public void WriteEnd()
        {
            // 패킷 크기 기록
            Length = position;
        }

        public Int32 ID
        {
            get
            {
                Int32 v = BitConverter.ToInt32( buffer, BasicType.HEAD_POS_PACKET_ID );
                return v;
            }
            set
            {
                byte[] v = BitConverter.GetBytes( value );
                v.CopyTo( buffer, BasicType.HEAD_POS_PACKET_ID );
            }
        }

        public Int32 SequenceNum
        {
            get
            {
                Int32 v = BitConverter.ToInt32( buffer, BasicType.HEAD_POS_SEQUENCE_NUM );
                return v;
            }
            set
            {
                byte[] v = BitConverter.GetBytes( value );
                v.CopyTo( buffer, BasicType.HEAD_POS_SEQUENCE_NUM );
            }
        }

        public Byte RmiContextValue
        {
            get
            {
                Byte v = buffer[BasicType.HEAD_POS_RMICONTEXT];
                return v;
            }
            set
            {
                buffer[BasicType.HEAD_POS_RMICONTEXT] = value;
            }
        }

        public Int32 position = BasicType.HEADSIZE; // Head 부분에는 데이터 기록을 하지 않기 위해 시작값을 채워둠

        public Message()
        {
            buffer = new byte[BasicType.MAX_PACKET_SIZE];
            Array.Clear( buffer, 0, BasicType.MAX_PACKET_SIZE );
        }

        /*
         Message data to string
         string to Message
         예제 및 함수
            Message msg01 = new Message();
            Message msg02 = new Message();

            List<Int32> vecValue01 = new List<Int32>();
            List<Int32> vecValue02 = new List<Int32>();

            vecValue01.Add( 1 );
            vecValue01.Add( 2 );
            vecValue01.Add( 3 );
            vecValue01.Add( 4 );
            vecValue01.Add( 5 );

            UMessageMarshal.Write( msg01, vecValue01 );

            string strValue = "";
            msg01.ConvertTo( ref strValue );

            msg02.ConvertFrom( ref strValue );

            UMessageMarshal.Read( msg02, out vecValue02 );
         */
        public void ConvertTo( ref string v )
        {
            if( BasicType.HEADSIZE >= position )
                return;

            byte[] temp = new byte[position - BasicType.HEADSIZE];
            Array.Copy( buffer, BasicType.HEADSIZE, temp, 0, temp.Length );

            v = Encoding.Default.GetString( temp );
        }
        public void ConvertFrom( ref string v )
        {
            if( v.Length == 0 )
                return;

            byte[] temp = Encoding.Default.GetBytes( v );

            Array.Copy( temp, 0, buffer, BasicType.HEADSIZE, temp.Length );
        }

        /*
         * buffer read
         * 직접 호출해서 사용하지 말고 꼭 MessageMarshal 을 통해서만 read 해야함
         */
        public void Read( out Byte v )
        {
            v = buffer[position];
            position += sizeof( Byte );
        }
        public void Read( out Int16 v )
        {
            v = BitConverter.ToInt16( buffer, position );
            position += sizeof( Int16 );
        }
        public void Read( out UInt16 v )
        {
            v = BitConverter.ToUInt16( buffer, position );
            position += sizeof( UInt16 );
        }
        public void Read( out Int32 v )
        {
            v = BitConverter.ToInt32( buffer, position );
            position += sizeof( Int32 );
        }
        public void Read( out UInt32 v )
        {
            v = BitConverter.ToUInt32( buffer, position );
            position += sizeof( UInt32 );
        }
        public void Read( out Int64 v )
        {
            v = BitConverter.ToInt64( buffer, position );
            position += sizeof( Int64 );
        }
        public void Read( out UInt64 v )
        {
            v = BitConverter.ToUInt64( buffer, position );
            position += sizeof( UInt64 );
        }
        public void Read( out float v )
        {
            v = BitConverter.ToSingle( buffer, position );
            position += sizeof( float );
        }
        public void Read( out double v )
        {
            v = BitConverter.ToDouble( buffer, position );
            position += sizeof( double );
        }
        public void Read( out string v )
        {
            // 문자열 길이는 최대 2바이트 까지. 0 ~ 32767
            int len = 0;
            Read( out len );

            // 인코딩은 UTF8 통일한다.
            v = Encoding.UTF8.GetString( buffer, position, len );
            position += len;
        }

        /*
         * Buffer write
         * 직접 호출해서 사용하지 말고 꼭 MessageMarshal 을 통해서만 write 해야함
         */
        public void Write( Byte v )
        {
            buffer[position] = v;
            position += sizeof( Byte );
        }
        public void Write( Int16 v )
        {
            byte[] value = BitConverter.GetBytes( v );
            value.CopyTo( buffer, position );
            position += value.Length;
        }
        public void Write( UInt16 v )
        {
            byte[] value = BitConverter.GetBytes( v );
            value.CopyTo( buffer, position );
            position += value.Length;
        }
        public void Write( Int32 v )
        {
            byte[] value = BitConverter.GetBytes( v );
            value.CopyTo( buffer, position );
            position += value.Length;
        }
        public void Write( UInt32 v )
        {
            byte[] value = BitConverter.GetBytes( v );
            value.CopyTo( buffer, position );
            position += value.Length;
        }
        public void Write( Int64 v )
        {
            byte[] value = BitConverter.GetBytes( v );
            value.CopyTo( buffer, position );
            position += value.Length;
        }
        public void Write( UInt64 v )
        {
            byte[] value = BitConverter.GetBytes( v );
            value.CopyTo( buffer, position );
            position += value.Length;
        }
        public void Write( float v )
        {
            byte[] value = BitConverter.GetBytes( v );
            value.CopyTo( buffer, position );
            position += value.Length;
        }
        public void Write( double v )
        {
            byte[] value = BitConverter.GetBytes( v );
            value.CopyTo( buffer, position );
            position += value.Length;
        }
        public void Write( string v )
        {
            byte[] value = Encoding.UTF8.GetBytes( v );

            Write( (int)value.Length );

            value.CopyTo( buffer, position );
            position += value.Length;
        }
    }
}
