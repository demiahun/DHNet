using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;

/*
 * 패킷 구성시 사용되는 인터페이스
 * Ex. )
 * Message msg;
 * msg.ID = packetID;
 * MessageMarshal.Write( msg, iValue );
 * MessageMarshal.Write( msg, GuildData );
 * msg.Finish();
 */

namespace DHNet
{
    public class MessageMarshal
    {
        public MessageMarshal()
        {
        }

        public static void Read( Message msg, out bool v )
        {
            Byte value = 0;
            msg.Read( out value );
            v = value > 0 ? true : false;
        }
        public static void Write( Message msg, bool v )
        {
            Byte value = (Byte)(v ? 1 : 0);
            msg.Write( value );
        }
        public static void Read( Message msg, out Int16 b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, Int16 b )
        {
            msg.Write( b );
        }
        public static void Read( Message msg, out List<Int16> values )
        {
            values = new List<Int16>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                Int16 value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<Int16> values )
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
        public static void Read( Message msg, out UInt16 b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, UInt16 b )
        {
            msg.Write( b );
        }
        public static void Read( Message msg, out List<UInt16> values )
        {
            values = new List<UInt16>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                UInt16 value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<UInt16> values )
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
        public static void Read( Message msg, out Int32 b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, Int32 b )
        {
            msg.Write( b );
        }
        public static void Read( Message msg, out List<Int32> values )
        {
            values = new List<Int32>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                Int32 value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<Int32> values )
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
        public static void Read( Message msg, out UInt32 b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, UInt32 b )
        {
            msg.Write( b );
        }
        public static void Read( Message msg, out List<UInt32> values )
        {
            values = new List<UInt32>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                UInt32 value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<UInt32> values )
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
        public static void Read( Message msg, out Int64 b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, Int64 b )
        {
            msg.Write( b );
        }
        public static void Read( Message msg, out List<Int64> values )
        {
            values = new List<Int64>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                Int64 value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<Int64> values )
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
        public static void Read( Message msg, out UInt64 b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, UInt64 b )
        {
            msg.Write( b );
        }
        public static void Read( Message msg, out List<UInt64> values )
        {
            values = new List<UInt64>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                UInt64 value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<UInt64> values )
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
        public static void Read( Message msg, out float b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, float b )
        {
            msg.Write( b );
        }
        public static void Read( Message msg, out List<float> values )
        {
            values = new List<float>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                float value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<float> values )
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
        public static void Read( Message msg, out double b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, double b )
        {
            msg.Write( b );
        }
        public static void Read( Message msg, out List<double> values )
        {
            values = new List<double>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                double value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<double> values )
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
        public static void Read( Message msg, out string b )
        {
            msg.Read( out b );
        }
        public static void Write( Message msg, string b )
        {
            if( b == null )
                b = "";
            msg.Write( b );
        }
        public static void Read( Message msg, out List<string> values )
        {
            values = new List<string>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                string value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<string> values )
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
        
        public static void Read( Message msg, out DateTime b )
        {
            Int16 year = 0;
            Int16 month = 0;
            Int16 day = 0;
            Int16 hour = 0;
            Int16 min = 0;
            Int16 sec = 0;
            Read( msg, out year );
            Read( msg, out month );
            Read( msg, out day );
            Read( msg, out hour );
            Read( msg, out min );
            Read( msg, out sec );

            bool IsFail = false;
            {
                if( year < 1899 ||
                    (month > 12 || month < 1) ||
                    (day > 31 || day < 1) ||
                    (hour > 24 || hour < 0) ||
                    (min > 59 || min < 0) ||
                    (sec > 59 || sec < 0)
                    )
                {
                    IsFail = true;
                }
            }
            if( IsFail )
            {
                b = DateTime.Now;
            }
            else
            {
                b = new DateTime( year, month, day, hour, min, sec );
            }
        }
        public static void Write( Message msg, DateTime b )
        {
            Write( msg, (Int16)b.Year );
            Write( msg, (Int16)b.Month );
            Write( msg, (Int16)b.Day );
            Write( msg, (Int16)b.Hour );
            Write( msg, (Int16)b.Minute );
            Write( msg, (Int16)b.Second );
        }
        public static void Read( Message msg, out List<DateTime> values )
        {
            values = new List<DateTime>();

            Int16 count = 0;
            msg.Read( out count );
            for( int i = 0; i < count; ++i )
            {
                DateTime value;
                Read( msg, out value );
                values.Add( value );
            }
        }
        public static void Write( Message msg, List<DateTime> values )
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

        /*
         * List, Dictionary 추가 샘플
         */

        //public static void Read<K, V>( Message msg, out Dictionary<K, V> dictionary )
        //{
        //    dictionary = new Dictionary<K, V>();

        //    Int16 count = 0;
        //    Read( msg, out count );
        //    for( int i = 0; i < count; ++i )
        //    {
        //        K key;
        //        V value;
        //        Read( msg, out key );
        //        Read( msg, out value );
        //        dictionary.Add( key, value );
        //    }
        //}
        //public static void Write<K, V>( Message msg, Dictionary<K, V> dictionary )
        //{
        //    if( dictionary == null )
        //    {
        //        Write( msg, (Int16)0 );
        //        return;
        //    }
        //    Write( msg, (Int16)dictionary.Count );
        //    foreach( var kv in dictionary )
        //    {
        //        Write( msg, kv.Key );
        //        Write( msg, kv.Value );
        //    }
        //}

        //public static void Read<T>( Message msg, out List<T> values )
        //{
        //    values = new List<T>();

        //    Int16 count = 0;
        //    msg.Read( out count );
        //    for( int i = 0; i < count; ++i )
        //    {
        //        T value;
        //        Read( msg, out value );
        //        values.Add( value );
        //    }
        //}
        //public static void Write<T>( Message msg, List<T> values )
        //{
        //    if( values == null )
        //    {
        //        msg.Write( (Int16)0 );
        //        return;
        //    }
        //    msg.Write( (Int16)values.Count );
        //    for( int i = 0; i < values.Count; ++i )
        //    {
        //        Write( msg, values[i] );
        //    }
        //}
    }
}
