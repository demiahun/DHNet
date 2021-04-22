using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NIDL
{
    class GeneratorCS
    {
        public string bufferCommonCS = "";
        public string bufferStubCS = "";
        public string bufferProxyCS = "";

        public bool Generator( List<CPacketGroup> listGroup, string filename )
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // filename_common.cs 제작
            bufferCommonCS += "using System;\n";
            bufferCommonCS += "using System.Collections.Generic;\n\n\n";
            bufferCommonCS += "using DHNet;\n";

            for( int i = 0; i < listGroup.Count; ++i )
            {
                var group = listGroup[i];
                bufferCommonCS += string.Format( "namespace {0}\n", group.name );
                bufferCommonCS += "{\n";    // namespace {
                bufferCommonCS += "\tpublic class Common\n";
                bufferCommonCS += "\t{\n";    // class {

                bufferCommonCS += "\t\t// Message ID that replies to each RMI method.\n";
                foreach( var packet in group.listTokenPacket )
                {
                    bufferCommonCS += string.Format( "\t\tpublic const Int32 Rmi_{0} = {1};\n", packet.name, packet.ID );
                }

                bufferCommonCS += "\t}\n";    // class }
                bufferCommonCS += "}\n\n";  // namespace }
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // filename_stub.cs 제작
            bufferStubCS += "using System;\n";
            bufferStubCS += "using System.Net;\n";
            bufferStubCS += "using System.Collections.Generic;\n\n\n";
            bufferStubCS += "using DHNet;\n";

            for( int i = 0; i < listGroup.Count; ++i )
            {
                var group = listGroup[i];
                bufferStubCS += string.Format( "namespace {0}\n", group.name );
                bufferStubCS += "{\n";    // namespace {
                bufferStubCS += "\tpublic class Stub : DHNet.RmiStub\n";
                bufferStubCS += "\t{\n";    // class {
                
                /*
                 * Delegate 정의
                 * Ex.)
                 * public delegate bool LogInReqDelegate( string deviceID, int iValue, List<int> vecValue );
                 * public LogInReqDelegate LogInReq = delete ( string deviceID, int iValue, List<int> vecValue )
                 * {
                 *      return false;
                 * }
                 */
                foreach( var packet in group.listTokenPacket )
                {
                    string funcDelegate = string.Format( "{0}Delegate", packet.name );
                    string funcArgs = ConvertArgsToString( packet.vecDataType, packet.vecVariable );

                    bufferStubCS += string.Format( "\t\tpublic delegate bool {0}{1};\n", funcDelegate, funcArgs );
                    bufferStubCS += string.Format( "\t\tpublic {0} {1} = delegate {2}\n", funcDelegate, packet.name, funcArgs );
                    bufferStubCS += "\t\t{\n";  // function {
                    bufferStubCS += "\t\t\treturn false;\n";
                    bufferStubCS += "\t\t};\n";  // function }
                }

                /*
                 * RmiStub.ProcessReceivedMessage() 를 override 한다.
                 * Ex.)
                 * public override bool ProcessReceivedMessage( Message msg )
                 * {
                 *      switch( msg.ID )
                 *      {
                 *      case Common.Rmi_LogInReq:
                 *          ProcessReceivedMessage_LogInReq( msg );
                 *          break;
                 *      default:
                 *          return false;
                 *      }
                 *      return true;
                 * }
                 */
                bufferStubCS += "\t\tpublic override bool ProcessReceivedMessage( Message msg )\n";
                bufferStubCS += "\t\t{\n";  // ProcessReceivedMessage {

                bufferStubCS += "\t\t\tswitch( msg.ID )\n";
                bufferStubCS += "\t\t\t{\n";    // switch {
                foreach( var packet in group.listTokenPacket )
                {
                    bufferStubCS += string.Format( "\t\t\tcase Common.Rmi_{0}:\n", packet.name );
                    bufferStubCS += string.Format( "\t\t\t\tProcessReceivedMessage_{0}( msg );\n", packet.name );
                    bufferStubCS += "\t\t\t\tbreak;\n";
                }
                bufferStubCS += "\t\t\tdefault: return false;\n";
                bufferStubCS += "\t\t\t}\n";    // switch }
                bufferStubCS += "\t\t\treturn true;\n";

                bufferStubCS += "\t\t}\n";  // ProcessReceivedMessage }

                /*
                 * 패킷 씨리얼 함수 만들기
                 * void ProcessReceivedMessage_LogInReq( Message msg )
                 * {
                 *      string deviceID;
                 *      int iValue;
                 *      List<int> vecValue;
                 *      UMessageMarshal.Read( msg, out deviceID );
                 *      UMessageMarshal.Read( msg, out iValue );
                 *      UMessageMarshal.Read( msg, out vecValue );
                 *      
                 *      bool _ret = LogInReq( deviceID, iValue, vecValue );
                 *      if( _ret == false )
                 *          Console.WriteLine( "Error: RMI function that a user did not create has been called." );
                 * }
                 */
                foreach( var packet in group.listTokenPacket )
                {
                    bufferStubCS += string.Format( "\t\tvoid ProcessReceivedMessage_{0}( Message msg )\n", packet.name );
                    bufferStubCS += "\t\t{\n";  // func {
                    
                    for( int arg = 0; arg < packet.vecDataType.Count; ++arg )
                    {
                        string dataType = packet.vecDataType[arg];
                        ConvertDataType2CS( ref dataType );

                        bufferStubCS += string.Format( "\t\t\t{0} {1}; UMessageMarshal.Read( msg, out {2} );\n", dataType, packet.vecVariable[arg], packet.vecVariable[arg] );
                    }

                    bufferStubCS += "\t\t\t// Call this method.\n";
                    string strVariable = ConvertVariableToString( packet.vecVariable );
                    bufferStubCS += string.Format( "\t\t\tbool _ret = {0}{1};\n", packet.name, strVariable );
                    bufferStubCS += "\t\t\tif( _ret == false )\n";
                    bufferStubCS += "\t\t\t\tConsole.WriteLine( \"Error: RMI function that a user did not create has been called.\" );\n";

                    bufferStubCS += "\t\t}\n";  // func }
                }

                bufferStubCS += "\t}\n";    // class }
                bufferStubCS += "}\n\n";  // namespace }
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // filename_stub.cs 제작
            bufferProxyCS += "using System;\n";
            bufferProxyCS += "using System.Net;\n";
            bufferProxyCS += "using System.Collections.Generic;\n\n\n";
            bufferProxyCS += "using DHNet;\n";

            for( int i = 0; i < listGroup.Count; ++i )
            {
                var group = listGroup[i];
                bufferProxyCS += string.Format( "namespace {0}\n", group.name );
                bufferProxyCS += "{\n";    // namespace {
                bufferProxyCS += "\tpublic class Proxy : DHNet.RmiProxy\n";
                bufferProxyCS += "\t{\n";    // class {

                /*
                 * public void LogInReq( RmiContext rmiContext, string deviceID, int iValue, List<int> vecValue )
                 * {
                 *      Message msg = new Message();
                 *      UMesssageMarshal.Write( msg, deviceID );
                 *      UMesssageMarshal.Write( msg, iValue );
                 *      UMesssageMarshal.Write( msg, vecValue );
                 *      
                 *      RmiSend( rmiContext, Common.Rmi_LogInReq, msg );
                 * }
                 */
                foreach( var packet in group.listTokenPacket )
                {
                    string funcArgs = "";
                    funcArgs += "( RmiContext rmiContext";
                    if( packet.vecDataType.Count > 0 )
                        funcArgs += ", ";
                    for( int arg = 0; arg < packet.vecDataType.Count; ++arg )
                    {
                        string dataType = packet.vecDataType[arg];
                        ConvertDataType2CS( ref dataType );
                        funcArgs += string.Format( "{0} {1}", dataType, packet.vecVariable[arg] );

                        if( arg < (packet.vecDataType.Count - 1) )
                            funcArgs += ", ";
                    }
                    funcArgs += " )";

                    bufferProxyCS += string.Format( "\t\tpublic void {0}{1}\n", packet.name, funcArgs );
                    bufferProxyCS += "\t\t{\n"; // func {

                    bufferProxyCS += "\t\t\tMessage msg = new Message();\n";

                    foreach( var vari in packet.vecVariable )
                    {
                        bufferProxyCS += string.Format( "\t\t\tUMessageMarshal.Write( msg, {0} );\n", vari );
                    }

                    bufferProxyCS += string.Format( "\t\t\tRmiSend( rmiContext, Common.Rmi_{0}, msg );\n", packet.name );

                    bufferProxyCS += "\t\t}\n"; // func }
                }

                bufferProxyCS += "\t}\n";    // class }
                bufferProxyCS += "}\n\n";  // namespace }
            }

            return true;
        }

        private void ConvertDataType2CS( ref string dataType )
        {
            if( dataType.Contains( "vector<" ) )
            {
                dataType = dataType.Replace( "vector<", "List<" );
            }
            else if( dataType.Contains( "map<" ) )
            {
                dataType = dataType.Replace( "map<", "Dictionary<" );
            }
            else if( dataType.Contains( "Time" ) )
            {
                dataType = dataType.Replace( "Time", "DateTime" );
            }
        }
        /*
         * ( string deviceID, int iValue, List<int> vecValue ) 스트링을 만들어 주는 함수
         */
        private string ConvertArgsToString( List<string> data, List<string> variable )
        {
            string funcArgs = "";
            funcArgs += "( ";
            for( int arg = 0; arg < data.Count; ++arg )
            {
                string dataType = data[arg];
                ConvertDataType2CS( ref dataType );
                funcArgs += string.Format( "{0} {1}", dataType, variable[arg] );

                if( arg < (data.Count - 1) )
                    funcArgs += ", ";
            }
            funcArgs += " )";

            return funcArgs;
        }
        /*
         * ( deviceID, iValue, vecValue ) 스트링을 만들어 주는 함수
         */
        private string ConvertVariableToString( List<string> variable )
        {
            string funcVariable = "";
            funcVariable += "( ";
            for( int arg = 0; arg < variable.Count; ++arg )
            {
                funcVariable += string.Format( "{0}", variable[arg] );

                if( arg < (variable.Count - 1) )
                    funcVariable += ", ";
            }
            funcVariable += " )";

            return funcVariable;
        }
    }
}
