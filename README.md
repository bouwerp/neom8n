Neo M8N GPS Receiver - C++ Library
==================================

This library provides the ability to stream NMEA sentences from the Neo M8N
GPS receiver over a serial (UART) interface. It also provides parsing of
the sentences into useful structures.

Currently, parsing has been implemented for the following sentence types:
* GGA (summarised position info)
* GSV (GPS satellite info)

Although this library should technically work on any POSIX operating system, 
it has only been tested on a Raspberry Pi, running Raspbian OS.

# Usage

The class is constructed with the serial device as the only argument.
Callbacks can then be registered to handle the data. The actual streaming to
the callbacks happen when the blocking `Read` method is called. This can
be done in a separate thread.

```cpp
...
neom8n::NeoM8N neoM8N("/dev/ttySC0");
// one or more callbacks can be registered to handle the sentences
neoM8N.RegisterCallback("process_data", [&](char *data) {
    std::string s(data);
    try {
        auto type = neom8n::GetSentenceType(s);
        switch (type) {
            case neom8n::GGA_TYPE: {
                auto gga = neom8n::GGA(s);
                cout << "Lat: " << gga.Latitude << gga.NorthSouthIndicator << endl;
                cout << "Lon: " << gga.Longitude << gga.EastWestIndicator << endl;
                break;
            }
            case neom8n::GSV_TYPE: {
                auto gsv = neom8n::GSV(s);
                cout << "Message #" << gsv.MessageNumber << " of " << gsv.NumberOfMessages << endl;
                cout << "Number of satellites: " << gsv.NumberOfSatellites << endl;
                break;
            }
            default: {
                cerr << "type '" << neom8n::SentenceTypeToString(type) << "' not supported yet" << endl;
                break;
            }
        }
    } catch (const exception &e) {
        cerr << "could not determine sentence type: " << e.what() << endl;
        cerr << "SENTENCE: " << s << endl;
    }
});

// execute the blocking read function in a separate thread
std::thread gpsThread([](neom8n::NeoM8N *neoM8N) -> void {
    neoM8N->Read();
}, &neoM8N);
```
# Running the unit tests

The [Catch2](https://github.com/catchorg/Catch2) unit testing framework is utilised in
this library. The header file is included as part of the source.

The test executable must first be compiled:
```cpp
cmake -DCMAKE_BUILD_TYPE=Debug ./
make -j 16
``` 

Then to run the test:
```cpp
./neom8n_test
```

# Building the library

Assuming the library will be cross compiled for another architecture:
```c++
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=<PATH_TO_TOOLCHAIN_FILE> --target neom8n ../
make -j 16
```