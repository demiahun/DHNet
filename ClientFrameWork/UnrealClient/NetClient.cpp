// Fill out your copyright notice in the Description page of Project Settings.

#include "NetClient.h"
#include "Message.h"
#include "MessageMarshal.h"
#include "IRmiStub.h"
#include "IRmiProxy.h"
#include "NgCrypto.h"
#include "ZipHelper.h"


namespace DHNet
{
	FNetClient::FNetClient()
	{

	}

	FNetClient::~FNetClient()
	{
		UE_LOG( LogTemp, Warning, TEXT( "~FNetClient()" ) );
	}

	uint32 FNetClient::Run()
	{
		UE_LOG( LogTemp, Warning, TEXT( "Start NetClient Thread ==== (ThreadID(%d)" ), FPlatformTLS::GetCurrentThreadId() );

		while( bRun )
		{
			if( !Socket )
			{
				UE_LOG( LogTemp, Error, TEXT( "Socket is null. FNetClient.cpp: line %d" ), __LINE__ );

				bRun = false;
				continue;
			}

			// 소켓이 끈겼다면 쓰레드를 종료하자
			Socket->SetNonBlocking( true );
			int32 t_BytesRead;
			uint8 t_Dummy;
			if( Socket->Recv( &t_Dummy, 1, t_BytesRead, ESocketReceiveFlags::Peek ) == false )
			{
				bRun = false;
				continue;
			}
			Socket->SetNonBlocking( false );

			/*
				보낼 정보가 있으면 send
			*/
			if( queSend.IsEmpty() == false )
			{
				CMessage *msgSend = nullptr;
				queSend.Dequeue( msgSend );

				if( msgSend != nullptr )
				{
					SequenceNumSend.Increment();
					msgSend->SetSequenceNum( SequenceNumSend.GetValue() );

					switch( msgSend->GetRmiContext() )
					{
					case RmiContext::ReliableCompress:
						{
							CompressToMessage( msgSend );
						}
						break;
					case RmiContext::FastEncryp:
						{
							DHNet::XOREncrypt( msgSend->m_buffer + HEADSIZE, msgSend->m_Length - HEADSIZE );
						}
						break;
					case RmiContext::FastEncrypCompress:
						{
							XOREncrypt( msgSend->m_buffer + HEADSIZE, msgSend->m_Length - HEADSIZE );

							CompressToMessage( msgSend );
						}
						break;
					}

					Int32 bytesSend = 0;
					if( Socket->Send( (uint8*)msgSend->m_buffer, msgSend->m_Length, bytesSend ) == false )
					{
						UE_LOG( LogTemp, Error, TEXT( "failed socket send. FNetClient.cpp: line %d" ), __LINE__ );

						GMessagePool.Free( msgSend );

						bRun = false;
						continue;
					}

					GMessagePool.Free( msgSend );
				}
			}

			/*
				받은 정보가 있나.?
				HasPendingData() 를 체크하여 하나의 패킷이 완료 될때까지 while 을 돌자
			*/
			UInt32 PendingDataSize = 0;
			Int32 BytesReadTotal = 0;
			TArray<uint8> receivedData;
			while( true )
			{
				if( Socket->HasPendingData( PendingDataSize ) == false )
				{
					break;
				}

				receivedData.SetNumUninitialized( BytesReadTotal + PendingDataSize );

				Int32 BytesRead = 0;
				if( !Socket->Recv( receivedData.GetData() + BytesReadTotal, receivedData.Num(), BytesRead ) )
				{
					// error code: (int32)SocketSubsystem->GetLastErrorCode()
					UE_LOG( LogTemp, Error, TEXT( "In progress read failed. FNetClient.cpp: line %d" ), __LINE__ );

					break;
				}
				BytesReadTotal += BytesRead;
			}

			if( receivedData.Num() > 0 )
			{
				auto msg = CMessage::Create();

				FMemory::Memcpy( msg->m_buffer, receivedData.GetData(), receivedData.Num() );

				switch( msg->GetRmiContext() )
				{
				case RmiContext::ReliableCompress:
					{
						UncompressToMessage( msg );
					}
					break;
				case RmiContext::FastEncryp:
					{
						XORDecrypt( msg->m_buffer + HEADSIZE, msg->GetLength() - HEADSIZE );
					}
					break;
				case RmiContext::FastEncrypCompress:
					{
						UncompressToMessage( msg );

						XORDecrypt( msg->m_buffer + HEADSIZE, msg->GetLength() - HEADSIZE );
					}
					break;
				}

				if( msg->GetID() < 0 )
				{
					_OnReceiveMessage( msg );
				}
				else
				{
					queRecv.Enqueue( msg );
				}
			}
		}

		return 0;
	}

