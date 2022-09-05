/*
 * Copyright (c) 2006-2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 *
 *Code by: Danial Mousavi
 *
 */

/**@Introduction:
*In this version of TLS example, we will use the 'mbed-mqtt' library to *establish a secured connection to a remote MQTT broker (server).
*
*
*
*
*
*/
#include "MQTTClient.h"
#include "MQTTConnect.h"
#include "mbed.h"
#include "ESP8266Interface.h"
#include <MQTTClientMbedOs.h>
#include <cstdint>
#include <cstdio>

/**@NOTES!
*
* The MOD-WiFi-ESP8266 is tested only with K64F, NUCLEO-L432KC and
* NUCLEO-L476RG platforms by the author. There are few technical mysteries
* and issues that have been encountered by the author. Some noteable ones are:
*
*    - Used ESP WiFi module will only works with UART1 interface, with pins D8
*      (PA_9) and D2(PA_10) with NUCLEO-L476RG out of the box, if no changes
*      are made to PeripheralPins.C and the jumpers on the board.
*
*    - NUCLEO-L432KC cannot establish TLS connection.
*      (Problem seems to be memory constraints with L432KC. 64KB of SRAM is not
*       sufficient enough memory!)
*
*    - Do not use "esp8266.built-in-dns" parameter in mbed_app.json file. It           may cause some issues with this example.
*
*
*
*/
ESP8266Interface wifi(MBED_CONF_APP_ESP_TX_PIN, MBED_CONF_APP_ESP_RX_PIN);  //Creat a WiFi interface

// here you add the CA certificate in chain PEM format. The following PEM formatted Root Chain Certificate is from test.mosquitto.org (August, 2022)
const char cert[] ="-----BEGIN CERTIFICATE-----\n"
"MIIFPzCCBCegAwIBAgISBLoxKrvgo1IWjviQTuwWeXGAMA0GCSqGSIb3DQEBCwUA\n"
"MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n"
"EwJSMzAeFw0yMjA1MjQwMzU4NDBaFw0yMjA4MjIwMzU4MzlaMB0xGzAZBgNVBAMT\n"
"EnRlc3QubW9zcXVpdHRvLm9yZzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n"
"ggEBAMVlQixUbp36iRUw1KRGehqqzOA/fqjvYQ/htyherIwrCKMZt6AoDaXTvd8K\n"
"hwOfKr6V71MXWqq+7sq95FhcMJQXS6jMtrxSqUrhvyXtWcnJe9zN2/C6+CvwamOP\n"
"cyHHRY8FNakOIs6G8WsdhHxu7D3CtZYJp2a7UhFBo/UdO1kvJGW2ReU0CZWtBZCg\n"
"wHVsDTI/3hhLWeN1ZVHKyYAifRkd39Owj1ZXFbQyrTAFgxaGjwZ8Z0Vtzgg/5CrB\n"
"2z8kBkG25ywEtKWiXVNiju0EaApHK1M34ZXo9fTMu+kNLlvsJVpLn0EaRNCKJgGm\n"
"OJezsEqo3e3/ylH67/EwDNf8NZcCAwEAAaOCAmIwggJeMA4GA1UdDwEB/wQEAwIF\n"
"oDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIwADAd\n"
"BgNVHQ4EFgQUBkhYCbv9JWU7LSA4P4rRZbi+nWAwHwYDVR0jBBgwFoAUFC6zF7dY\n"
"VsuuUAlA5h+vnYsUwsYwVQYIKwYBBQUHAQEESTBHMCEGCCsGAQUFBzABhhVodHRw\n"
"Oi8vcjMuby5sZW5jci5vcmcwIgYIKwYBBQUHMAKGFmh0dHA6Ly9yMy5pLmxlbmNy\n"
"Lm9yZy8wMgYDVR0RBCswKYISdGVzdC5tb3NxdWl0dG8ub3JnghN0ZXN0Ni5tb3Nx\n"
"dWl0dG8ub3JnMEwGA1UdIARFMEMwCAYGZ4EMAQIBMDcGCysGAQQBgt8TAQEBMCgw\n"
"JgYIKwYBBQUHAgEWGmh0dHA6Ly9jcHMubGV0c2VuY3J5cHQub3JnMIIBBAYKKwYB\n"
"BAHWeQIEAgSB9QSB8gDwAHYAKXm+8J45OSHwVnOfY6V35b5XfZxgCvj5TV0mXCVd\n"
"x4QAAAGA9G5z0QAABAMARzBFAiBRtFSyGzj960BhRCsWR6fHHqFDZKEnqZYdOnXd\n"
"pPKnJQIhAJYxSk81MYMm6sPHMHEk1qzLHox7AveNp0+q+no5FVKWAHYAQcjKsd8i\n"
"RkoQxqE6CUKHXk4xixsD6+tLx2jwkGKWBvYAAAGA9G50BAAABAMARzBFAiEAn1oK\n"
"qeze7kcgsvY4ArRDhQYpLBC/eDLdj740XFrswaUCIBPqE7OeMV4whhxB9JTIlfGh\n"
"VjbMEvXYUPv2Vmq8e0saMA0GCSqGSIb3DQEBCwUAA4IBAQB7reTupf23LS0TowCf\n"
"N5VMFXjoBFyvPzTNMlc2+ywIMrNuSz9mCKQIOzXl86l2KodF3chw4H7X1DYY5wZu\n"
"HZoZem8IRZUwIJBA4n93Jd8lMZjZnGSYxWcALfTORK06znDtQFEYm8dUPwEqMGR3\n"
"/ac4YDphqYSw1K3oVslgeXiMJ53CW6v/nB1QPs0Lhd7iFL0jGndeYlclxst1Ybz5\n"
"ntFl1FW0kKVZ7tyoAx2c6cKLgwRof7XzEL+N93fBJIJs4rWhZHuqYoSNxReJ2UIf\n"
"VjJ7rDHrmW46WYPmzQFqIFYepVRAJlqVEaDmUB97yhG3RZgXv9CPN5PFu3+YSq0N\n"
"IyVR\n"
"-----END CERTIFICATE-----\n"
"-----BEGIN CERTIFICATE-----\n"
"MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\n"
"WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n"
"RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
"AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\n"
"R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\n"
"sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\n"
"NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\n"
"Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\n"
"/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\n"
"AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\n"
"Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\n"
"FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\n"
"AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\n"
"Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\n"
"gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\n"
"PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\n"
"ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\n"
"CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\n"
"lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\n"
"avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\n"
"yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\n"
"yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\n"
"hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\n"
"HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\n"
"MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\n"
"nLRbwHOoq7hHwg==\n"
"-----END CERTIFICATE-----\n"
"-----BEGIN CERTIFICATE-----\n"
"MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/\n"
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
"DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB\n"
"AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC\n"
"ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL\n"
"wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D\n"
"LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK\n"
"4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5\n"
"bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y\n"
"sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ\n"
"Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4\n"
"FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc\n"
"SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql\n"
"PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND\n"
"TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw\n"
"SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1\n"
"c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx\n"
"+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB\n"
"ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu\n"
"b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E\n"
"U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu\n"
"MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC\n"
"5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW\n"
"9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG\n"
"WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O\n"
"he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC\n"
"Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5\n"
"-----END CERTIFICATE-----";


