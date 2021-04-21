using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.CSharp;

namespace SerializeGenerator
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

            Console.WriteLine( "args info : " );
            for( int i = 0; i < 3; ++i )
            {
                Console.WriteLine( args[i] );
            }

            byte[] buffer = null;
            using( var fs = File.Open( args[0], FileMode.Open ) )
            {
                buffer = new byte[fs.Length];
                fs.Read( buffer, 0, (int)fs.Length );

                fs.Close();
            }

            if( buffer == null )
            {
                Console.WriteLine( "Curr dir : " + Directory.GetCurrentDirectory() );
                Console.WriteLine( "Fail file open : " + args[0] );

                WaitShutdown();
                return;
            }

            ClassParser parser = new ClassParser();
            if( parser.parse( buffer ) == false )
            {
                WaitShutdown();
                return;
            }

            var generatorCpp = new GeneratorCpp();
            generatorCpp.Generator( parser.vecClassInfo );
            var generatorCS = new GeneratorCS();
            generatorCS.Generator( parser.vecClassInfo );

            File.WriteAllBytes( args[1], Encoding.UTF8.GetBytes( generatorCpp.buffVars ) );
            File.WriteAllBytes( args[2], Encoding.UTF8.GetBytes( generatorCS.buffVars ) );

            Console.WriteLine( "SerialzeGenerator finish.!" );
        }
    }
}
