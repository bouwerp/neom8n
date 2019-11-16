#include <cstring>
#include "neom8n.h"

using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::string;
using std::exception;

NeoM8N::NeoM8N(const std::string& device) {
    capture = false;
    /*
      Open modem device for reading and writing and not as controlling tty
      because we don't want to get killed if linenoise sends CTRL-C.
    */
    fd = open(device.c_str(), O_RDWR | O_NOCTTY );
    if (fd <0) {perror(device.c_str()); exit(-1); }
    tcgetattr(fd,&oldPortSettings); /* save current serial port settings */
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
    newPortSettings.c_cc[VINTR]    = 0;     /* Ctrl-c */
    newPortSettings.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newPortSettings.c_cc[VERASE]   = 0;     /* del */
    newPortSettings.c_cc[VKILL]    = 0;     /* @ */
    newPortSettings.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newPortSettings.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newPortSettings.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    newPortSettings.c_cc[VSWTC]    = 0;     /* '\0' */
    newPortSettings.c_cc[VSTART]   = 0;     /* Ctrl-q */
    newPortSettings.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newPortSettings.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newPortSettings.c_cc[VEOL]     = 0;     /* '\0' */
    newPortSettings.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newPortSettings.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newPortSettings.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newPortSettings.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newPortSettings.c_cc[VEOL2]    = 0;     /* '\0' */

    /*
      now clean the modem line and activate the settings for the port
    */
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW,&newPortSettings);
}

void NeoM8N::RegisterCallback(const std::string& key, GPSCallback cb) {
    cbs.insert_or_assign(key, cb);
}

void NeoM8N::UnregisterCallback(const std::string& key) {
    cbs.erase(key);
}

void NeoM8N::Capture() {
    int res;
    char buf[4096];
    while (true) {
        if (!capture) {
            return;
        }
        res = read(fd, buf, 4096);
        if (res == -1) {
            if  (errno == EAGAIN) {
                sleep(1);
                continue;
            } else {
                clog<<"ERROR: "<<strerror(errno)<<endl;
                return;
            }
        }
        /* set end of string, so we can printf */
        buf[res]=0;
        for (auto const& v : cbs) {
            v.second(buf);
        }
    }
}

NeoM8N::~NeoM8N() {
    /* stop capturing */
    capture = false;
    /* restore the old port settings */
    tcsetattr(fd, TCSANOW, &oldPortSettings);
    /* close the port */
    close(fd);
}
