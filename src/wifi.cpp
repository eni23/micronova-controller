#include "Arduino.h"
#include <WiFi.h>

#include "wifi.h"



uint32_t wifi_connect_ticks = 0;


void wifi_connect(){

    // do the static config
    if ( settings.getBool("if-static", IFCONFIG_STATIC) ){

        IPAddress ip;
        IPAddress gateway;
        IPAddress subnet;
        IPAddress dnsserver;  
        
        ip.fromString( settings.getString("if-ip", IFCONFIG_ADDR) );
        gateway.fromString( settings.getString("if-gw", IFCONFIG_GATEWAY) );
        subnet.fromString( settings.getString("if-nm", IFCONFIG_NETMASK) );
        dnsserver.fromString( settings.getString("if-dns", IFCONFIG_DNS) );

        if (! WiFi.config(ip, gateway, subnet, dnsserver) ){
            printf("ERR: static wifi config failed\n");
            return;
        }
    }

    WiFi.setAutoReconnect(true);
    WiFi.mode(WIFI_STA);

    WiFi.begin(
        settings.getString("wifi-ssid", WIFI_SSID).c_str(), 
        settings.getString("wifi-pass", WIFI_PASS).c_str()
    );

    printf("Connecting to WiFi ");

    wifi_connect_ticks = 0;
    while (WiFi.status() != WL_CONNECTED) {
        //printf(".");
        delay(250);
        if (wifi_connect_ticks>WIFI_TRY_CONNECT_TICKS){
            printf("Error connecting after %i ticks\n", WIFI_TRY_CONNECT_TICKS);
            return;
        }
    }
    printf(" OK \n");
}

void wifi_disconnect(){
    WiFi.disconnect();
}

void wifi_reconnect(){
    wifi_disconnect();
    delay(100);
    wifi_connect();
}