const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

void scan_for_AP(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;

    printf("Scan:\r\n");

    int count = wifi->scan(NULL, 0);

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;

    ap = new WiFiAccessPoint[count];

    count = wifi->scan(ap, count);
    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\r\n", count);

    delete[] ap;
}
static void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
}



int connect_to_wlan(SocketAddress &addr){
    printf("Searching for WiFi APs\r\n\r\n");

    scan_for_AP(&wifi);

    printf("\r\nConnecting to local network...\r\n");
    int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\r\nConnection error\r\n");
        return ret;
    }

    printf("Success\r\n\r\n");
    printf("MAC: %s\r\n", wifi.get_mac_address());
    wifi.get_ip_address(&addr);
    printf("IP: %s\r\n", addr.get_ip_address());
    wifi.get_netmask(&addr);
    printf("Netmask: %s\r\n", addr.get_ip_address());
    wifi.get_gateway(&addr);
    printf("Gateway: %s\r\n", addr.get_ip_address());
    printf("RSSI: %d\r\n\r\n", wifi.get_rssi());

    /* These are not in use! Feel free to try them though! */
    //http_demo(&wifi);
    //wifi.disconnect();
    printf("\r\nDone\r\n");
    return ret;
}



nsapi_size_or_error_t connect_to_broker(nsapi_size_or_error_t &res, MQTTClient &client, MQTTPacket_connectData data){
    if ((res = client.connect(data) != 0)) {
        printf("ERROR: did not connect to broker. code: %d\n", res);
        return res;
    }

    printf("Connected to broker.\n");
    return res;
}


nsapi_size_or_error_t TLSSocket_connect(NetworkInterface *net, TLSSocket *soc, nsapi_size_or_error_t &res, const char* hostname = "test.mosquitto.org", uint16_t port = 8886){
    res = soc->set_root_ca_cert(cert);
    if (res != 0) {
        printf("Error: socket->set_root_ca_cert() returned %d\n", res);
        return res;
    }

    // ATTACH THE SOCKET TO THE NETWORK INTERFACE (in this case, WiFi)
    res = soc->open((*net).wifiInterface());
    if (res != 0) {
        printf("Error! socket->open() returned: %d\n", res);
        return res;
    }
    SocketAddress addr;
    wifi.gethostbyname(hostname, &addr);
    addr.set_port(port);
    
    // CONNECT THE SOCKET TO REMOTE HOST!
    printf("Connecting to %s on port %d\n", hostname, port);
    res = soc->connect(addr);
    if(res != 0) {
        printf("Error! socket->connect() returned: %d\n", res);
        res = soc->connect(addr);
        return res;
    }
    else{
        printf("Secure connection to the host established!\n");
    }
    return res;
}


int main()
{
    //CREAT A SECURE SOCKET
    TLSSocket *socket = new TLSSocket;
    //buffer for NSAPI error messages 
    nsapi_size_or_error_t result;
    //buffer to store IP address information
    SocketAddress a;
    
    MQTTPacket_connectData mqtt_conn_data = MQTTPacket_connectData_initializer;
    mqtt_conn_data.MQTTVersion = 3;
    mqtt_conn_data.clientID.cstring = MBED_CONF_APP_MQTT_ID;
    MQTTClient mqtt_client(socket);
    
    connect_to_wlan(a); 
    TLSSocket_connect(wifi.wifiInterface(), socket, result);
    connect_to_broker(result, mqtt_client, mqtt_conn_data);

    MQTT::Message message;
    char buf[100];
    sprintf(buf, "<!-- WRITE YOUR MESSAGE HERE --!>\r\n");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;

    //Subscribing to a topic 
    result = mqtt_client.subscribe(MBED_CONF_APP_MQTT_TOPIC, MQTT::QOS0, messageArrived);
    if (result != 0) {
    printf("Failed to subscribe to the given topic. Error: %d\n", result);
        return result;
    }
    
    while(true){
        printf("Published a message.\n");
        result = mqtt_client.publish(MBED_CONF_APP_MQTT_TOPIC, message);
        if (result != 0) {
            printf("Failed to publish to the given topic. Error: %d\n", result);
        }
        mqtt_client.yield(2000); //publish ever 2 second
    }
}


