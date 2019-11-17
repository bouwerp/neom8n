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
#include <regex>

using std::string;

typedef std::function<void(char *data)> GPSCallback;

#define CHECKSUM_REGEX "[$](.*)[*]([0-9A-Fa-f]+)$"
#define TYPE_REGEX "[$][A-Z]{2}([A-Z]{3}).*[*][0-9A-Fa-f]+$"
#define GGA_REGEX "[$]([A-Z]{2})GGA,([0-9]{2}[0-9]{2}[0-9]{2}[.][0-9]+)*,([0-9]+[.][0-9]+)*,([NS])*,([0-9]+[.][0-9]+)*,([EW])*,([0126])*,([0-9]{2}),([0-9]+[.][0-9]+)*,([0-9]+[.][0-9]+)*,M*,([0-9]+[.][0-9]+)*,(.*),(.*)[*][0-9A-Fa-f]+$"
#define VTG_REGEX "[$]([A-Z]{2})VTG,([0-9]+[.][0-9]+)*,T*,(.*),M*,([0-9]+[.][0-9]+)*,N*,([0-9]+[.][0-9]+)*,K*,([NEAD])*[*][0-9A-Fa-f]+$"
#define GSV_REGEX "[$]([A-Z]{2})GSV,([0-9]+)*,([0-9]+)*,([0-9]+)*((?:,[0-9]+,[0-9]*,[0-9]*,[0-9]*){0,4})(?:,([0-9]*))*[*][0-9A-Fa-f]+$"
#define GSV_SATELLITE_DATA_REGEX ",([0-9]+)*,([0-9]+)*,([0-9]+)*,([0-9]*)*"
#define GLL_REGEX "[$]([A-Z]{2})GLL,([0-9]+[.][0-9]+)*,([NS])*,([0-9]+[.][0-9]+)*,([EW])*,([0-9]{2}[0-9]{2}[0-9]{2}[.][0-9]+)*,([AV])*,([NEAD])*[*][0-9A-Fa-f]+$"
#define ZDA_REGEX "[$]([A-Z]{2})ZDA,([0-9]{2}[0-9]{2}[0-9]{2}[.][0-9]+)*,([0-9]{2})*,([0-9]{2})*,([0-9]{4})*,([-]*[0-9]{2})*,([0-9]{2})*[*][0-9A-Fa-f]+$"
#define TXT_REGEX "[$]([A-Z]{2})TXT,([0-9][0-9])*,([0-9][0-9])*,([0-9][0-9])*,(.*)[*][0-9A-Fa-f]+$"
#define RMC_REGEX "[$]([A-Z]{2})RMC,([0-9]{2}[0-9]{2}[0-9]{2}[.][0-9]+)*,([VA]),([0-9]+[.][0-9]+)*,([NS])*,([0-9]+[.][0-9]+)*,([EW])*,([0-9]+[.][0-9]+)*,([0-9]+[.][0-9]+)*,([0-9]{2}[0-9]{2}[0-9]{2})*,,,([NEAD])*(?:,([AV]))*[*][0-9A-Fa-f]+$"
#define GSA_REGEX "[$]([A-Z]{2})GSA,([MA])*,([123])*((?:,[0-9]*){0,12}),([0-9]+[.][0-9]+)*,([0-9]+[.][0-9]+)*,([0-9]+[.][0-9]+)*(?:,([0-9]+))*[*][0-9A-Fa-f]+$"
#define GSA_SATELLITE_IDS_REGEX ",([0-9]+)*"

class InvalidSentenceTypeError : public std::exception {
    virtual const char *what() const noexcept override;
};

class NoMatchingSentenceTypeError : public std::exception {
    virtual const char *what() const noexcept override;
};

enum SentenceType {
    GGA_TYPE = 0,
    VTG_TYPE,
    GSV_TYPE,
    GLL_TYPE,
    ZDA_TYPE,
    TXT_TYPE,
    RMC_TYPE,
    GSA_TYPE
};

string SentenceTypeToString(SentenceType t);

SentenceType StringToSentenceType(string s);

class InvalidSentenceError : public std::exception {
    virtual const char *what() const noexcept override;
};

class GGA {
public:
    GGA(const string &s);
    SentenceType Type;
    string Talker;
    string Time;
    double Latitude;
    string NorthSouthIndicator;
    double Longitude;
    string EastWestIndicator;
    string QualityIndicator;
    int NumberOfSatellitesUsed;
    double HDOP;
    double Altitude;
    double GeoIDSeparation;
    int64_t DifferentialAge;
    int DifferentialStationID;
};

class SatelliteInfo {
public:
    SatelliteInfo(const string &s);

    // satellite ID
    int SatelliteID;
    // elecation (0-90 degrees)
    int Elevation;
    // azimuth (0-359 degrees)
    int Azimuth;
    // signal stength (0-99 dBH)
    int SignalStrength;
};

class GSV {
public:
    GSV(const string &s);
    SentenceType Type;
    string Talker;
    int NumberOfMessages;
    int MessageNumber;
    int NumberOfSatellites;
    int SignalID;
    SatelliteInfo SatelliteInfos[];
};

class NeoM8N {
public:
    NeoM8N(const std::string &device);

    ~NeoM8N();

    void RegisterCallback(const std::string &key, GPSCallback cb);

    void UnregisterCallback(const std::string &key);

    void Read();

private:
    int fd;
    std::map<std::string, GPSCallback> cbs;
    struct termios oldPortSettings{}, newPortSettings{};
    bool reading;
};


#endif //NEOM8N_NEOM8N_H