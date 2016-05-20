/*
** gummy52
*/

#include "stdafx.h"

void SocketPacket::AppendSendData(STLBuffer& result, SOCKET sSocket) //, AuthCrypt& crypt)
{
	ServerPktHeader header;

	header.cmd = getOpcode();
	//EndianConvert(header.cmd);

	header.size = (uint16)getSize() + 2;
	//EndianConvertReverse(header.size);

	// World of Warcraft packet headers are encrypted.
	//	That process not a part of this project
	//crypt.EncryptSend((uint8*)&header, sizeof(header));

	result << header.size;
	result << header.cmd;

	if (getSize())
		result.append((uint8*)getContents(), getSize());
}

bool SocketPacket::RecvSelf(SOCKET sSocket) //, AuthCrypt& crypt)
{
	STLBuffer headerData;
	headerData.resize(sizeof(ClientPktHeader));

	if (Util::recvBytes(sSocket, (char*)headerData.getContents(), (uint32)headerData.getSize()) != headerData.getSize())
		return false;

	m_uiCreatedTime = clock();

	// World of Warcraft packet headers are encrypted.
	//	That process not a part of this project

	// crypt.DecryptRecv((uint8*)headerData.getContents(), headerData.getSize());
	ClientPktHeader header = *((ClientPktHeader*)headerData.getContents());
	// EndianConvertReverse(header.size);
	// EndianConvert(header.cmd);

	if ((header.size < 4) || (header.size > 10240) || (header.cmd > 10240))
		return false;

	header.size -= 4;

	clear();
	setOpcode(header.cmd);

	// It's fine to receive a packet who is nothing but an opcode.
	if (!header.size)
		return true;

	resize(header.size);
	return Util::recvBytes(sSocket, (char*)getContents(), (uint32)getSize()) == (uint32)getSize();
}