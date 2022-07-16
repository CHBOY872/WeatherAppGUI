#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include "Client.hpp"
extern "C"
{
#include "cJSON.h"
}

static char request_start[] = "GET /data/2.5/weather?q=";
// city Kyiv
static char request_mid[] = "&APPID=";
// appid 54e798c771c8670785e640216be615e5
static char request_end[] = "&units=metric HTTP/1.0\r\n\r\n";

void WeatherInfo::Clear()
{
    int city_len = strlen(city);
    bzero(city, city_len);

    int main_info_len = strlen(main_info);
    bzero(main_info, main_info_len);

    int additional_info_len = strlen(additional_info);
    bzero(additional_info, additional_info_len);
}

void WeatherInfo::Show()
{
    printf("MAIN -> %s\n", main_info);
    printf("ADD INFO -> %s\n", additional_info);
    printf("TEMP -> %f\n", temperature);
    printf("PRESSURE -> %d\n", pressure);     // ATMOSPHERIC PRESSURE
    printf("HUMIDITY -> %d ", humidity);      // PERCENTAGE
    printf("WIND SPEED -> %f\n", wind_speed); // METER/SEC
    printf("CLOUDS -> %d", clouds);           // PERCENTAGE
    printf("CITY -> %s\n", city);
}

/////////////////////////////

Client::Client(int _port, char *_api_key, int _api_key_len, char *_ip)
    : fd(-1), port(_port), ip(_ip), api_key(_api_key),
      api_key_len(_api_key_len), buf(""), buf_used(0)
{
}

void Client::WriteRequest(const char *msg)
{
    write(fd, msg, strlen(msg));
}

bool Client::Connect()
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
        return false;

    struct sockaddr_in addr_server;
    addr_server.sin_addr.s_addr = inet_addr(ip);
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port);

    int stat =
        connect(fd, (struct sockaddr *)&addr_server, sizeof(addr_server));
    if (stat == -1)
        return false;
    return true;
}

Client *Client::Start(char *_ip, int port, char *_api)
{
    return new Client(port, _api, strlen(_api), _ip);
}

bool Client::HandleRequest(WeatherInfo *to)
{
    to->Clear();
    char *ptr = strstr(buf, "{");
    cJSON *monitor_parse = cJSON_Parse(ptr);

    cJSON *object = 0;

    cJSON *value = cJSON_GetObjectItemCaseSensitive(monitor_parse, "weather");
    if (!value)
    {
        cJSON_Delete(monitor_parse);
        return false;
    }

    cJSON_ArrayForEach(object, value)
    {
        strcpy(to->main_info,
               cJSON_GetObjectItemCaseSensitive(object, "main")->valuestring);
        strcpy(to->additional_info,
               cJSON_GetObjectItemCaseSensitive(object, "description")->valuestring);
    }
    object = cJSON_GetObjectItemCaseSensitive(monitor_parse, "main");
    to->temperature =
        cJSON_GetObjectItemCaseSensitive(object, "temp")->valuedouble;
    to->pressure =
        cJSON_GetObjectItemCaseSensitive(object, "pressure")->valueint;
    to->humidity =
        cJSON_GetObjectItemCaseSensitive(object, "humidity")->valueint;
    object = cJSON_GetObjectItemCaseSensitive(monitor_parse, "wind");
    to->wind_speed =
        cJSON_GetObjectItemCaseSensitive(object, "speed")->valuedouble;

    object = cJSON_GetObjectItemCaseSensitive(monitor_parse, "clouds");
    to->clouds =
        cJSON_GetObjectItemCaseSensitive(object, "all")->valueint;
    strcpy(to->city,
           cJSON_GetObjectItemCaseSensitive(monitor_parse, "name")->valuestring);

    cJSON_Delete(monitor_parse);
    return true;
}

bool Client::Request(const char *city, WeatherInfo *to)
{
    if (!Connect())
        return false;

    int city_len = strlen(city);
    int char_len = city_len + api_key_len + sizeof(request_start) +
                   sizeof(request_mid) + sizeof(request_end) - 2;
    char *msg = new char[char_len];
    sprintf(msg, "%s%s%s%s%s",
            request_start, city, request_mid, api_key, request_end);
    WriteRequest(msg);
    delete[] msg;
    int rc = read(fd, buf, buffer - buf_used);
    if (rc <= 0)
        return false;

    bool res = true;
    if (!HandleRequest(to))
        res = false;

    bzero(buf, buf_used + rc);
    buf_used = 0;
    return res;
}