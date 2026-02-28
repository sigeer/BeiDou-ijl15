#pragma once
class NetService
{
public:
    static void PostLanguage(unsigned char language);
    static void PostHpMpAlert(DWORD hp, DWORD mp);

    static void ConfigureRecvPacket();
};