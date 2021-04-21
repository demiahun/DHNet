using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SerializeGenerator
{
    class GeneratorCS
    {
        public string buffVars = "";

        public bool Generator( List<ClassInfo> vecInfo )
        {
            buffVars += "using System; \n";
            buffVars += "using System.Collections.Generic; \n";
            buffVars += "using System.Linq; \n";
            buffVars += "using System.Text; \n";

            buffVars += "\n";
            buffVars += "\t/*\n";
            buffVars += "\t* 사용자 정의 데이터형 Read, Write 만드는 곳\n";
            buffVars += "\t* 사용자 정의 List, Dictionary 추가 시에는 Messagemarshal 의 샘플을 보고 추가하자\n";
            buffVars += "\t*/\n";
            buffVars += "\n";
            buffVars += "using NGNet;\n";
            
            buffVars += "public class UMessageMarshal : NGNet.MessageMarshal\n";
            buffVars += "{\n";

            foreach( var info in vecInfo )
            {
                // 주석 출력
                buffVars += "\t/* \n";
                buffVars += string.Format( "\t{0} packet\n", info.name );
                buffVars += "\t*/ \n";

                // Read 출력
                buffVars += string.Format( "\tpublic static void Read( Message msg, out {0} v )\n", info.name );
                buffVars += "\t{\n";
                buffVars += string.Format( "\t\tv = new {0}();\n", info.name );
                foreach( var arg in info.vecArgs )
                {
                    buffVars += string.Format( "\t\tRead( msg, out v.{0} );\n", arg );
                }
                buffVars += "\t}\n";

                // Write 출력
                buffVars += string.Format( "\tpublic static void Write( Message msg, {0} v )\n", info.name );
                buffVars += "\t{\n";
                foreach( var arg in info.vecArgs )
                {
                    buffVars += string.Format( "\t\tWrite( msg, v.{0} );\n", arg );
                }
                buffVars += "\t}\n";

                // List Read 출력
                buffVars += string.Format( "\tpublic static void Read( Message msg, out List<{0}> values )\n", info.name );
                buffVars += "\t{\n";
                buffVars += string.Format( "\t\tvalues = new List<{0}>();\n", info.name );
                buffVars += "\t\tInt16 count = 0;\n";
                buffVars += "\t\tmsg.Read( out count );\n";
                buffVars += "\t\tfor( int i = 0; i < count; ++i )\n";
                buffVars += "\t\t{\n";
                buffVars += string.Format( "\t\t\t{0} value2;\n", info.name );
                buffVars += "\t\t\tRead( msg, out value2 );\n";
                buffVars += "\t\t\tvalues.Add( value2 );\n";
                buffVars += "\t\t}\n";
                buffVars += "\t}\n";

                // List Write 출력
                buffVars += string.Format( "\tpublic static void Write( Message msg, List<{0}> values )\n", info.name );
                buffVars += "\t{\n";

                buffVars += "\t\tif( values == null )\n";
                buffVars += "\t\t{\n";
                buffVars += "\t\t\tmsg.Write( (Int16)0 );\n";
                buffVars += "\t\t\treturn;\n";
                buffVars += "\t\t}\n";
                buffVars += "\t\tmsg.Write( (Int16)values.Count );\n";
                buffVars += "\t\tfor( int i = 0; i < values.Count; ++i )\n";
                buffVars += "\t\t{\n";
                buffVars += "\t\t\tWrite( msg, values[i] );\n";
                buffVars += "\t\t}\n";

                buffVars += "\t}\n";
            }

            buffVars += "}\n";
            return true;
        }
    }
}
