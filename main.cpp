/*
 * Copyright (c) 2006-2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 *
 *Code by: Danial Mousavi
 *
 */

/* This example is based on Mbed official toturial found here: https://os.mbed.com/docs/mbed-os/v6.15/apis/connectivity-tutorials.html*/
#include "mbed.h"
#include "ESP8266Interface.h"
#include "mbed_trace.h"


ESP8266Interface wifi(D8, D2);

// here you add the CA certificate in PEM format. The following PEM chain certificate is from os.mbed.com
const char cert[] =\
"-----BEGIN CERTIFICATE-----\n"
"MIIGfDCCBWSgAwIBAgIQD2nY7yl9rIxwC4SfxN9vczANBgkqhkiG9w0BAQsFADBG\n"
"MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRUwEwYDVQQLEwxTZXJ2ZXIg\n"
"Q0EgMUIxDzANBgNVBAMTBkFtYXpvbjAeFw0yMjAzMzEwMDAwMDBaFw0yMzA0Mjky\n"
"MzU5NTlaMBUxEzARBgNVBAMMCioubWJlZC5jb20wggEiMA0GCSqGSIb3DQEBAQUA\n"
"A4IBDwAwggEKAoIBAQCp/wBXDP6oTt7dlVgISfqSb6yE3pjuNfKLZ9/vvNW3xoFt\n"
"dj3Wq7ERbMg1N69Miw5LszCeQDTnoagyMAtUARkuOiylof6H69K4deZXHUaLY7dy\n"
"uDUx0SfUSawEsAfO2fl6mt8Wk/Nn1z0vERMN8+49/EJsU5rvxNj/MSBcpLDTMuK3\n"
"4QRcbNHDDSpIzuQ4n+634Q517e/67B3HEBn1CqQ4ZZXT4D1mgVwtYiqkQ/xTZhBi\n"
"1F0MVXtBdqDc77RL3cv1CS8LffPT9hzunxjegUk+pat2vUIde777IRFMRqVI3E1G\n"
"P3huXVY1lbrW5tAzF3JPKiEL9wQX28rdZjGB3qS7AgMBAAGjggOVMIIDkTAfBgNV\n"
"HSMEGDAWgBRZpGYGUqB7lZI8o5QHJ5Z0W/k90DAdBgNVHQ4EFgQUtbfs4ejAo3OB\n"
"2xJcQyBJimj+fdEwgcMGA1UdEQSBuzCBuIIKKi5tYmVkLmNvbYIQYXBpLmtlaWwu\n"
"YXJtLmNvbYIKKi5tYmVkLm9yZ4ISKi5hcGkua2VpbC5hcm0uY29tggxrZWlsLmFy\n"
"bS5jb22CCG1iZWQuY29tghMqLmludGVybmFsLm1iZWQuY29tgg1jb3JlLm1iZWQu\n"
"Y29tgg4qLmtlaWwuYXJtLmNvbYIIbWJlZC5vcmeCDyouY29yZS5tYmVkLmNvbYIR\n"
"aW50ZXJuYWwubWJlZC5jb20wDgYDVR0PAQH/BAQDAgWgMB0GA1UdJQQWMBQGCCsG\n"
"AQUFBwMBBggrBgEFBQcDAjA9BgNVHR8ENjA0MDKgMKAuhixodHRwOi8vY3JsLnNj\n"
"YTFiLmFtYXpvbnRydXN0LmNvbS9zY2ExYi0xLmNybDATBgNVHSAEDDAKMAgGBmeB\n"
"DAECATB1BggrBgEFBQcBAQRpMGcwLQYIKwYBBQUHMAGGIWh0dHA6Ly9vY3NwLnNj\n"
"YTFiLmFtYXpvbnRydXN0LmNvbTA2BggrBgEFBQcwAoYqaHR0cDovL2NydC5zY2Ex\n"
"Yi5hbWF6b250cnVzdC5jb20vc2NhMWIuY3J0MAwGA1UdEwEB/wQCMAAwggF/Bgor\n"
"BgEEAdZ5AgQCBIIBbwSCAWsBaQB2AOg+0No+9QY1MudXKLyJa8kD08vREWvs62nh\n"
"d31tBr1uAAABf91acgIAAAQDAEcwRQIgbqpJMSdFOBkqySj8oUPN5Bshb57uo44T\n"
"blkZJKw3GYwCIQDEgGnrnLzfeX4BOMhlIroq82LhaBOTHycXNeXYCc0rQAB2ADXP\n"
"GRu/sWxXvw+tTG1Cy7u2JyAmUeo/4SrvqAPDO9ZMAAABf91acisAAAQDAEcwRQIh\n"
"AJdDHSLjxb+svixRO3KR/MT4vaUzcvmCHBDtWl5Rq4PlAiA8Wl/3wy1dOtJWUipN\n"
"fu99x9UzGMWLelWhEDPkOKStIgB3ALNzdwfhhFD4Y4bWBancEQlKeS2xZwwLh9zw\n"
"Aw55NqWaAAABf91ackMAAAQDAEgwRgIhALOFeln6Ih/+QNuORSvFFHWBUJ2zJvmu\n"
"b0OWAuvXOFVKAiEAvqJxfU1LZodYzwrxOiqFtaN1yqMmJz5pqigcFBGXCmQwDQYJ\n"
"KoZIhvcNAQELBQADggEBABZXWIOva9dRjGKCpGOZwP0eV7uX/KrH6Y9aCQpLQn1+\n"
"CeqpNYLdHZDeAjsrMRYb/gKZ1ea5E989sxuxv+PPu2mzFV3EArhv0iLzNWHvJxeW\n"
"2vTF5Aq9o3vec0WjrtAWtmk4jJgaXlhz3j4lA7W6f9Zm/epOzyQgiro/jEb7pZti\n"
"QiexaC9+KW8Uu8aXHMjidwZC1UC0WcUfALLvs/UeQd8YgL4P+7dLJqyGpfFcf5sX\n"
"GU+1yDb25SQvJzWhhJqhugy7whHEIWDM9CbuONu4J1t6rMMrb9Dd1rs5sMSJqEDf\n"
"eGfKmmfduw1q70jHdgF1o+N2DZ7erIxAjYB1jKn9nuU=\n"
"-----END CERTIFICATE-----\n"
"-----BEGIN CERTIFICATE-----\n"
"MIIESTCCAzGgAwIBAgITBntQXCplJ7wevi2i0ZmY7bibLDANBgkqhkiG9w0BAQsF\n"
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
"b24gUm9vdCBDQSAxMB4XDTE1MTAyMTIyMjQzNFoXDTQwMTAyMTIyMjQzNFowRjEL\n"
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEVMBMGA1UECxMMU2VydmVyIENB\n"
"IDFCMQ8wDQYDVQQDEwZBbWF6b24wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
"AoIBAQDCThZn3c68asg3Wuw6MLAd5tES6BIoSMzoKcG5blPVo+sDORrMd4f2AbnZ\n"
"cMzPa43j4wNxhplty6aUKk4T1qe9BOwKFjwK6zmxxLVYo7bHViXsPlJ6qOMpFge5\n"
"blDP+18x+B26A0piiQOuPkfyDyeR4xQghfj66Yo19V+emU3nazfvpFA+ROz6WoVm\n"
"B5x+F2pV8xeKNR7u6azDdU5YVX1TawprmxRC1+WsAYmz6qP+z8ArDITC2FMVy2fw\n"
"0IjKOtEXc/VfmtTFch5+AfGYMGMqqvJ6LcXiAhqG5TI+Dr0RtM88k+8XUBCeQ8IG\n"
"KuANaL7TiItKZYxK1MMuTJtV9IblAgMBAAGjggE7MIIBNzASBgNVHRMBAf8ECDAG\n"
"AQH/AgEAMA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUWaRmBlKge5WSPKOUByeW\n"
"dFv5PdAwHwYDVR0jBBgwFoAUhBjMhTTsvAyUlC4IWZzHshBOCggwewYIKwYBBQUH\n"
"AQEEbzBtMC8GCCsGAQUFBzABhiNodHRwOi8vb2NzcC5yb290Y2ExLmFtYXpvbnRy\n"
"dXN0LmNvbTA6BggrBgEFBQcwAoYuaHR0cDovL2NybC5yb290Y2ExLmFtYXpvbnRy\n"
"dXN0LmNvbS9yb290Y2ExLmNlcjA/BgNVHR8EODA2MDSgMqAwhi5odHRwOi8vY3Js\n"
"LnJvb3RjYTEuYW1hem9udHJ1c3QuY29tL3Jvb3RjYTEuY3JsMBMGA1UdIAQMMAow\n"
"CAYGZ4EMAQIBMA0GCSqGSIb3DQEBCwUAA4IBAQAfsaEKwn17DjAbi/Die0etn+PE\n"
"gfY/I6s8NLWkxGAOUfW2o+vVowNARRVjaIGdrhAfeWHkZI6q2pI0x/IJYmymmcWa\n"
"ZaW/2R7DvQDtxCkFkVaxUeHvENm6IyqVhf6Q5oN12kDSrJozzx7I7tHjhBK7V5Xo\n"
"TyS4NU4EhSyzGgj2x6axDd1hHRjblEpJ80LoiXlmUDzputBXyO5mkcrplcVvlIJi\n"
"WmKjrDn2zzKxDX5nwvkskpIjYlJcrQu4iCX1/YwZ1yNqF9LryjlilphHCACiHbhI\n"
"RnGfN8j8KLDVmWyTYMk8V+6j0LI4+4zFh2upqGMQHL3VFVFWBek6vCDWhB/b\n"
"-----END CERTIFICATE-----\n"
"-----BEGIN CERTIFICATE-----\n"
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
"rqXRfboQnoZsG4q5WTP468SQvvG5\n"
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

