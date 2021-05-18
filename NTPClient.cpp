#include "Arduino.h"
#include "NTPClient.h"
#include "WiFi.h"
#include "WiFiUdp.h"
#include "IPAddress.h"

// constructors

NTPClient::NTPClient(WiFiUDP udp)
        : _udp(udp) {
    _udpPort = NTP_DEFAULT_PORT;
    _server = NTP_DEFAULT_SERVER;
    _timeZone = NTP_DEFAULT_TIMEZONE;
};

NTPClient::NTPClient(WiFiUDP udp, int timeZone)
        : _udp(udp), _timeZone(timeZone) {
    _udpPort = NTP_DEFAULT_PORT;
    _server = NTP_DEFAULT_SERVER;
};

NTPClient::NTPClient(WiFiUDP udp, String server, int timeZone)
        : _udp(udp), _server(server), _timeZone(timeZone) {
    _udpPort = NTP_DEFAULT_PORT;
};

NTPClient::NTPClient(WiFiUDP udp, String server, u_int port, int timeZone)
        : _udp(udp), _server(server), _udpPort(port), _timeZone(timeZone) {
};

// class methods

void NTPClient::initUdp() {
    Serial.println("Starting UDP for NTPClient...");
    _udp.begin(_udpPort);
};

time_t NTPClient::getNTPTime() {
    Serial.println("clearing packets");
    while(_udp.parsePacket() > 0) ; // clear previous incoming udp packets
    Serial.println("...clear");

    // get an IP address from the NTP server pool
    IPAddress ntpServerIP;
    WiFi.hostByName(_server.c_str(), ntpServerIP);

    Serial.print("NTP server IP: ");
    ntpServerIP.printTo(Serial); Serial.println();

    // send NTP request
    sendNTPRequestPacket(ntpServerIP);

    // await response
    return receiveNTPResponsePacket();
};

const byte NTP_LEAP_INDICATOR_BITS = 0b11000000; // LI: first 2 bits (3 = unsynchronized)
const byte NTP_VERSION_BITS        = 0b00100000; // Version: next 3 bits(4 = current version)
const byte NTP_MODE_BITS           = 0b00000011; // Mode: last 3 bits (3 = client)

// Packet format details: https://labs.apnic.net/?p=462
void NTPClient::sendNTPRequestPacket(IPAddress &addr) {
    //fill packet buffer with 0's
    memset(packetBuffer, 0, NTP_PACKET_BUFFER_SIZE);

    // Initialize values needed to form NTP request
    packetBuffer[0] = NTP_LEAP_INDICATOR_BITS | NTP_VERSION_BITS | NTP_MODE_BITS; // first byte: LI, Version, Mode
    packetBuffer[1] = 0;     // second byte: Stratum, or type of clock (0 = Unspecified)
    packetBuffer[2] = 6;     // third byte:  Polling Interval (6 seconds)
    packetBuffer[3] = 0xEC;  // fourth byte: Peer Clock Precision (0xEC = 236 for ~ 2.4GHz (I think...))

    // 4 bytes of 0 for Root Delay - packetBuffer[4] ... packetBuffer[7]

    // 4 bytes of 0 for Root Dispersion - packetBuffer[8] ... packetBuffer[11]

    // 4 bytes of ASCII codes for Reference Identifier
    // WHY THE FUCK is it "1N14" when that isn't a valid source according to the NTP protocol source list?
    // https://forum.arduino.cc/t/udp-ntp-clients/95868
    // doesn't seem to matter though, soo.... oh well, don't fix it if it ain't broke
    packetBuffer[12] = 49;   // 1
    packetBuffer[13] = 0x4E; // N
    packetBuffer[14] = 49;   // 1
    packetBuffer[15] = 52;   // 4

    //packet initialized, time to send it to the time-server.
    _udp.beginPacket(addr, NTP_REQUEST_PORT);
    _udp.write(packetBuffer, NTP_PACKET_BUFFER_SIZE);
    _udp.endPacket();
    Serial.println("NTP packet sent!");
};

time_t NTPClient::receiveNTPResponsePacket() {
    //set begin wait time
    u_long beginWait = millis();

    //continuously check for response until timed out
    while(millis() - beginWait < NTP_RESPONSE_WAIT_TIME) {
        //get response size
        int size = _udp.parsePacket();

        //check response size
        if(size >= NTP_PACKET_BUFFER_SIZE) {
            Serial.println("NTP Response packet received");

            // read response into packet buffer
            _udp.read(packetBuffer, NTP_PACKET_BUFFER_SIZE);

            // parse 4 bytes from the response into a 32 bit unsigned integer (long)
            // response time is a 64 bit value, with 32 bits each for the integer and decimal portions
            // we don't need to be super accurate, and our time is stored as a non-decimal value
            // so we only need to get the first 32 bits of the time in the response.
            u_long secSince1900;
            secSince1900 = (u_long)packetBuffer[40] << 24; // first byte, shifted 24 bits left, leaving room for 3 more
            secSince1900 |= (u_long)packetBuffer[41] << 16; // second byte, shifted 16 bits left, leaving room for 2 more
            secSince1900 |= (u_long)packetBuffer[42] << 8; // third byte, shifted 8 bits left, leaving room for 1 more
            secSince1900 |= (u_long)packetBuffer[43]; // fourth and final byte, not shifted, since it's the last one.

            //convert from UTC to unix time (seconds since 1970)
            u_long unixTime = secSince1900 - NTP_UNIX_TIME_OFFSET;

            //account for timezone
            unixTime += _timeZone * NTP_SECONDS_PER_HOUR;

            //return time to calling function
            return unixTime;
        }
    }
    
    // return 0 if no response received
    Serial.println("No response received from NTP server");
    return 0;
};