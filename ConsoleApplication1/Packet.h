/*
** gummy52
*/

#ifndef _PACKET_H
#define _PACKET_H

#include "stdafx.h"

#pragma pack(push, 1)

struct ServerPktHeader
{
    uint16 size;
    uint16 cmd;
};

struct ClientPktHeader
{
    uint16 size;
    uint32 cmd;
};

#pragma pack(pop)

class Packet : public STLBuffer
{
    public:
        Packet() : 
			STLBuffer(0),
			m_uiOpcode(0),
			m_uiCreatedTime(clock())
        { }

        Packet(const Packet &packet) : 
			STLBuffer(packet),
			m_uiOpcode(packet.m_uiOpcode),
			m_uiCreatedTime(packet.m_uiCreatedTime)
        { }
        
		explicit Packet(const uint16 opcode, const size_t res = 200) :
			STLBuffer(res), 
			m_uiOpcode(opcode),
			m_uiCreatedTime(clock())
		{ }

        void initialize(const uint16 opcode, const size_t newres = 200)
        {
            clear();
            m_vData.reserve(newres);
            m_uiOpcode = opcode;
        }

        void setOpcode(const uint16 opcode) 
		{
			m_uiOpcode = opcode;
		}

        uint16 getOpcode() const 
		{
			return m_uiOpcode;
		}
				
		uint32 m_uiCreatedTime;

    protected:
        uint16 m_uiOpcode;
};

// World of Warcraft packet headers are encrypted.
//	That process not a part of this project
class SocketPacket : public Packet
{
	public:
		SocketPacket() :		
			Packet()
        {}

		SocketPacket(const Packet &packet) : 
			Packet(packet)
        {}

		void AppendSendData(STLBuffer& result, SOCKET sSocket); //, AuthCrypt& crypt)
		bool RecvSelf(SOCKET sSocket); //, AuthCrypt& crypt)
};

#endif


