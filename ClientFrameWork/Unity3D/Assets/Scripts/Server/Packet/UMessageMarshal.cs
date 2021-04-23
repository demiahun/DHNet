using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DHNet;

class UMessageMarshal : MessageMarshal
{
    /*
     * 사용자 정의 데이터형 Read, Write 만드는 곳
     * 사용자 정의 List, Dictionary 추가 시에는 Messagemarshal 의 샘플을 보고 추가하자
     */
    public static void Read( Message msg, out EventInfo v )
    {
        v = new EventInfo();
        Read( msg, out v.tmStart );
        Read( msg, out v.tmEnd );
        Read( msg, out v.iValue01 );
        Read( msg, out v.iValue02 );
    }
    public static void Write( Message msg, EventInfo v )
    {
        Write( msg, v.tmStart );
        Write( msg, v.tmEnd );
        Write( msg, v.iValue01 );
        Write( msg, v.iValue02 );
    }
    public static void Read( Message msg, out List<EventInfo> values )
    {
        values = new List<EventInfo>();

        Int16 count = 0;
        msg.Read( out count );
        for( int i = 0; i < count; ++i )
        {
            EventInfo value2;
            Read( msg, out value2 );
            values.Add( value2 );
        }
    }
    public static void Write( Message msg, List<EventInfo> values )
    {
        if( values == null )
        {
            msg.Write( (Int16)0 );
            return;
        }
        msg.Write( (Int16)values.Count );
        for( int i = 0; i < values.Count; ++i )
        {
            Write( msg, values[i] );
        }
    }
}
