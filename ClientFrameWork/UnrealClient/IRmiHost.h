// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicType.h"
#include "NgEnum.h"



namespace DHNet
{
	class IRmiHost
	{
	public:
		IRmiHost(){}
		virtual ~IRmiHost(){}

		virtual void RmiSend( HostID remote, RmiContext rmiContext, Int32 packetID, class CMessage *msg ){}
	};
}
