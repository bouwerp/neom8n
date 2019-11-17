#include <cstring>
#include "neom8n.h"

using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::string;
using std::exception;

namespace neom8n {
    NeoM8N::NeoM8N(const std::string &device) {
        reading = false;
        /*
          Open modem device for reading and writing and not as controlling tty
          because we don't want to get killed if linenoise sends CTRL-C.
        */
        fd = open(device.c_str(), O_RDWR | O_NOCTTY);
        if (fd < 0) {
            perror(device.c_str());
            exit(-1);
        }
        tcgetattr(fd, &oldPortSettings); /* save current serial port settings */
        bzero(&newPortSettings, sizeof(newPortSettings)); /* clear struct for new port settings */

        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, O_NONBLOCK);

        /*
           BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
           CRTSCTS : output hardware flow control (only used if the cable has
                     all necessary lines. See sect. 7 of Serial-HOWTO)
           CS8     : 8n1 (8bit,no parity,1 stopbit)
           CLOCAL  : local connection, no modem contol
           CREAD   : enable receiving characters
         */
        newPortSettings.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;

        /*
          IGNPAR  : ignore bytes with parity errors
          ICRNL   : map CR to NL (otherwise a CR input on the other computer
                    will not terminate input)
          otherwise make device raw (no other input processing)
        */
        newPortSettings.c_iflag = IGNPAR | ICRNL;

        /*
         Raw output.
        */
        newPortSettings.c_oflag = 0;

        /*
          ICANON  : enable canonical input
          disable all echo functionality, and don't send signals to calling program
        */
        newPortSettings.c_lflag = ICANON;

        /*
          initialize all control characters
          default values can be found in /usr/include/termios.h, and are given
          in the comments, but we don't need them here
        */
        newPortSettings.c_cc[VINTR] = 0;     /* Ctrl-c */
        newPortSettings.c_cc[VQUIT] = 0;     /* Ctrl-\ */
        newPortSettings.c_cc[VERASE] = 0;     /* del */
        newPortSettings.c_cc[VKILL] = 0;     /* @ */
        newPortSettings.c_cc[VEOF] = 4;     /* Ctrl-d */
        newPortSettings.c_cc[VTIME] = 0;     /* inter-character timer unused */
        newPortSettings.c_cc[VMIN] = 1;     /* blocking read until 1 character arrives */
        newPortSettings.c_cc[/*VSWTC*/7] = 0;     /* '\0' */
        newPortSettings.c_cc[VSTART] = 0;     /* Ctrl-q */
        newPortSettings.c_cc[VSTOP] = 0;     /* Ctrl-s */
        newPortSettings.c_cc[VSUSP] = 0;     /* Ctrl-z */
        newPortSettings.c_cc[VEOL] = 0;     /* '\0' */
        newPortSettings.c_cc[VREPRINT] = 0;     /* Ctrl-r */
        newPortSettings.c_cc[VDISCARD] = 0;     /* Ctrl-u */
        newPortSettings.c_cc[VWERASE] = 0;     /* Ctrl-w */
        newPortSettings.c_cc[VLNEXT] = 0;     /* Ctrl-v */
        newPortSettings.c_cc[VEOL2] = 0;     /* '\0' */

