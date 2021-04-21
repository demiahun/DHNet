using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SerializeGenerator
{
    class GeneratorCpp
    {
        public string buffVars = "";

        public bool Generator( List<ClassInfo> vecInfo )
        {
            buffVars += "#include \"stdafx.h\" \n";
            buffVars += "#include \"Vars.h\" \n";

            buffVars += "\n\n";
            buffVars += "using namespace std;\n";
            buffVars += "using namespace NGNet;\n\n\n";

            buffVars += "namespace NGNet\n";
            buffVars += "{\n";

            foreach( var info in vecInfo )
            {
                buffVars += "\t/* \n";
                buffVars += string.Format( "\t{0} packet\n", info.name );
                buffVars += "\t*/ \n";

                buffVars += string.Format( "\tNGNet::CMessage& operator >> ( CMessage &a, {0} &v )\n", info.name );
                buffVars += "\t{\n";
                foreach( var arg in info.vecArgs )
                {
                    buffVars += string.Format( "\t\ta >> v.{0};\n", arg );
                }
                buffVars += "\t\treturn a;\n";
                buffVars += "\t}\n";

                buffVars += string.Format( "\tNGNet::CMessage& operator << ( CMessage &a, const {0} &v )\n", info.name );
                buffVars += "\t{\n";
                foreach( var arg in info.vecArgs )
                {
                    buffVars += string.Format( "\t\ta << v.{0};\n", arg );
                }
                buffVars += "\t\treturn a;\n";
                buffVars += "\t}\n";
            }

            buffVars += "}\n";
            return true;
        }
    }
}
