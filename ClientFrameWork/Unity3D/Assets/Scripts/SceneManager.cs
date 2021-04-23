using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SceneManager : MonoBehaviour
{
    public Text txNoti;

    [SerializeField]
    Button btConnect;
    [SerializeField]
    Button btLogInReq;

    // Start is called before the first frame update
    void Start()
    {
        btConnect.onClick.AddListener( _OnClickConnect );
        btLogInReq.onClick.AddListener( _OnClickLogInReq );
    }

    void _OnClickConnect()
    {
        if( EchoServer.Ins.Connect() == false )
        {
            Debug.LogError( "failed EchoServer Connect.!" );
            txNoti.text = "EchoServer 접속 실패";
        }
    }
    void _OnClickLogInReq()
    {
        if( EchoServer.Ins.IsConnected == false )
        {
            txNoti.text = "서버 접속이 되어 있지 않음.";
            return;
        }

        EchoServer.Ins.proxy.LogInReq( DHNet.RmiContext.Reliable, "test", DateTime.Now, new List<string>() );
    }
}
