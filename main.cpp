#include<iostream>
#include<string>
#include<vector>
#include<chrono>
#include "RtMidi.h"
#include "MidiFile.h"
#include "Instrument.h"

using namespace smf;
//using namespace std;

#if defined(WIN32)
#include <windows.h>

#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
#include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

void list_midi_out_devices(std::shared_ptr<RtMidiOut>& midiout);
void print_message_type(const MidiEvent& event);

int main(int argc, char* argv[]) {

    if(argc < 3) {
        std::cout << "usage:\n"
        << "midiplayer option inputfile.mid\n"
        << "options:\n"
        << " -l list midi devices\n"
        << " -p midi device out number\n"
        << std::endl;

        return 0;
    }

    std::shared_ptr<RtMidiOut> midiout;
    try {
        midiout = std::make_shared<RtMidiOut>();
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

    // Process command line options and arguments
    unsigned int midiout_port = 0;
    std::string infile(argv[argc - 1]);
    for(int i = 0; i < argc; ++i) {
        std::string arg = std::string(argv[i]);
        if(arg == "-l") list_midi_out_devices(midiout);
        if(arg == "-p") midiout_port = std::atoi(argv[i+1]);
    }

    // enable midi output on the given port
    try {
        midiout->openPort(midiout_port);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

    // open and read the midi file
    MidiFile midifile;
    if(!midifile.read(infile)) {
        std::cout << "error: error reading midi file" << std::endl;
        exit(EXIT_FAILURE);
    }
    midifile.doTimeAnalysis();
    midifile.linkNotePairs();
    int tracks = midifile.getTrackCount();

    std::vector<int> event(tracks);
    std::vector<bool> eot(tracks); // end of track

    std::chrono::time_point<std::chrono::system_clock> start, current;
    start = std::chrono::system_clock::now();

    std::cout << "TICK\tTRACK#\tTYPE\tCMD\tARG1\tARG2\tCHANNEL\n";

    bool eof = false; // end of file
    while(!eof) {
        // Check to see if all tracks have finished
        eof = true;
        for(int track = 0; track < tracks; ++track) {
            eof = eof && eot[track];
        }

        current = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = current - start;

        for(int track = 0; track < tracks; ++track) {
            if (event[track] < midifile[track].size()
            && midifile[track][event[track]].seconds <= elapsed_seconds.count()) {
                std::cout << midifile[track][event[track]].tick << '\t';
                std::cout << "TRACK" << track << '\t';
                print_message_type(midifile[track][event[track]]);

                std::vector<unsigned char> message;

                unsigned int command = midifile[track][event[track]][0];
                std::cout << command << '\t';
                message.push_back(command);

                unsigned int arg1 = midifile[track][event[track]][1];
                std::cout << arg1 << '\t';
                message.push_back(arg1);

                // If command is not a program change
                // Push the second argument
                if(!midifile[track][event[track]].isPatchChange()) {
                    unsigned int arg2 = midifile[track][event[track]][2];
                    std::cout << arg2 << '\t';
                    message.push_back(arg2);
                } else {
                    std::cout << "--\t";
                }

                std::cout << midifile[track][event[track]].getChannel() << '\n';

                try {
                    midiout->sendMessage(&message);
                } catch (RtMidiError &error) {
                    error.printMessage();
                    exit(EXIT_FAILURE);
                }

                // Check if command is end of track
                if(midifile[track][event[track]].isEndOfTrack()) eot[track] = true;

                ++event[track];
            }
        }
    }
    std::cout << "End of File\n";
    return 0;
}

void list_midi_out_devices(std::shared_ptr<RtMidiOut>& midiout) {
    //Check outputs
    unsigned int nPorts = midiout->getPortCount();
    std::cout << "There are " << nPorts << " MIDI output ports available.\n";
    std::string portName;
    for (unsigned int i = 0; i < nPorts; ++i) {
        try {
            portName = midiout->getPortName(i);
        } catch (RtMidiError &error) {
            error.printMessage();
            exit(EXIT_FAILURE);
        }
        std::cout << "Output port #" << i << ": " << portName << '\n';
    }
    std::cout << '\n';
}

void print_message_type(const MidiEvent& event) {
    if(event.isNoteOn()) std::cout << "NOTEON\t";
    if(event.isNoteOff()) std::cout << "NOTEOFF\t";

    if(event.isAftertouch()) std::cout << "AFTRTCH\t";
    else if(event.isPressure()) std::cout << "PRSSR\t";
    else if(event.isPitchbend()) std::cout << "PTCHBND\t";
    else if(event.isSustainOn()) std::cout << "SSTNON\t";
    else if(event.isSustainOff()) std::cout << "SSTNOFF\t";
    else if(event.isController()) std::cout << "CTRLR\t";

    if(event.isPatchChange()) std::cout << "PRGRM\t";
    else if(event.isTimbre()) std::cout << "TIMBRE\t";

    if(event.isEndOfTrack()) std::cout << "EOT\t";
    else if(event.isMeta()) std::cout << "META\t";
}
