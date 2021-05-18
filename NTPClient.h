#ifndef NTPCLIENT_H
#define NTPCLIENT_H

#include "WiFiUdp.h"

//defaults
const u_int NTP_DEFAULT_PORT = 8888;
const char NTP_DEFAULT_SERVER[] = "us.pool.ntp.org";
const int NTP_DEFAULT_TIMEZONE = -6; // Central Standard Time = UTC-6:00


const int NTP_PACKET_BUFFER_SIZE = 48;     // packet buffer size = 48 bytes
const byte NTP_REQUEST_PORT        = 123;  // NTP requests go to port 123
const byte NTP_RESPONSE_WAIT_TIME  = 1500; // wait up to 1500ms for a response from the NTP server
const u_long NTP_UNIX_TIME_OFFSET = 2208988800UL; // number of seconds between 1/1/1900 and 1/1/1970
const u_int NTP_SECONDS_PER_HOUR = 60 * 60;

class NTPClient {
    private:
        WiFiUDP _udp;
        u_int _udpPort;
        String _server;
        int _timeZone;
        byte packetBuffer[NTP_PACKET_BUFFER_SIZE];
    public:
        //NTPClient(); // empty constructor to allow declaration before initialization
        NTPClient(WiFiUDP); //udp only
        NTPClient(WiFiUDP, int); //udp, timezone
        NTPClient(WiFiUDP, String, int); //udp, server, timezone
        NTPClient(WiFiUDP, String, u_int, int); //udp, server, port, timezone

        void initUdp();
        time_t getNTPTime();
        void sendNTPRequestPacket(IPAddress&);
        time_t receiveNTPResponsePacket();
};

#endif