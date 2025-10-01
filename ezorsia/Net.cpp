#include "stdafx.h"
#include "Packet.h"
#include "Net.h"

const DWORD clientSocketAddr = 0x00BE7914;
const DWORD COutPacketAddr = 0x0049637B;
const DWORD CInPacketAddr = 0x004965F1;

static auto ClientSocket = reinterpret_cast<void**>(clientSocketAddr);

typedef void(__fastcall* PacketSend)(void* clientSocket, int dummy, COutPacket& packet);
static auto Send = reinterpret_cast<PacketSend>(COutPacketAddr);

typedef int(__fastcall* PacketRecv)(void* pthis, int dummy, CInPacket* in_pack);
static auto Recv = reinterpret_cast<PacketRecv>(CInPacketAddr);

PacketRecv RecvHookInternal = [](PVOID pThis, int dummy, CInPacket* in_pack) -> int
    {
        short pId = in_pack->GetOpCode();
        // µÇÂ¼½á¹û
        if (pId == 0x00)
        {
            NetService::PostLanguage();
        }
        return Recv(pThis, 0, in_pack);
    };


void NetService::RecvHook() {
	Memory::SetHook(true, reinterpret_cast<void**>(&Recv), RecvHookInternal);
}

void NetService::PostLanguage() {
    COutPacket writer(0x1000);
    if (Client::SwitchChinese)
        writer.Encode1(0);
    else
        writer.Encode1(1);
    Send(*ClientSocket, 0, writer);
}