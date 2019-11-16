#ifndef NEOM8N_NEOM8N_H
#define NEOM8N_NEOM8N_H

#include <string>
#include <functional>
#include <map>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <cstdio>
#include <unistd.h>

typedef std::function<void(char *data)> GPSCallback;

class NeoM8N {
public:
    NeoM8N(const std::string& device);
    ~NeoM8N();
    void RegisterCallback(const std::string& key, GPSCallback cb);
    void UnregisterCallback(const std::string& key);
    void Capture();
private:
    int fd;
    std::map<std::string, GPSCallback> cbs;
    struct termios oldPortSettings{}, newPortSettings{};
    bool capture;
};

#endif //NEOM8N_NEOM8N_H