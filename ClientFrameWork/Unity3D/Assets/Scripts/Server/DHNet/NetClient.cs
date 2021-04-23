using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Collections;
using UnityEngine;

namespace DHNet
{
    public class CIPv6Supporter
    {
        enum ADDRESSFAM
        {
            IPv4,
            IPv6,
        }

#if UNITY_IOS && !UNITY_EDITOR
        [DllImport( "__Internal" )]
        private static extern string getIPv6( string host );
#endif
        // Check IP or not
        bool IsIPAddress( string data )
        {
            Match match = Regex.Match( data, @"\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b" );
            return match.Success;
        }

        string GetIPv6( string host )
        {
#if UNITY_IOS && !UNITY_EDITOR
            return getIPv6 (host);
#else
            return host + "&&ipv4";
#endif
        }

        // Get IP type and synthesize IPv6, if needed, for iOS
        void GetIPType( string serverIp, out String newServerIp, out AddressFamily IPType )
        {
            IPType = AddressFamily.InterNetwork;
            newServerIp = serverIp;
            try
            {
                string IPv6 = GetIPv6( serverIp );
                if( !string.IsNullOrEmpty( IPv6 ) )
                {
                    string[] tmp = System.Text.RegularExpressions.Regex.Split( IPv6, "&&" );
                    if( tmp != null && tmp.Length >= 2 )
                    {
                        string type = tmp[1];
                        if( type == "ipv6" )
                        {
                            newServerIp = tmp[0];
                            IPType = AddressFamily.InterNetworkV6;
                        }
                    }
                }
            }
            catch( Exception e )
            {
                Debug.Log( string.Format("GetIPv6 error: {0}" + e.Message) );
            }
        }

        // Get IP address by AddressFamily and domain
        private string GetIPAddress( string hostName, ADDRESSFAM AF )
        {
            if( AF == ADDRESSFAM.IPv6 && !System.Net.Sockets.Socket.OSSupportsIPv6 )
                return null;
            if( string.IsNullOrEmpty( hostName ) )
                return null;
            System.Net.IPHostEntry host;
            string connectIP = "";
            try
            {
                host = System.Net.Dns.GetHostEntry( hostName );
                foreach( System.Net.IPAddress ip in host.AddressList )
                {
                    if( AF == ADDRESSFAM.IPv4 )
                    {
                        if( ip.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork )
                            connectIP = ip.ToString();
                    }
                    else if( AF == ADDRESSFAM.IPv6 )
                    {
                        if( ip.AddressFamily == System.Net.Sockets.AddressFamily.InterNetworkV6 )
                            connectIP = ip.ToString();
                    }

                }
            }
            catch( Exception e )
            {
                Debug.Log( "GetIPAddress error: {0}" + e.Message );
            }
            return connectIP;
        }

        public void ConvertHostInfo( string hostname, out string convertedHost, out AddressFamily convertedFamily )
        {
            convertedFamily = AddressFamily.InterNetwork;
            if( IsIPAddress( hostname ) )
            {
                GetIPType( hostname, out convertedHost, out convertedFamily );
            }
            else
            {
                convertedHost = GetIPAddress( hostname, ADDRESSFAM.IPv6 );
                if( string.IsNullOrEmpty( convertedHost ) )
                    convertedHost = GetIPAddress( hostname, ADDRESSFAM.IPv4 );
                else
                    convertedFamily = AddressFamily.InterNetworkV6;
            }
            //NGLog.Log( "Connecting to {0}, protocol {1}", convertedHost, convertedFamily );

            if( string.IsNullOrEmpty( convertedHost ) )
                convertedHost = hostname;
        }
    }
    /*
     * 서버 접속시 사용할 파라메터
     * 변수
     *      hostname : 서버에 접속할 IP 또는 호스트네임
     *      port : 접속할 포트
     *      OnJoinServerComplete : 접속 성공시 호출 될 함수 포인터   
     *      OnLeaveServer : 서버 접속 종료시 호출 될 함수 포인터
     *      OnReceiveMessage : Packet receive 를 받는 함수(NIDL 을 사용하지 않고 직접 처리 할 때 사용 하면됨)
    */

    public class CNetConnectionParam
    {
        public string hostname = "127.0.0.1";
        public int port = 10000;



