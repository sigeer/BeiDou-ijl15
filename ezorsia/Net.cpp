#include "stdafx.h"
#include "Packet.h"
#include "Net.h"

const DWORD clientSocketAddr = 0x00BE7914;
const DWORD COutPacketAddr = 0x0049637B;
const DWORD CInPacketAddr = 0x004965F1;

static auto ClientSocket = reinterpret_cast<void**>(clientSocketAddr);

typedef void(__fastcall* PacketSend)(void* clientSocket, int dummy, COutPacket& packet);
static auto Send = reinterpret_cast<PacketSend>(COutPacketAddr);

typedef void(__fastcall* PacketRecv)(void* pthis, int dummy, CInPacket* in_pack);
static auto Recv = reinterpret_cast<PacketRecv>(CInPacketAddr);

PacketRecv RecvHookInternal = [](PVOID pThis, int dummy, CInPacket* in_pack) -> void
    {
        short pId = in_pack->GetOpCode();
        // µÇÂ¼½á¹û
        if (pId == 0x00)
        {
            if (Client::SwitchChinese)
                NetService::PostLanguage(0);
            else
                NetService::PostLanguage(1);
        }
        if (pId == 0x1001) 
        {
            unsigned char hpAlert = in_pack->GetCode(0);
            unsigned char mpAlert = in_pack->GetCode(1);
            Client::SetHpMpAlert(hpAlert, mpAlert);
        }
        Recv(pThis, dummy, in_pack);
    };


void NetService::PostLanguage(unsigned char language) {
    COutPacket writer(0x1000);
    writer.Encode1(language);
    Send(*ClientSocket, 0, writer);
}

void NetService::PostHpMpAlert(DWORD hpRatio, DWORD mpRatio) {
    COutPacket writer(0x1001);
    writer.Encode1(hpRatio);
    writer.Encode1(mpRatio);
    Send(*ClientSocket, 0, writer);
}

void NetService::ConfigureRecvPacket() {
	Memory::SetHook(true, reinterpret_cast<void**>(&Recv), RecvHookInternal);
}