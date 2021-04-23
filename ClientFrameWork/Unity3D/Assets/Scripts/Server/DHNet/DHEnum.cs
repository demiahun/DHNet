using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DHNet
{
    public enum ErrorType
    {
        ErrorType_None = -1,

        ErrorType_Ok = 0,
        /*
         * 연결 실패
         */
        ErrorType_TCPConnectFailure,
        /*
         * StartParam 에 콜백 함수 할당을 하지 않음.
         */
        ErrorType_InvalidCallbackFunc,

        ErrorType_StartFailListen,
        /*
         * 재접속 연결 실패
         */
        ErrorType_ReconnectFail,
        /*
         * 이미 연결되어 있음
         */
        ErrorType_AlreadyConnected,
        /*
		 * 상대측 호스트에서 연결을 해제했음
		 */
        ErrorType_DisconnectFromRemote,
    }

    /*
     * ReliableCompress, FastEncrypCompress : 패킷을 압축하는 플러그로 패킷 데이터가 500 Byte 이상이여야 효과가 있음
     */
    public enum RmiContext
    {
        Reliable,        
        FastEncryp,

        ReliableCompress,
        FastEncrypCompress,
    }

    public enum IsConnectedState
    {
        OK = 0,
        SocketNone,
        Reconnect,
        SocketNotConnected,
    }
}
