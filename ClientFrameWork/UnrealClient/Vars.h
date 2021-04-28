#pragma once

#include "CoreMinimal.h"
#include "Message.h"
#include "MessageMarshal.h"


class EventInfo
{
public:
	FDateTime tmStart;
	FDateTime tmEnd;
	int		iValue01 = 0;
	int		iValue02 = 0;
};

namespace DHNet
{
	CMessage& operator >> ( CMessage &a, EventInfo &b );
	CMessage& operator<<( CMessage &a, const EventInfo &b );
}