        public delegate void OnJoinServerCompleteDelegate( ErrorType errorType );
        public OnJoinServerCompleteDelegate OnJoinServerComplete = null; //delegate ( ErrorType errorType ){};        
        public delegate void OnPacketReceiveDelegate( Message msg );
        public OnPacketReceiveDelegate OnReceiveMessage = null; //delegate ( ErrorType errorType ){};
        public delegate void OnLeaveServerDelegate( ErrorType errorType );
        public OnLeaveServerDelegate OnLeaveServer = null; //delegate ( ErrorType errorType ){};

        public bool IsValid
        {
            get
            {
                if( OnJoinServerComplete == null ||
                    OnLeaveServer == null
                    )
                    return false;
                return true;
            }
        }
    }
    /*
     * 서버접속시 사용될 Socket 객체
     */
    public class CNetClient
    {
        private const Int64 HostID_None = -1;
        private Int64 _hostID = HostID_None;
        private Int64 hostID
        {
            get
            {
                return _hostID;
            }
            set
            {
                _hostID = value;
            }
        }
        private UInt64 _PingMS = 0;

        private Socket socket = null;
        private CNetConnectionParam startParam = null;

        private byte[] bufferRecv = null;   // 패킷을 받는 용도
        private int buffRecvSize = 0;

        private AsyncCallback asyncCallbackConnect = null;
        private AsyncCallback asyncCallbackSend = null;
        private AsyncCallback asyncCallbackRecv = null;

        /*
         * Critical section
         */
        private object csSend = null;
        private object csRecv = null;

        /*
         * Send, Recv Queue
         */
        private Queue<Message> queSend = null;
        private Queue<Message> queRecv = null;

        private List<RmiProxy> listRmiProxy = new List<RmiProxy>();
        private List<RmiStub> listRmiStub = new List<RmiStub>();

        /*
         * StartParameter Func 타입별 ErrorType 결과를 클라이언트 쓰레드에 전달하기 위한 변수
         */
        private int ErrorTypeJoinServerCompleteValue = (int)ErrorType.ErrorType_None;
        private ErrorType ErrorTypeJoinServerComplete
        {
            get
            {
                return (ErrorType)ErrorTypeJoinServerCompleteValue;
            }
            set
            {
                Interlocked.Exchange( ref ErrorTypeJoinServerCompleteValue, (int)value );
            }
        }
        private int ErrorTypeLeaveServerValue = (int)ErrorType.ErrorType_None;
        private ErrorType ErrorTypeLeaveServer
        {
            get
            {
                return (ErrorType)ErrorTypeLeaveServerValue;
            }
            set
            {
                Interlocked.Exchange( ref ErrorTypeLeaveServerValue, (int)value );
            }
        }

        /*
         * 재접속 연결을 위한 정보
         * None : 접속대기 전 상태
         * ConnectBegin : 접속 시작(BeginConnect() 시출)
         * ConnectVerify : Socket 접속 후 서버 인증 대기(Ex. HostID 등)
         * Connected : Sokcet 접속 및 서버 인증이 끝난 상태(로직 통신 가능한 상태)
         * 
         * Reconnecting : 재접속 시작
         * ReconnectBegin : 재접속 BeginConnect() 호출
         * ReconnectFail : 재접속 실패 -> Reconnecting 으로 전환
         * Close : 정상 종료상태
         */
        private enum EConnectState : int
        {
            None = 0,
            ConnectBegin,
            ConnectVerify,
            Connected,

            Reconnecting,
            ReconnectBegin,
            ReconnectVerify,
            ReconnectFail,
            ReconnectClose,
        }
        private int ConnectStateValue = (int)EConnectState.None;
        private EConnectState ConnectState
        {
            get
            {
                return (EConnectState)ConnectStateValue;
            }
            set
            {
                EConnectState csValue = value;
                if( ConnectState == EConnectState.Reconnecting && value == EConnectState.ConnectBegin )
                    csValue = EConnectState.ReconnectBegin;
                if( ConnectState == EConnectState.ReconnectBegin && value == EConnectState.ConnectVerify )
                    csValue = EConnectState.ReconnectVerify;

                Interlocked.Exchange( ref ConnectStateValue, (int)csValue );
            }
        }
        private DateTime tmReconnectStart = DateTime.Now;

        /*
		TCP 패킷의 순서 번호
		: 이를 활용하여 중복패킷 콜백시 체크하여 접속을 끈어 버린다.
		*/
        private Int32 SequenceNumSend = 0;
        private Int32 SequenceNumRecv = 0;
        private Int32 SequenceNumFailCount = 0;

