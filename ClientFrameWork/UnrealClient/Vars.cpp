#include "Vars.h"


namespace DHNet
{
	/*
	NGServerGroupInfo packet
	*/
	CMessage& operator >> ( CMessage &a, EventInfo &b )
	{
		a >> b.tmStart;
		a >> b.tmEnd;
		a >> b.iValue01;
		a >> b.iValue02;
		return a;
	}
	CMessage& operator<<( CMessage &a, const EventInfo &b )
	{
		a << b.tmStart;
		a << b.tmEnd;
		a << b.iValue01;
		a << b.iValue02;
		return a;
	}
}