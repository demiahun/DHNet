using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DHNet
{
    public abstract class RmiProxy
    {
        public CNetClient m_core = null;

        public RmiProxy()
        {
        }
        ~RmiProxy()
        {
        }

        public virtual void RmiSend( RmiContext rmiContext, Int32 packetID, Message msg )
        {
            m_core.RmiSend( rmiContext, packetID, msg );
        }
    }
}