        public CNetClient()
        {
            bufferRecv = new byte[BasicType.MAX_PACKET_SIZE];
            csSend = new object();
            csRecv = new object();
            queSend = new Queue<Message>();
            queRecv = new Queue<Message>();

            asyncCallbackConnect = new AsyncCallback( OnCallbackConnect );
            asyncCallbackSend = new AsyncCallback( OnCallbackSend );
            asyncCallbackRecv = new AsyncCallback( OnCallbackRecv );
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        // function private

        private void BeginConnect()
        {
            try
            {

                if (socket != null)
                {
                    if (socket.Connected)
                    {
                        Debug.Log("BeginConnect() Already Connected.?");
                        //socket.Shutdown(SocketShutdown.Both);
                    }
                    socket.Close();
                }

                string convertedHost = startParam.hostname;
                AddressFamily convertedFamily = AddressFamily.InterNetwork;
#if !UNITY_STANDALONE
                CIPv6Supporter ipv6Supporter = new CIPv6Supporter();
                ipv6Supporter.ConvertHostInfo(startParam.hostname, out convertedHost, out convertedFamily);
#endif

                socket = new Socket(convertedFamily, SocketType.Stream, ProtocolType.Tcp);
                socket.NoDelay = true;
#if UNITY_STANDALONE
                const int IOC_VENDOR = 0x18000000;
                const int IOC_IN = -2147483648; //0x80000000;
                const int SIO_TCP_SET_ACK_FREQUENCY = IOC_IN | IOC_VENDOR | 23;

                byte[] outvalue = BitConverter.GetBytes( 0 );
                socket.IOControl( SIO_TCP_SET_ACK_FREQUENCY, BitConverter.GetBytes( (int)1 ), outvalue );
#endif

                socket.BeginConnect(convertedHost, startParam.port, asyncCallbackConnect, socket);

                ConnectState = EConnectState.ConnectBegin;

                Debug.Log( "BeginConnect().!" );
            }
            catch( Exception ex )
            {
                Debug.Log( "BeginConnect() Exception " + ex.ToString() );

                switch( ConnectState )
                {
                case EConnectState.None:
                    {
                        // 재접속이 아닐 경우만 함수를 통해 클라에 알리자
                        ErrorTypeJoinServerComplete = ErrorType.ErrorType_TCPConnectFailure;
                    }
                    break;
                case EConnectState.Reconnecting:
                    {
                        ConnectState = EConnectState.ReconnectFail;
                    }
                    break;
                }

                try
                {
                    if( socket != null )
                        socket.Close();
                }
                catch( Exception )
                {
                    Debug.Log( "BeginConnect() socket shutdown Exception " + ex.ToString() );
                }

                return;
            }
        }
        /*
         * Socket.BeginConnect() 에 바인딩 되어 접속 완료시 호출
         * P.S : host thread 에서 호출됨
         */
        private void OnCallbackConnect( IAsyncResult _iar )
        {
            try
            {
                Socket socketTemp = (Socket)_iar.AsyncState;

                if( socketTemp.Connected == false )
                {
                    if( ConnectState == EConnectState.None )
                    {
                        ErrorTypeJoinServerComplete = ErrorType.ErrorType_TCPConnectFailure;
                    }

                    Debug.Log( "OnCallbackConnect() : 접속 실패.!" );
                    return;
                }

                socketTemp.EndConnect( _iar );

                // 메세지 리시브 대기
                buffRecvSize = 0;
                socket.BeginReceive( bufferRecv, buffRecvSize, bufferRecv.Length, SocketFlags.None, asyncCallbackRecv, socket );

                ConnectState = EConnectState.ConnectVerify;

                Debug.Log( "OnCallbackConnect().!" );
            }
            catch( Exception ex )
            {
                Debug.Log( "OnCallbackConnect() exception : " + ex.ToString() );

                switch (ConnectState)
                {
                case EConnectState.ConnectBegin:
                {
                    ErrorTypeJoinServerComplete = ErrorType.ErrorType_TCPConnectFailure;
                    ConnectState = EConnectState.None;
                }
                break;
                case EConnectState.ReconnectBegin:
                {
                    ConnectState = EConnectState.ReconnectFail;
                }
                break;
                }

                if (socket != null)
                    socket.Close();
            }
        }

        private void BeginSend()
        {
            try
            {
                if( IsConnected == false )
                {
                    Debug.Log( "BeginSend() : IsConnected() == false" );
                    return;
                }

                if( ConnectState != EConnectState.Connected )
                {
                    // 서버와의 기본 통신까지 끝난 상태여야 패킷을 보낼수 있다.
                    Debug.Log( "BeginSend() : ConnectState != ReconnectState.Connected" );

                    return;
                }

                Message msg = null;

                lock( csSend )
                {
                    if( queSend.Count == 0 )
                    {
                        return;
                    }

                    msg = queSend.Peek();
                }

                switch( msg.RmiContextValue )
                {
                case (byte)RmiContext.ReliableCompress:
                    {
                        ZipHelper.CompressToMessage( ref msg );
                    }
                    break;
                case (byte)RmiContext.FastEncryp:
                    {
                        Crypto.XOREncrypt( msg.buffer, BasicType.HEADSIZE, msg.Length );
                    }
                    break;
                case (byte)RmiContext.FastEncrypCompress:
                    {
                        Crypto.XOREncrypt( msg.buffer, BasicType.HEADSIZE, msg.Length );

                        ZipHelper.CompressToMessage( ref msg );
                    }
                    break;                
                }

                ++SequenceNumSend;
                msg.SequenceNum = SequenceNumSend;

                socket.BeginSend( msg.buffer, 0, msg.Length, SocketFlags.None, asyncCallbackSend, socket );
            }
            catch( Exception ex )
            {
                Debug.Log( "BeginSend() exception : " + ex.ToString() );

                try
                {
                    if( socket != null )
                        socket.Close();
                }
                catch( Exception )
                {
                    Debug.Log( "BeginSend() socket shutdown Exception " + ex.ToString() );
                }

                ConnectState = EConnectState.None;
            }
        }
        /*
         * Socket.BeginSend() 에 바인딩 되어 Send 완료시 호출
         * P.S : host thread 에서 호출됨
         */
        private void OnCallbackSend( IAsyncResult iar )
        {
            try
            {
                Socket socketTemp = (Socket)iar.AsyncState;

                if(socketTemp.Connected == false )
                {
                    Debug.Log( "NetClient.OnCallbackSend() : socketTemp.connected == false" );

                    if (socket != null)
                        socket.Close();

                    return;
                }

                int sendSize = socketTemp.EndSend( iar );
                int queSize = 0;

                lock( csSend )
                {
                    if( queSend.Count > 0 )
                    {
                        var packet = queSend.Dequeue();

                        if( packet.Length != sendSize )
                        {
                            Debug.Log( "OnCallbackSend : packet.position != sendSize...?" );
                        }

                        queSize = queSend.Count;
                    }
                    else
                    {
                        // 재접속 요청시 queSend 에 넣지 않기 때문에 이곳으로 올수 있음.
                        Debug.Log( "OnCallbackSend : queSend.Count == 0" );
                    }
                }

                if( queSize > 0 )
                {
                    BeginSend();
                }
            }
            catch( Exception ex )
            {
                Debug.Log( "OnCallbackSend exception : " + ex.ToString() );

                try
                {
                    if( socket != null )
                        socket.Close();
                }
                catch( Exception )
                {
                    Debug.Log( "OnCallbackSend() socket shutdown Exception " + ex.ToString() );
                }
            }
        }
        /*
         * Socket.BeginReceive() 바인딩 되어 Packet 을 받게 되면 호출
         * P.S : host thread 에서 호출됨
         */
        private void OnCallbackRecv( IAsyncResult iar )
        {
            try
            {
                Socket socketTemp = (Socket)iar.AsyncState;

                if (socketTemp.Connected == false)
                {
                    Debug.Log( "NetClient.OnCallbackRecv() : socketTemp.connected == false" );

                    //if (socket != null)
                    //    socket.Close();
                    return;
                }

                int iReadSize = socketTemp.EndReceive( iar );

                if( iReadSize > 0 )
                {
                    buffRecvSize += iReadSize;
                    /*
		                패킷이 나누어져서 오거나
		                2개 이상의 패킷이 하나로 올수 있어서 체크한다(네이글 알골리즘)
		            */
                    while( buffRecvSize >= sizeof(int) )
                    {
                        Int32 packetLength = 0;
                        packetLength = BitConverter.ToInt32( bufferRecv, 0 );

                        if( buffRecvSize >= packetLength )
                        {
                            Message msgRecv = new Message();
                            Buffer.BlockCopy( bufferRecv, 0, msgRecv.buffer, 0, packetLength );

                            buffRecvSize -= packetLength;
                            if( buffRecvSize >= sizeof( int ) )
                            {
                                // 전달할 부분을 빼고 데이터가 남았다면 앞부분으로 당김
                                Buffer.BlockCopy( bufferRecv, packetLength, bufferRecv, 0, buffRecvSize );
                            }

                            switch( msgRecv.RmiContextValue )
                            {
                            case (byte)RmiContext.ReliableCompress:
                                {
                                    ZipHelper.UncompressToMessage( ref msgRecv );
                                }
                                break;
                            case (byte)RmiContext.FastEncryp:
                                {
                                    Crypto.XOREncrypt( msgRecv.buffer, BasicType.HEADSIZE, packetLength );
                                }
                                break;
                            case (byte)RmiContext.FastEncrypCompress:
                                {
                                    ZipHelper.UncompressToMessage( ref msgRecv );

                                    Crypto.XOREncrypt( msgRecv.buffer, BasicType.HEADSIZE, msgRecv.Length );
                                }
                                break;
                            }

                            // HeartBit 회신
                            if( msgRecv.ID < 0 )
                            {
                                OnReceiveServerMessage( msgRecv );

                                continue;
                            }

                            lock( csRecv )
                            {
                                // 클라 쓰레드로 전달
                                queRecv.Enqueue( msgRecv );
                            }
                        }
                        else
                        {
                            // 온전한 패킷을 구성하기에 충분한 데이터가 없음. loop를 빠져나간다.
                            break;
                        }
                    }
                }
                else if( iReadSize == 0 )
                {
                    Debug.Log( "OnCallbackRecv() ReadSize == 0 : Socket Close" );

                    // LeaveServer 가 FrameMove 에서 호출되면 Socket 종료처리 한다.
                    ErrorTypeLeaveServer = ErrorType.ErrorType_DisconnectFromRemote;

                    ConnectState = EConnectState.None;

                    if( socket != null )
                        socket.Close();

                    return;
                }

                // 메세지 리시브 대기
                if( IsConnected )
                {
                    socket.BeginReceive( bufferRecv, buffRecvSize, bufferRecv.Length - buffRecvSize, SocketFlags.None, asyncCallbackRecv, socket );
                }
            }
            catch( Exception ex )
            {
                Debug.Log( "OnCallbackRecv exception : " + ex.ToString() );

                try
                {
                    if( socket != null )
                        socket.Close();
                }
                catch( Exception exSub )
                {
                    Debug.Log( "OnCallbackRecv socket shutdown exception : " + exSub.ToString() );
                }
            }
        }

        private void OnReceiveServerMessage( Message msg )
        {
            switch( msg.ID )
            {
            case BasicType.PACKETID_HEART_BIT:
                {
                    MessageMarshal.Read( msg, out _PingMS );
                    RmiSend( RmiContext.Reliable, BasicType.PACKETID_HEART_BIT, msg );
                }
                break;
            case BasicType.PACKETID_SC_HOSTID_INFO:
                {
                    if( hostID == HostID_None )
                    {
                        // 신규접속 성공 보냄
                        Int64 tempHost = HostID_None;
                        MessageMarshal.Read( msg, out tempHost );
                        hostID = tempHost;
                        UInt32 xorKey0, xorKey1, xorKey2;
                        MessageMarshal.Read( msg, out xorKey0 );
                        MessageMarshal.Read( msg, out xorKey1 );
                        MessageMarshal.Read( msg, out xorKey2 );
                        Crypto.XOR_KEY.Clear();
                        Crypto.XOR_KEY.Add( xorKey0 );
                        Crypto.XOR_KEY.Add( xorKey1 );
                        Crypto.XOR_KEY.Add( xorKey2 );

                        Debug.Log( string.Format( "OnReceiveServerMessage(HostInfo) Read( hostID({0}) )", hostID ) );

                        Message msgSend = new Message();
                        msgSend.ID = BasicType.PACKETID_CS_HOSTID_RECV;
                        MessageMarshal.Write( msgSend, hostID );

                        ++SequenceNumSend;
                        msgSend.SequenceNum = SequenceNumSend;

                        msgSend.WriteEnd();
                        //RmiSend( RmiContext.Reliable, BasicType.PACKETID_CS_HOSTID_RECV, msgSend );

                        socket.BeginSend( msgSend.buffer, 0, msgSend.Length, SocketFlags.None, asyncCallbackSend, socket );
                    }
                    else
                    {
                        // 재접속 처리 요청
                        Message msgSend = new Message();
                        msgSend.ID = BasicType.PACKETID_CS_HOSTID_RECONNECT;

                        ++SequenceNumSend;
                        msgSend.SequenceNum = SequenceNumSend;

                        MessageMarshal.Write( msgSend, hostID );
                        msgSend.WriteEnd();

                        Int64 remoteID;
                        MessageMarshal.Read( msg, out remoteID );

                        Debug.Log( string.Format( "OnReceiveServerMessage(HostInfoRecon) hostID({0}), Read( hostID({1}) )", hostID, remoteID ) );

                        try
                        {
                            if( socket.Connected )
                            {
                                // 재접속 중이라 쌓여있는 패킷이 있을수 있어 다이렉트로 회신한다.
                                socket.BeginSend( msgSend.buffer, 0, msgSend.Length, SocketFlags.None, asyncCallbackSend, socket );
                            }
                        }
                        catch
                        {
                            try
                            {
                                if( socket != null )
                                socket.Close();
                            }
                            catch( Exception ex )
                            {
                                Debug.Log( "OnReceiveServerMessage() socket shutdown Exception " + ex.ToString() );
                            }
                        }
                    }
                }
                break;
            case BasicType.PACKETID_SC_CONNECT_SUCCESS:
                {
                    Debug.Log( "OnReceiveServerMessage() : PACKETID_SC_CONNECT_SUCCESS" );
                    ConnectState = EConnectState.Connected;
                    ErrorTypeJoinServerComplete = ErrorType.ErrorType_Ok;

                    BeginSend();
                }
                break;
            case BasicType.PACKETID_SC_RECONNECT_SUCCESS:
                {
                    ConnectState = EConnectState.Connected;

                    Debug.Log( string.Format( "OnReceiveServerMessage() " + BasicType.PACKETID_SC_RECONNECT_SUCCESS.ToString() ) );

                }
                break;
            case BasicType.PACKETID_SC_RECONNECT_FAIL:
                {
                    Debug.Log( string.Format( "OnReceiveServerMessage() " + BasicType.PACKETID_SC_RECONNECT_FAIL.ToString() ) );

                    /*
                     * 재접속 실패일 경우 클라에서는 접속중으로 인식하기 때문에 접속 종료를 알리자
                     * LeaveServer 가 FrameMove 에서 호출되면 Socket 종료처리 한다.                    
                     */                     
                    ErrorTypeLeaveServer = ErrorType.ErrorType_ReconnectFail;
                }
                break;
            }
        }


        ///////////////////////////////////////////////////////////////////////////////////////
        // function user interface

        public ErrorType Connect( CNetConnectionParam cp )
        {
            Disconnect();

            startParam = cp;

            if( startParam.IsValid == false )
            {
                return ErrorType.ErrorType_InvalidCallbackFunc;
            }

            try
            {
                if( IsConnected )
                {
                    return ErrorType.ErrorType_AlreadyConnected;
                }

                BeginConnect();
            }
            catch( Exception ex )
            {
                Debug.Log( "Connect exception : " + ex.ToString() );

                return ErrorType.ErrorType_TCPConnectFailure;
            }

            return ErrorType.ErrorType_Ok;
        }

        public void Disconnect()
        {
            ConnectState = EConnectState.None;
            hostID = HostID_None;

            lock (csSend)
            {
                queSend.Clear();
            }

            try
            {
                if( socket == null )
                {
                    Debug.Log( "NetClient.Disconnect() : socket == null" );
                    return;
                }

                Debug.Log( "NetClient.Disconnect()" );

                socket.Close();
            }
            catch( Exception ex )
            {
                Debug.Log( "NetClient.Disconnect() exception : " + ex.ToString() );

                socket.Close();
            }
        }

        /*
         * 패킷전송
         * 큐가 비어 있을 경우에는 큐에 추가한 뒤 바로 Send 하고
         * 데이터가 들어 있을 경우에는 새로 추가만 한다.
         */
        public void RmiSend( RmiContext rmiContext, Int32 packetID, Message packet )
        {
            packet.ID = packetID;
            packet.RmiContextValue = (byte)rmiContext;
            packet.WriteEnd();
            int queSize = 0;

            lock( csSend )
            {
                queSend.Enqueue( packet );
                queSize = queSend.Count;
            }

            if( queSize > 1 )
                return;

            BeginSend();
        }

        /*
         * 클라이언트 Tick 에서 호출해주어야 한다.
         * Recv 된 패킷이 queue 되어 있을 경우 1 frame 당 1개의 packet 을 OnPacketReceive() 을 통해 전달 해준다
         */
        public void FrameMove()
        {
            if( startParam == null || startParam.IsValid == false )
                return;

            if( ErrorTypeJoinServerComplete != ErrorType.ErrorType_None )
            {
                startParam.OnJoinServerComplete( ErrorTypeJoinServerComplete );
                ErrorTypeJoinServerComplete = ErrorType.ErrorType_None;
            }
            if( ErrorTypeLeaveServer != ErrorType.ErrorType_None )
            {
                startParam.OnLeaveServer( ErrorTypeLeaveServer );
                ErrorTypeLeaveServer = ErrorType.ErrorType_None;

                Disconnect();
            }

            try
            {
                if( socket != null && socket.Connected == false )
                {
                    // 재접속.?
                    switch( ConnectState )
                    {
                    case EConnectState.Connected:
                        {
                            tmReconnectStart = DateTime.Now;
                            ConnectState = EConnectState.Reconnecting;

                            BeginConnect();
                        }
                        break;
                    case EConnectState.ReconnectFail:
                        {
                            var tmValue = DateTime.Now - tmReconnectStart;
                            if( tmValue.TotalSeconds >= 60 )
                            {
                                // 재접속 연결 시간이 지나면 종료하자(1분)
                                ErrorTypeLeaveServer = ErrorType.ErrorType_ReconnectFail;
                                ConnectState = EConnectState.ReconnectClose;

                                break;
                            }

                            ConnectState = EConnectState.Reconnecting;
                            BeginConnect();
                        }
                        break;
                    }

                    return;
                }
            }
            catch( Exception ex )
            {
                Debug.Log( "FrameMove exception : " + ex.ToString() );
            }

            List<Message> vecRecv = new List<Message>();

            lock( csRecv )
            {
                while( queRecv.Count > 0 )
                {
                    vecRecv.Add( queRecv.Dequeue() );
                }
            }

            if( vecRecv.Count > 0 )
            {
                foreach( var bufferRecv in vecRecv )
                {
                    bool IsStub = false;
                    foreach( var stub in listRmiStub )
                    {
                        if( stub.ProcessReceivedMessage( bufferRecv ) )
                        {
                            IsStub = true;
                            break;
                        }
                    }

                    if( IsStub == false && startParam.OnReceiveMessage != null )
                    {
                        startParam.OnReceiveMessage( bufferRecv );
                        IsStub = true;
                    }

                    if( IsStub == false && listRmiStub.Count > 0 )
                    {
                        Debug.Log( "function that a user did not create has been called. : listRmiStub.Count > 0" );
                    }
                }
            }
        }

        public bool IsConnected
        {
            get
            {
                var ret = GetConnectedState();
                return ret == IsConnectedState.OK
                    || ret == IsConnectedState.Reconnect;
            }            
        }

        public IsConnectedState GetConnectedState()
        {
            try
            {
                if( socket == null )
                {
                    Debug.Log( "socket == null" );
                    return IsConnectedState.SocketNone;
                }

                switch( ConnectState )
                {
                case EConnectState.None:
                case EConnectState.ReconnectClose:
                    {
                        return IsConnectedState.SocketNone;
                    }
                case EConnectState.ConnectBegin:
                case EConnectState.ConnectVerify:
                case EConnectState.Connected:
                    {
                        return IsConnectedState.OK;
                    }                    
                case EConnectState.ReconnectBegin:
                case EConnectState.ReconnectFail:
                case EConnectState.Reconnecting:
                case EConnectState.ReconnectVerify:
                    {
                        return IsConnectedState.Reconnect;
                    }
                }                
            }
            catch( Exception ex )
            {
                Debug.Log( "GetConnectedState exception : " +  ex.ToString() );
            }

            return IsConnectedState.SocketNone;
        }

        public void AttachProxy( RmiProxy proxy )
        {
            proxy.m_core = this;
            listRmiProxy.Add( proxy );
        }
        public void AttachStub( RmiStub stub )
        {
            listRmiStub.Add( stub );
        }

        public UInt64 GetPingMS()
        {
            return _PingMS;
        }
    }
}
