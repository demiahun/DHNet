using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;

namespace NIDL
{
    class Program
    {
        public static void WaitShutdown()
        {
            Console.WriteLine( "Please any enter key.!" );
            Console.ReadKey();
        }

        static void Main( string[] args )
        {
            if( args.Length < 3 )
            {
                Console.WriteLine( "빌드 인자가 부족함 args.Length({0})", args.Length );

                WaitShutdown();
                return;
            }

            if( args[0] != "-cs" && args[0] != "-cpp" )
            {
                Console.WriteLine( "fail language type : {0} -> -cs and -cpp", args[0] );

                WaitShutdown();
                return;
            }

            byte[] buffer = null;
            using( var fs = File.Open( args[1], FileMode.Open ) )
            {
                buffer = new byte[fs.Length];
                fs.Read( buffer, 0, (int)fs.Length );

                fs.Close();
            }

            if( buffer == null )
            {
                Console.WriteLine( "Curr dir : " + Directory.GetCurrentDirectory() );
                Console.WriteLine( "Fail file open : " + args[1] );

                WaitShutdown();
                return;
            }

            string filenameIDL = Path.GetFileNameWithoutExtension( args[1] );

            IDLParser parser = new IDLParser();
            if( parser.parse( buffer, filenameIDL + ".NIDL" ) == false )
            {
                Console.WriteLine( filenameIDL + " : parsing fail.!" );

                WaitShutdown();
                return;
            }

            if( args[0] == "-cpp" )
            {
                var generator = new GeneratorCpp();
                if( generator.Generator( parser.listGroup, filenameIDL ) == false )
                {
                    Console.WriteLine( filenameIDL + " : GeneratorCPP fail.!" );

                    WaitShutdown();
                    return;
                }

                string outFileName = args[2];
                outFileName += filenameIDL;
                outFileName += "_common.h";
                File.WriteAllBytes( outFileName, Encoding.UTF8.GetBytes( generator.bufferCommonH ) );

                outFileName = args[2];
                outFileName += filenameIDL;
                outFileName += "_stub.h";
                File.WriteAllBytes( outFileName, Encoding.UTF8.GetBytes( generator.bufferStubH ) );

                outFileName = args[2];
                outFileName += filenameIDL;
                outFileName += "_stub.cpp";

                File.WriteAllBytes( outFileName, Encoding.UTF8.GetBytes( generator.bufferStubCpp ) );

                outFileName = args[2];
                outFileName += filenameIDL;
                outFileName += "_proxy.h";
                File.WriteAllBytes( outFileName, Encoding.UTF8.GetBytes( generator.bufferProxyH ) );

                outFileName = args[2];
                outFileName += filenameIDL;
                outFileName += "_proxy.cpp";
                File.WriteAllBytes( outFileName, Encoding.UTF8.GetBytes( generator.bufferProxyCpp ) );
            }
            if( args[0] == "-cs" )
            {
                var generator = new GeneratorCS();
                if( generator.Generator( parser.listGroup, filenameIDL ) == false )
                {
                    Console.WriteLine( filenameIDL + " : GeneratorCS fail.!" );

                    WaitShutdown();
                    return;
                }

                string outFileName = args[2];
                outFileName += filenameIDL;
                outFileName += "_common.cs";
                File.WriteAllBytes( outFileName, Encoding.UTF8.GetBytes( generator.bufferCommonCS ) );

                outFileName = args[2];
                outFileName += filenameIDL;
                outFileName += "_stub.cs";
                File.WriteAllBytes( outFileName, Encoding.UTF8.GetBytes( generator.bufferStubCS ) );

                outFileName = args[2];
                outFileName += filenameIDL;
                outFileName += "_proxy.cs";
                File.WriteAllBytes( outFileName, Encoding.UTF8.GetBytes( generator.bufferProxyCS ) );
            }

            Console.WriteLine( "{0} Compile Success.!", filenameIDL );
            //WaitShutdown();
            return;

            //for( int i = 0; i < parser.listGroup.Count; ++i )
            //{
            //    var packetGroup = parser.listGroup[i];

            //    Console.WriteLine( "Group Name : " + packetGroup.name );
            //    Console.WriteLine( "Group ID : " + packetGroup.ID );
            //    for( int pk = 0; pk < packetGroup.listTokenPacket.Count; ++pk )
            //    {
            //        var packet = packetGroup.listTokenPacket[pk];
            //        Console.Write( packet.ID + " : " + packet.name );
            //        for( int arg = 0; arg < packet.listDataType.Count; ++arg )
            //        {
            //            var dt = packet.listDataType[arg];
            //            var va = packet.listVariable[arg];
            //            Console.Write( "/ {0} {1}", dt, va );
            //        }
            //        Console.WriteLine( "" );
            //    }
            //}

            //if( buffer != null && buffer.Length > 0 )
            //{
            //    File.WriteAllBytes( args[1], buffer );
            //}
        }
    }
}
