using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NIDL
{
    /*
     * 아래와 같은 패킷 하나의 토근 정보를 가진다.
     * LogInReq( string deviceID, Time tmValue, vector<Int32> vecValue );
     */
    class CTokenPacket
    {
        public List<string> vecDataType = new List<string>();
        public List<string> vecVariable = new List<string>();
        public string name = "";
        public Int32 ID = 0;

        public static CTokenPacket Create()
        {
            return new CTokenPacket();
        }

        public void AddArg( string dt, string var )
        {
            vecDataType.Add( dt );
            vecVariable.Add( var );
        }
    }
    /*
     * 아래와 같은 패킷 그룹 정보를 가진다.
     * packet TestC2S 1000
     * {
     * 	    LogInReq( string deviceID, Time tmValue, vector<Int32> vecValue );
     *      CheckServerInfoReq();
     * }
     */
    class CPacketGroup
    {
        public string name = "";
        public Int32 ID = 0;
        public List<CTokenPacket> listTokenPacket = new List<CTokenPacket>();

        public static CPacketGroup Create()
        {
            return new CPacketGroup();
        }
    }

    class IDLParser
    {
        public List<CPacketGroup> listGroup = new List<CPacketGroup>();

        public CPacketGroup GetCurrentGroup()
        {
            int index = listGroup.Count;
            if( index == 0 )
                return null;

            return listGroup[index - 1];
        }
        public CTokenPacket GetCurrentPacket()
        {
            var packetGroup = GetCurrentGroup();
            if( packetGroup == null )
                return null;

            if( packetGroup.listTokenPacket.Count == 0 )
                return null;

            int index = packetGroup.listTokenPacket.Count;
            return packetGroup.listTokenPacket[index - 1];
        }

        public bool parse( byte[] buffer, string filename )
        {
            int lineCount = 1;
            int argsCount = 0;
            string token = "";
            bool IsGroupParsing = false;
            bool IsPacketParsing = false;
            bool IsContainerParsing = false;
            bool IsComment = false; // 주석 파싱 중인가.(주석 정보를 건너 띄기 위해)
            List<string> listGroupBaseInfo = new List<string>();

            /*
             * token 취합은 default 에서 조합이 이루어 지고
             * 주석처리 중일때는 조합하지 않는다.
             */

            for( int i = 0; i < buffer.Length; ++i )
            {
                char value = (char)buffer[i];
                switch( value )
                {
                case '\r':                
                case ' ':
                case '\t':
                case '\n':
                case ',':
                    {
                        // 하나의 Token 을 분석하자

                        if( IsContainerParsing )
                        {
                            // 컨테이너형 자료구조 분석중이면 공문을 그냥 넘기자
                            if( value == ',' )
                                token += value;
                            break;
                        }

                        if( token.Length > 0 )
                        {
                            if( IsGroupParsing == false && IsPacketParsing == false )
                            {
                                /*
                                 * 패킷 그룹 정보 취함 
                                 * Ex.) packetCS TestC2S 1000
                                 * packetCS : C# 용 패킷
                                 * TestC2S : 패킷 그룹 이름
                                 * 1000 : 패킷 시작 번호
                                 */
                                switch( listGroupBaseInfo.Count )
                                {
                                case 0:
                                    {
                                        if( token == "packet" )
                                            listGroupBaseInfo.Add( token );
                                    }
                                    break;
                                case 1:
                                case 2:
                                    {
                                        listGroupBaseInfo.Add( token );
                                    }
                                    break;
                                default:
                                    break;                          
                                }
                            }

                            if( IsGroupParsing && IsPacketParsing )
                            {
                                var tokenPacket = GetCurrentPacket();
                                if( tokenPacket == null )
                                {
                                    Console.WriteLine( "{0}({1}) : Syntax error args parsing({2}), tokenPacket == null", filename, lineCount, token );
                                    return false;
                                }

                                if( (argsCount % 2) == 0 )
                                {
                                    // DataType
                                    tokenPacket.vecDataType.Add( token );
                                }
                                else
                                {
                                    // Variable name
                                    tokenPacket.vecVariable.Add( token );
                                }

                                ++argsCount;
                            }
                        }

                        if( value == '\n' )
                        {
                            if( IsComment  )
                                IsComment = false;

                            ++lineCount;
                        }
                        token = "";
                    }
                    break;

                case '{':
                    {
                        if( IsComment )
                            break;

                        if( IsGroupParsing || IsPacketParsing )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error(case '{', IsGroupParsing || IsPacketParsing)", filename, lineCount );
                            return false;
                        }

                        if( listGroupBaseInfo.Count != 3 )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error(Ex. packet TestC2S 1000)", filename, lineCount );
                            return false;
                        }

                        var packetGroup = CPacketGroup.Create();
                        packetGroup.name = listGroupBaseInfo[1];
                        try
                        {
                            packetGroup.ID = int.Parse( listGroupBaseInfo[2] );
                        }
                        catch( Exception ex )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error GroupID.?({2})", filename, lineCount, listGroupBaseInfo[2] );
                            Console.WriteLine( ex.ToString() );
                            return false;
                        }

                        listGroup.Add( packetGroup );

                        listGroupBaseInfo.Clear();

                        IsGroupParsing = true;
                    }
                    break;
                case '}':
                    {
                        if( IsComment )
                            break;

                        if( IsGroupParsing == false || IsPacketParsing )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error(case '}', IsGroupParsing == false || IsPacketParsing)", filename, lineCount );
                            return false;
                        }

                        IsGroupParsing = false;
                    }
                    break;
                case '(':
                    {
                        if( IsComment )
                            break;

                        if( IsGroupParsing == false || IsPacketParsing )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error(case '(', IsGroupParsing == false || IsPacketParsing)", filename, lineCount );
                            return false;
                        }

                        // 패킷 시작시 정보를 셋팅하자

                        var packetGroup = GetCurrentGroup();
                        if( packetGroup == null )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error(CTokenGroup == null)", filename, lineCount );
                            return false;
                        }

                        var tokenPacket = CTokenPacket.Create();
                        tokenPacket.name = token;
                        tokenPacket.ID = packetGroup.ID + packetGroup.listTokenPacket.Count;

                        packetGroup.listTokenPacket.Add( tokenPacket );

                        token = "";
                        IsPacketParsing = true;
                        argsCount = 0;
                    }
                    break;
                case ')':
                    {
                        if( IsComment )
                            break;

                        if( IsGroupParsing == false || IsPacketParsing == false )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error(case ')', IsGroupParsing == false || IsPacketParsing == false)", filename, lineCount );
                            return false;
                        }

                        if( IsContainerParsing )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error near >(case ')')", filename, lineCount );
                            return false;
                        }

                        var tokenPacket = GetCurrentPacket();
                        if( tokenPacket == null )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error(case ')', tokenPacket == null)", filename, lineCount );
                            return false;
                        }

                        // 공백없이 괄호가 닫힐 경우
                        if( IsGroupParsing && IsPacketParsing && token.Length > 0 )
                        {
                            if( (argsCount % 2) == 0 )
                            {
                                // DataType
                                tokenPacket.vecDataType.Add( token );
                            }
                            else
                            {
                                // Variable name
                                tokenPacket.vecVariable.Add( token );
                            }

                            token = "";

                            ++argsCount;
                        }

                        if( tokenPacket.vecDataType.Count != tokenPacket.vecVariable.Count )
                        {
                            Console.WriteLine( "{0}({1}) : Syntax error(DataType.Count != Variable.Count)", filename, lineCount );
                            return false;
                        }
                        
                        IsPacketParsing = false;
                    }
                    break;
                case '<':
                    {
                        if( IsComment )
                            break;

                        token += value;
                        IsContainerParsing = true;                        
                    }
                    break;
                case '>':
                    {
                        if( IsComment )
                            break;

                        token += value;
                        IsContainerParsing = false;
                    }
                    break;
                case ';':
                    break;

                default:
                    {
                        if( IsComment == false )
                        {
                            // 주석이 아닐 경우만 토큰을 수집한다.
                            token += value;
                        }

                        if( token == "//" )
                        {
                            IsComment = true;
                            token = "";
                        }
                    }
                    break;
                }
            }
            return true;
        }
    }
}
