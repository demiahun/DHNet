using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NIDL
{
    class GeneratorCpp
    {
        public string bufferCommonH = "";
        public string bufferStubH = "";
        public string bufferStubCpp = "";
        public string bufferProxyH = "";
        public string bufferProxyCpp = "";

        public bool Generator( List<CPacketGroup> listGroup, string filename )
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // filename_common.h 제작
            bufferCommonH += "#pragma once \n\n";

            for( int i = 0; i < listGroup.Count; ++i )
            {
                var group = listGroup[i];
                bufferCommonH += string.Format( "namespace {0}\n", group.name );
                bufferCommonH += "{\n"; // namespace {

                foreach( var packet in group.listTokenPacket )
                {
                    bufferCommonH += string.Format( "\tconst Int32 Rmi_{0} = {1};\n\n", packet.name, packet.ID );
                }

                if( group.listTokenPacket.Count > 0 )
                {
                    var packetS = group.listTokenPacket[0];
                    var packetE = group.listTokenPacket[group.listTokenPacket.Count - 1];

                    bufferCommonH += string.Format( "\tconst Int32 Rmi_StartID = {0};\n", packetS.ID );
                    bufferCommonH += string.Format( "\tconst Int32 Rmi_EndID = {0};\n", packetE.ID );
                }
                else
                {
                    bufferCommonH += string.Format( "\tconst Int32 Rmi_StartID = {0};\n", group.ID );
                    bufferCommonH += string.Format( "\tconst Int32 Rmi_EndID = {0};\n", group.ID );
                }

                bufferCommonH += "}\n\n";   // namespace }
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // filename_stub.h 제작
            bufferStubH += "#pragma once \n\n";
            bufferStubH += string.Format( "#include \"{0}_common.h\"\n\n", filename );

            for( int i = 0; i < listGroup.Count; ++i )
            {
                var group = listGroup[i];
                bufferStubH += string.Format( "namespace {0}\n", group.name );
                bufferStubH += "{\n";   // namespace {

                bufferStubH += "\tclass Stub : public DHNet::IRmiStub \n";
                bufferStubH += "\t{\n"; // class Stub {
                bufferStubH += "\tpublic:\n";

                // 함수 선언
                foreach( var packet in group.listTokenPacket )
                {
                    // 공용으로 사용되는 함수 선언문 만들기
                    //# LogInReq( HostID remote
                    string strFunc = string.Format( "{0}( HostID remote", packet.name );

                    //# LogInReq( HostID remote,
                    if( packet.vecDataType.Count > 0 )
                    {
                        strFunc += ",";
                    }

                    for( int arg = 0; arg < packet.vecDataType.Count; ++arg )
                    {
                        //# LogInReq( HostID remote, const string &deviceID
                        string dataType = packet.vecDataType[arg];
                        ConvertDataType2Cpp( ref dataType );
                        strFunc += string.Format( " const {0} &{1}", dataType, packet.vecVariable[arg] );

                        //# LogInReq( HostID remote, const string &deviceID,
                        if( arg < (packet.vecDataType.Count - 1) )
                            strFunc += ",";
                    }
                    //# LogInReq( HostID remote, const string &deviceID )
                    strFunc += " )";

                    // 부모함수 완성
                    bufferStubH += "\t\tvirtual bool " + strFunc + "\n";
                    bufferStubH += "\t\t{ return false; }\n";

                    // DECRMI_
                    bufferStubH += string.Format( "#define DECRMI_{0}_{1} bool {2} override\n", group.name, packet.name, strFunc );

                    // DEFRMI_
                    bufferStubH += string.Format( "#define DEFRMI_{0}_{1}(DerivedClass) bool DerivedClass::{2}\n\n", group.name, packet.name, strFunc );
                }

                /*
                 * 패킷 핸들링 함수 선어
                 * virtual bool ProcessReceivedMessage( HostID remote, DHNet::CMessage* msg ) override;
                 */
                bufferStubH += "\t\tvirtual bool ProcessReceivedMessage( HostID remote, DHNet::CMessage* msg ) override;\n";


                bufferStubH += "\t};\n"; // class Stub }

                bufferStubH += "}\n\n"; // namespace }
            }

            // filename_stub.cpp 제작
            bufferStubCpp += "#pragma once\n\n";
            bufferStubCpp += string.Format( "#include \"{0}_stub.h\"\n\n", filename );

            for( int i = 0; i < listGroup.Count; ++i )
            {
                var group = listGroup[i];

                bufferStubCpp += string.Format( "namespace {0}\n", group.name );
                bufferStubCpp += "{\n"; // namespace {

                bufferStubCpp += "\tbool Stub::ProcessReceivedMessage( HostID remote, DHNet::CMessage* msg )\n";
                bufferStubCpp += "\t{\n";   // ProcessReceivedMessage {

                bufferStubCpp += "\t\tswitch( msg->GetID() )\n";
                bufferStubCpp += "\t\t{\n"; // switch {

                foreach( var packet in group.listTokenPacket )
                {
                    bufferStubCpp += string.Format( "\t\tcase Rmi_{0}:\n", packet.name );
                    bufferStubCpp += "\t\t\t{\n";   // case {

                    // 변수 선언 및 시리얼 라이즈
                    string strArgs = "";
                    for( int arg = 0; arg < packet.vecDataType.Count; ++arg )
                    {
                        string dataType = packet.vecDataType[arg];
                        ConvertDataType2Cpp( ref dataType );
                        // Ex.) int iValue; *msg >> iValue; 출력
                        bufferStubCpp += string.Format( "\t\t\t\t{0} {1}; *msg >> {2};\n", dataType, packet.vecVariable[arg], packet.vecVariable[arg] );

                        strArgs += string.Format( ", " + packet.vecVariable[arg] );
                    }

                    // Ex.) bool _ret = LogInReq( remote, iValue );
                    bufferStubCpp += string.Format( "\n\t\t\t\tbool _ret = {0}( remote{1} );\n", packet.name, strArgs );
                    bufferStubCpp += "\t\t\t\tif( _ret == false )\n";
                    bufferStubCpp += string.Format( "\t\t\t\t\t::printf( \"{0} function that a user did not create has been called.#####\" );\n", packet.name );

                    bufferStubCpp += "\t\t\t}\n";   // case }
                    bufferStubCpp += "\t\t\tbreak;\n";   // break;
                }

                bufferStubCpp += "\t\t\tdefault: return false;\n";

                bufferStubCpp += "\t\t}\n"; // switch }

                bufferStubCpp += "\t\treturn true;\n";

                bufferStubCpp += "\t}\n";   // ProcessReceivedMessage }

                bufferStubCpp += "}\n\n";   // namespace }
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // filename_proxy.h 제작
            bufferProxyH += "#pragma once \n\n";
            bufferProxyH += string.Format( "#include \"{0}_common.h\"\n\n", filename );

            for( int i = 0; i < listGroup.Count; ++i )
            {
                var group = listGroup[i];
                bufferProxyH += string.Format( "namespace {0}\n", group.name );
                bufferProxyH += "{\n";   // namespace {

                bufferProxyH += "\tclass Proxy : public DHNet::IRmiProxy \n";
                bufferProxyH += "\t{\n"; // class Proxy {
                bufferProxyH += "\tpublic:\n";

                // 함수 선언
                foreach( var packet in group.listTokenPacket )
                {
                    // 공용으로 사용되는 함수 선언문 만들기
                    //# LogInReq( HostID remote
                    string strFunc = string.Format( "{0}( HostID remote, RmiContext rmiContext", packet.name );

                    //# LogInReq( HostID remote,
                    if( packet.vecDataType.Count > 0 )
                    {
                        strFunc += ",";
                    }

                    for( int arg = 0; arg < packet.vecDataType.Count; ++arg )
                    {
                        //# LogInReq( HostID remote, const string &deviceID
                        string dataType = packet.vecDataType[arg];
                        ConvertDataType2Cpp( ref dataType );
                        strFunc += string.Format( " const {0} &{1}", dataType, packet.vecVariable[arg] );

                        //# LogInReq( HostID remote, const string &deviceID,
                        if( arg < (packet.vecDataType.Count - 1) )
                            strFunc += ",";
                    }
                    //# LogInReq( HostID remote, const string &deviceID )
                    strFunc += " );";

                    // Send 함수 완성
                    bufferProxyH += "\t\tvoid " + strFunc + "\n";
                }

                bufferProxyH += "\t};\n"; // class Proxy }

                bufferProxyH += "}\n\n"; // namespace }
            }

            // filename_proxy.cpp 제작
            bufferProxyCpp += "#pragma once\n\n";
            bufferProxyCpp += string.Format( "#include \"{0}_proxy.h\"\n\n", filename );

            for( int i = 0; i < listGroup.Count; ++i )
            {
                var group = listGroup[i];

                bufferProxyCpp += string.Format( "namespace {0}\n", group.name );
                bufferProxyCpp += "{\n"; // namespace {

                // 함수 정의
                foreach( var packet in group.listTokenPacket )
                {
                    // LogInReq( HostID remote, DHNet::RmiContext rmiContext, int iValue ); 만들기
                    string strFunc = string.Format( "\tvoid Proxy::{0}( HostID remote, DHNet::RmiContext rmiContext", packet.name );
                    if( packet.vecDataType.Count > 0 )
                    {
                        strFunc += ",";
                    }
                    for( int arg = 0; arg < packet.vecDataType.Count; ++arg )
                    {
                        string dataType = packet.vecDataType[arg];
                        ConvertDataType2Cpp( ref dataType );
                        strFunc += string.Format( " const {0} &{1}", dataType, packet.vecVariable[arg] );

                        if( arg < (packet.vecDataType.Count - 1) )
                        {
                            strFunc += ",";
                        }
                    }
                    strFunc += " )\n";

                    bufferProxyCpp += strFunc;
                    bufferProxyCpp += "\t{\n";  // packet func {

                    bufferProxyCpp += "\t\tauto msg = CMessage::Create();\n";
                    bufferProxyCpp += string.Format( "\t\tmsg->SetID( Rmi_{0} );\n", packet.name );
                    for( int arg = 0; arg < packet.vecDataType.Count; ++arg )
                    {
                        bufferProxyCpp += string.Format( "\t\t*msg << {0};\n", packet.vecVariable[arg] );
                    }

                    bufferProxyCpp += string.Format( "\t\tRmiSend( remote, rmiContext, Rmi_{0}, msg );\n", packet.name );

                    bufferProxyCpp += "\t}\n";  // packet func }
                }

                bufferProxyCpp += "}\n\n";   // namespace }
            }

            return true;
        }

        private void ConvertDataType2Cpp( ref string dataType )
        {
            if( dataType.Contains( "vector<" ) )
            {
                dataType = dataType.Replace( "vector<", "std::vector<" );
            }

            if (dataType.Contains("string"))
            {
                dataType = dataType.Replace("string", "String");
            }

            if( dataType.Contains( "map<" ) )
            {
                dataType = dataType.Replace( "map<", "std::map<" );
            }

            if( dataType.Contains( "Time" ) )
            {
                dataType = dataType.Replace( "Time", "CDHTime" );
            }
        }
    }
}