void scan_demo(WiFiInterface *wifi)
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

void http_demo(NetworkInterface *net)
{
    // Open a socket on the network interface, and create a TCP connection to mbed.org
    TCPSocket socket;
    socket.open(net);

    SocketAddress tmp_IP_addr_Buff;
    net->gethostbyname("ifconfig.io", &tmp_IP_addr_Buff);
    tmp_IP_addr_Buff.set_port(80);
    socket.connect(tmp_IP_addr_Buff);
    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: ifconfig.io\r\n\r\n";
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\n", scount, strstr(sbuffer, "\r\n") - sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n") - rbuffer, rbuffer);

    // Close the socket to return its memory and bring down the network interface
    socket.close();
}

int main()
{
    mbed_trace_init();
    nsapi_size_or_error_t result;
    SocketAddress a; //buffer to store IP address information

    printf("WiFi example\r\n\r\n");

    scan_demo(&wifi);

    printf("\r\nConnecting...\r\n");
    int ret = wifi.connect("IoT_Test", "MQTT_TLS_TEST_2022", NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\r\nConnection error\r\n");
        return -1;
    }

    printf("Success\r\n\r\n");
    printf("MAC: %s\r\n", wifi.get_mac_address());
    wifi.get_ip_address(&a);
    printf("IP: %s\r\n", a.get_ip_address());
    wifi.get_netmask(&a);
    printf("Netmask: %s\r\n", a.get_ip_address());
    wifi.get_gateway(&a);
    printf("Gateway: %s\r\n", a.get_ip_address());
    printf("RSSI: %d\r\n\r\n", wifi.get_rssi());

    //http_demo(&wifi);
    ThisThread::sleep_for(10s);
    //wifi.disconnect();
    
    printf("\r\nDone\r\n");
    TLSSocket *socket = new TLSSocket;
    result = socket->set_root_ca_cert(cert);
    if (result != 0) {
        printf("Error: socket->set_root_ca_cert() returned %d\n", result);
        return result;
    }

    result = socket->open(wifi.wifiInterface());
    if (result != 0) {
        printf("Error! socket->open() returned: %d\n", result);
        return result;
    }
    SocketAddress addr;
    wifi.gethostbyname("os.mbed.com", &addr);
    addr.set_port(443);
    
    printf("Connecting to os.bed.com on port 443\n");
    result = socket->connect(addr);
    if(result != 0) {
        printf("Error! socket->connect() returned: %d\n", result);
        result = socket->connect(addr);
        return result;
    }
    char sbuffer[] = "GET / HTTPS/1.1\r\nHost: os.mbed.com\r\n\r\n";
    int scount = socket->send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\n", scount, strstr(sbuffer, "\r\n") - sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket->recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n") - rbuffer, rbuffer);
}


