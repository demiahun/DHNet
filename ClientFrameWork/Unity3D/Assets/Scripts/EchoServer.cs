using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using DHNet;

public class EchoServer : Singleton<EchoServer>
{
    public CNetClient netClient;
    public TestC2S.Proxy proxy = new TestC2S.Proxy();
    public TestS2C.Stub stub = new TestS2C.Stub();

    public bool ConnectServerSuccess = false;

    public bool Connect()
    {
        if( netClient == null )
        {
            netClient = new CNetClient();

            netClient.AttachProxy( proxy );
            netClient.AttachStub( stub );

            stub.LogInAck = LogInAck;
        }

        CNetConnectionParam param = new CNetConnectionParam();
        param.hostname = "127.0.0.1";
        param.port = 21010;
        param.OnJoinServerComplete = OnJoinServerComplete;
        param.OnLeaveServer = OnLeaveServer;

        if( netClient.Connect( param ) != ErrorType.ErrorType_Ok )
        {
            NGLog.Log( "CenterServer Connect fail.!" );
            return false;
        }

        return true;
    }
    public void Disconect()
    {
        ConnectServerSuccess = false;

        if( netClient == null || netClient.IsConnected == false )
            return;

        netClient.Disconnect();
    }
    public bool IsConnected
    {
        get
        {
            if( netClient == null )
                return false;
            // 실제 JoinServerComplete func이 호출되고 나서야 연결 성공.
            if( false == ConnectServerSuccess )
            {
                return false;
            }
            return netClient.IsConnected;
        }        
    }

    void OnJoinServerComplete( ErrorType errorType )
    {
        Debug.Log( "OnJoinServerComplete : " + errorType.ToString() );

        if( errorType == ErrorType.ErrorType_Ok && ConnectServerSuccess == false )
        {
            ConnectServerSuccess = true;
        }

        if( errorType != ErrorType.ErrorType_Ok )
        {
            Debug.LogError( "failed EchoServer Connet.!" );
        }
    }

    void OnLeaveServer( ErrorType errorType )
    {
        Debug.Log( "OnLeaveServer() " + errorType.ToString() );
    }

    bool LogInAck( string deviceID, DateTime tmValue, List<string> vecValue )
    {
        var sceneManager = GameObject.Find( "SceneManager" )?.GetComponent<SceneManager>();

        if( sceneManager != null )
        {
            sceneManager.txNoti.text = "LogInAck 받음.!";
        }
        return true;
    }
}
