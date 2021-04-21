using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SerializeGenerator
{
    class ClassInfo
    {
        public string name;
        public List<string> vecArgs = new List<string>();
    }
    class ClassParser
    {
        public List<ClassInfo> vecClassInfo = new List<ClassInfo>();

        public bool parse( byte[] buffer )
        {

            bool isClass = false;
            bool isComment = false;
            int ClassBlockCount = 0;

            string token = "";
            List<string> vecToken = new List<string>();

            for( int i = 0; i < buffer.Length; ++i )
            {
                char value = (char)buffer[i];

                switch( value )
                {
                case '\r':
                case ' ':
                case '\t':
                    {
                        if( token.Length > 0 )
                        {
                            vecToken.Add( token );
                            token = "";
                        }
                    }
                    break;
                case '\n':
                    {
                        if( isComment )
                            isComment = false;

                        if( isClass == false && ClassBlockCount == 0 )
                        {
                            if( vecToken.Count > 1 )
                            {
                                if( vecToken[0] == "class" || vecToken[0] == "struct" )
                                {
                                    isClass = true;

                                    ClassInfo classInfo = new ClassInfo();
                                    classInfo.name = vecToken[1];

                                    if( vecToken.Count >= 5 )   // 1차 상속까지만 인자 커버해주자 ㅠㅠ
                                    {
                                        if( vecToken[3] == "public" )
                                        {
                                            foreach( var info in vecClassInfo )
                                            {
                                                if( info.name == vecToken[4] )
                                                {
                                                    classInfo.vecArgs.AddRange( info.vecArgs );
                                                    break;
                                                }
                                            }
                                        }
                                    }

                                    vecClassInfo.Add( classInfo );
                                }
                            }
                        }

                        vecToken.Clear();
                    }
                    break;
                case '{':
                    {
                        if( isClass )
                            ClassBlockCount += 1;
                    }
                    break;
                case '}':
                    {
                        if( isClass )
                            ClassBlockCount -= 1;
                        if( isClass && ClassBlockCount == 0 )
                            isClass = false;
                    }
                    break;
                case ';':
                    {
                        if( token.Length > 0 )
                        {
                            vecToken.Add( token );
                            token = "";
                        }

                        if( isClass && ClassBlockCount == 1 )
                        {
                            int idx = vecClassInfo.Count - 1;
                            switch( vecToken.Count )
                            {
                            case 1:
                                vecClassInfo[idx].vecArgs.Add( vecToken[0] );
                                break;
                            case 2:
                                vecClassInfo[idx].vecArgs.Add( vecToken[1] );
                                break;
                            case 4:
                                {
                                    if( vecToken[0] == "std::vector<" )
                                    {
                                        vecClassInfo[idx].vecArgs.Add( vecToken[3] );
                                    }
                                    else
                                    {
                                        vecClassInfo[idx].vecArgs.Add( vecToken[1] );
                                    }
                                }
                                break;
                            }
                        }
                    }
                    break;
                default:
                    {
                        if( isComment == false )
                        {
                            token += value;
                        }                        

                        if( token == "//" )
                        {
                            isComment = true;
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
