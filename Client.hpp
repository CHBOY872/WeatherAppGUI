#ifndef CLIENT_HPP_SENTER
#define CLIENT_HPP_SENTER

enum
{
    buffer = 2048,
};

class Client;

class WeatherInfo
{
    friend class Client;
    char main_info[buffer];
    char additional_info[buffer];
    float temperature; // IN CELSIUM
    int pressure;      // ATMOSPHERIC PRESSURE
    int humidity;      // PERCENTAGE
    float wind_speed;  // METER/SEC
    int clouds;        // PERCENTAGE
    char city[buffer];

public:
    WeatherInfo()
    {
        *(main_info) = 0;
        *(additional_info) = 0;
        *(city) = 0;
    }
    void Show();

    const char *GetMainInfo() { return main_info; }
    const char *GetAdditionalInfo() { return additional_info; }
    float GetTemperature() { return temperature; }
    int GetPressure() { return pressure; }
    int GetHumidity() { return humidity; }
    float GetWindSpeed() { return wind_speed; }
    int GetClouds() { return clouds; }
    const char *GetCity() { return city; }

private:
    void Clear();
};

class Client
{
    int fd;
    int port;
    char *ip;
    char *api_key;
    int api_key_len;
    char buf[buffer];
    int buf_used;
    Client(int _port, char *_api_key, int _api_key_len, char *_ip);

public:
    static Client *Start(char *_ip, int port, char *_api);
    ~Client() {}

    bool Request(const char *city, WeatherInfo *to);

private:
    bool Connect();
    void WriteRequest(const char *msg);
    bool HandleRequest(WeatherInfo *to);
};

#endif