        /*
          now clean the modem line and activate the settings for the port
        */
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd, TCSANOW, &newPortSettings);
    }

    void NeoM8N::RegisterCallback(const std::string &key, GPSCallback cb) {
        cbs.insert_or_assign(key, cb);
    }

    void NeoM8N::UnregisterCallback(const std::string &key) {
        cbs.erase(key);
    }

    void NeoM8N::Read() {
        int res;
        char buf[4096];
        reading = true;
        while (true) {
            if (!reading) {
                return;
            }
            memset(buf, 0, sizeof(buf));
            res = read(fd, buf, 4096);
            if (res == -1) {
                if (errno == EAGAIN) {
                    sleep(1);
                    continue;
                } else {
                    clog << "ERROR: " << strerror(errno) << endl;
                    return;
                }
            }
            if (res == 0) {
                continue;
            }
            if (buf[0] != '$') {
                continue;
            }
            /* set end of string, so we can printf */
            buf[res] = 0;
            for (auto const &v : cbs) {
                v.second(buf);
            }
        }
    }

    NeoM8N::~NeoM8N() {
        /* stop capturing */
        reading = false;
        /* restore the old port settings */
        tcsetattr(fd, TCSANOW, &oldPortSettings);
        /* close the port */
        close(fd);
    }

    std::string &ltrim(std::string &str, const std::string &chars = "\t\n\v\f\r ") {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    std::string &rtrim(std::string &str, const std::string &chars = "\t\n\v\f\r ") {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    std::string &trim(std::string &str, const std::string &chars = "\t\n\v\f\r ") {
        return ltrim(rtrim(str, chars), chars);
    }

    string getMatch(string m) {
        if (string(m).empty()) throw InvalidSentenceError();
        return m;
    }

    SatelliteInfo::SatelliteInfo(const string &s) {

    }

    GSV::GSV(const string &s) {

    }

    GGA::GGA(const string &sentence) {
        std::regex r(GGA_REGEX);
        std::smatch match;
        auto s = sentence;
        trim(s);
        if (std::regex_search(s, match, r)) {
            if (match.size() != 14) {
                throw InvalidSentenceError();
            }
            Type = GGA_TYPE;
            Talker = getMatch(match[1]);
            Time = getMatch(match[2]);
            Latitude = stod(getMatch(match[3]));
            NorthSouthIndicator = getMatch(match[4]);
            Longitude = stod(getMatch(match[5]));
            EastWestIndicator = getMatch(match[6]);
            QualityIndicator = getMatch(match[7]);
            NumberOfSatellitesUsed = stoi(getMatch(match[8]));
            HDOP = stod(getMatch(match[9]));
            Altitude = stod(getMatch(match[10]));
            GeoIDSeparation = stod(getMatch(match[11]));
        }
    }

    const char *InvalidSentenceError::what() const noexcept {
        return "the provided sentence has an invalid format for the specified type";
    }

    const char *InvalidSentenceTypeError::what() const noexcept {
        return "invalid sentence type - must be one of: GGA(0), VTG(1), GSV(2), GLL(3), ZDA(3), TXT(5), RMC(6), GSA(7)";
    }

    const char *NoMatchingSentenceTypeError::what() const noexcept {
        return "no matching sentence type for the string provided";
    }

    string SentenceTypeToString(SentenceType t) {
        switch (t) {
            case GGA_TYPE:
                return "GGA";
            case VTG_TYPE:
                return "VTG";
            case GSV_TYPE:
                return "GSV";
            case GLL_TYPE:
                return "GLL";
            case ZDA_TYPE:
                return "ZDA";
            case TXT_TYPE:
                return "TXT";
            case RMC_TYPE:
                return "RMC";
            case GSA_TYPE:
                return "GSA";
            default:
                throw InvalidSentenceTypeError();
        }
    }

    SentenceType StringToSentenceType(const string &s) {
        if (s == "GGA") {
            return GGA_TYPE;
        } else if (s == "VTG") {
            return VTG_TYPE;
        } else if (s == "GSV") {
            return GSV_TYPE;
        } else if (s == "GLL") {
            return GLL_TYPE;
        } else if (s == "ZDA") {
            return ZDA_TYPE;
        } else if (s == "TXT") {
            return TXT_TYPE;
        } else if (s == "RMC") {
            return RMC_TYPE;
        } else if (s == "GSA") {
            return GSA_TYPE;
        }
        throw NoMatchingSentenceTypeError();
    }

    SentenceType GetSentenceType(const string &sentence) {
        std::regex r(TYPE_REGEX);
        std::smatch match;
        auto s = sentence;
        trim(s);
        if (std::regex_search(s, match, r)) {
            if (match.size() != 2) {
                throw InvalidSentenceError();
            }
        }
        return StringToSentenceType(getMatch(match[1]));
    }
}
