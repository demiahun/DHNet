#pragma once



class EventInfo
{
public:
	CDHTime tmStart;
	CDHTime tmEnd;
	int		iValue01 = 0;
	int		iValue02 = 0;
};

namespace DHNet
{
	inline CMessage& operator >> ( CMessage &a, EventInfo &b )
	{
		a >> b.tmStart;
		a >> b.tmEnd;
		a >> b.iValue01;
		a >> b.iValue02;
		return a;
	}
	inline CMessage& operator<<( CMessage &a, const EventInfo &b )
	{
		a << b.tmStart;
		a << b.tmEnd;
		a << b.iValue01;
		a << b.iValue02;
		return a;
	}
}