	void FNetClient::RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, class CMessage *msg )
	{
		msg->SetRmiContext( rmiContext );
		msg->SetID( packetID );
		msg->WriteEnd();

		queSend.Enqueue( msg );
	}

	bool FNetClient::Start( FString hostName, int32 port )
	{
		// 플랫폼에 맞는 소켓생성
		Socket = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->CreateSocket( NAME_Stream, TEXT( "NetClient" ), false );

		if( !Socket )
		{
			UE_LOG( LogTemp, Error, TEXT( "failed create Socket.!" ) );
			return false;
		}

		FIPv4Address ip;
		FIPv4Address::Parse( hostName, ip );

		auto internetAddr = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->CreateInternetAddr();
		internetAddr->SetIp( ip.Value );
		internetAddr->SetPort( port );

		if( Socket->Connect( *internetAddr ) == false )
		{
			UE_LOG( LogTemp, Error, TEXT( "failed Conenct Socket.!" ) );
			return false;
		}

		// 쓰레드를 생성하여 Run() 을 통해 packet recv, send 가 이루어 지도록 함
		bRun = true;
		Thread = FRunnableThread::Create( this, *FString::Printf( TEXT( "FSocketWorker %s:%d" ), *HostName, Port ), 128 * 1024, TPri_Normal );

		HostName = hostName;
		Port = port;

		UE_LOG( LogTemp, Warning, TEXT( "Start NetClient ====(ThreadID(%d)" ), FPlatformTLS::GetCurrentThreadId() );

		return true;
	}

	void FNetClient::End()
	{
		bRun = false;

		if( Socket != nullptr )
		{
			Socket->Close();
		}

		if( Thread )
		{
			Thread->WaitForCompletion();
			delete Thread;
			Thread = nullptr;
		}

		UE_LOG( LogTemp, Warning, TEXT( "End NetClient ====" ) );
	}

	void FNetClient::FrameMove()
	{
		if( queRecv.IsEmpty() == false )
		{
			CMessage *msg = nullptr;
			queRecv.Dequeue( msg );

			if( msg != nullptr )
			{
				for( auto rmi : vecStub )
				{
					bool ret = rmi->ProcessReceivedMessage( msg->m_HostID, msg );
					if( ret )
						return;
				}

				UE_LOG( LogTemp, Error, TEXT( "function that a user did not create has been called.(ID : %d)" ), msg->GetID() );

				GMessagePool.Free( msg );
			}
		}
	}

	void FNetClient::AttachStub( IRmiStub *stub )
	{
		vecStub.Add( stub );
	}

	void FNetClient::AttachProxy( IRmiProxy *proxy )
	{
		proxy->m_core = this;
		vecProxy.Add( proxy );
	}

	bool FNetClient::IsConnected()
	{
		if( Socket == nullptr )
			return false;

		if( Socket->GetConnectionState() != SCS_Connected )
			return false;

		if( bVerify == false )
			return false;

		return true;
	}

	void FNetClient::_OnReceiveMessage( class CMessage *msg )
	{
		switch( msg->GetID() )
		{
		case PACKETID_HEART_BIT:
			{
				auto msgSend = CMessage::Create();
				msg->CopyTo( msgSend );
				RmiSend( HostID_None, RmiContext::Reliable, PACKETID_HEART_BIT, msgSend );
			}
			break;
		case PACKETID_SC_HOSTID_INFO:
			{
				*msg >> m_HostID;
				UInt32 xorKey0, xorKey1, xorKey2;
				*msg >> xorKey0;
				*msg >> xorKey1;
				*msg >> xorKey2;

				DHNet::XOR_KEY0 = xorKey0;
				DHNet::XOR_KEY1 = xorKey1;
				DHNet::XOR_KEY2 = xorKey2;

				auto msgSend = CMessage::Create();
				RmiSend( HostID_None, RmiContext::Reliable, PACKETID_CS_HOSTID_RECV, msgSend );

				bVerify = true;

				UE_LOG( LogTemp, Warning, TEXT( "FNetClient Verify x0(%d), X1(%d), X2(%d)" ), xorKey0, xorKey1, xorKey2 );
			}
			break;
		}

		GMessagePool.Free( msg );
	}

}
