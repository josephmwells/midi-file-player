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
#include "swti.hpp"
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
#include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

void list_midi_out_devices(std::shared_ptr<RtMidiOut>& midiout);
void print_message_type(const MidiEvent& event);

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "usage:\n"
        << "midiplayer option inputfile.mid\n"
        << "options:\n"
        << " -l list midi devices\n"
        << " -p [port #] midi device out number\n"
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

    // Set up for our loop
    std::vector<int> event(tracks);
    std::vector<bool> eot(tracks); // end of track
    std::vector<std::string> track_name(tracks);
    std::chrono::time_point<std::chrono::system_clock> start, current;
    start = std::chrono::system_clock::now();

    // Set the window size to fit all of our info
    Window.setSizeChars(75 + (127 - 21) * 2, 5 + tracks); // x = header size + notes size

    // Begin printing header info and track info
    int cursor_offset = 0; // This is just for spacing and formatting
    Cursor.setPosition( 0, 0 + cursor_offset);
    Cursor.setColor(YELLOW);
    std::cout << "TRACK NAME: ";
    Cursor.setColor(BLUE);
    std::cout << infile;
    Cursor.setColor(YELLOW);
    std::cout << " OUTPUT PORT: ";
    Cursor.setColor(BLUE);
    std::cout << midiout->getPortName(midiout_port);

    Cursor.setPosition(0, 2);
    Cursor.setColor(YELLOW);
    std::cout << "TICK\tTRACK#\tNAME\t\tTYPE\tCMD\tARG1\tARG2\tCHANNEL\t || \n";
    Cursor.setColor(BLUE);
    cursor_offset = 3;

    bool eof = false; // end of file
    while(!eof) {
        // Check to see if all tracks have finished
        eof = true;
        for(int track = 0; track < tracks; ++track) {
            eof = eof && eot[track];
        }

        // Check elapsed time
        current = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = current - start;

        // If there is an unprocessed event and its time to process it
        // Run all of this stuff
        for(int track = 0; track < tracks; ++track) {
            if (event[track] < midifile[track].size()
            && midifile[track][event[track]].seconds <= elapsed_seconds.count()) {
                Cursor.setPosition(0, track + cursor_offset);
                Cursor.setColor((Color)((track % 15) + 1));
                std::cout << midifile[track][event[track]].tick << '\t';
                std::cout << "TRACK" << track << '\t';

                // Get track name and print it properly
                if(midifile[track][event[track]].isTrackName()) {
                    track_name[track] = midifile[track][event[track]].getMetaContent();
                    if(track_name[track].length() > 15)
                        track_name[track] = track_name[track].substr(0, 15);
                }
                std::cout << track_name[track];
                if(track_name[track].length() < 8) std::cout << "\t\t";
                else if(track_name[track].length() < 16) std::cout << '\t';

                // Print the message type
                print_message_type(midifile[track][event[track]]);

                // The way RTMidi works is we build a message of unsigned chars
                // and push unsigned ints into. This creates a packet of bytes
                // that will then be sent to the chosen midiout port
                std::vector<unsigned char> message;

                // A midi message is made up of two parts, the command bits
                // and the parameter bits
                unsigned int command = midifile[track][event[track]][0];
                std::cout << command << '\t';
                message.push_back(command);

                // A midi message can have one or two parameters
                // Most messages have two parameters unless it is a
                // Program Change message in which case their is only one
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

                std::cout << midifile[track][event[track]].getChannel() << '\t';
                std::cout << " || ";

                // Print note names
                if(midifile[track][event[track]].isNoteOn()) {
                    // Setting the cursor position requires some math to make enough room for
                    // two characters per note name
                    Cursor.setPosition(Cursor.getX() + ((arg1-21) * 2), track + cursor_offset);
                    std::string note_letter = "AaBCcDdEFfGg";
                    std::cout << note_letter[(arg1-21) % 12] << (int)(((arg1-21)/12)+1);
                } else if(midifile[track][event[track]].isNoteOff()) {
                    Cursor.setPosition(Cursor.getX() + ((arg1-21) * 2), track + cursor_offset);
                    std::cout << "  ";
                }

                // Send message
                try {
                    midiout->sendMessage(&message);
                } catch (RtMidiError &error) {
                    error.printMessage();
                    exit(EXIT_FAILURE);
                }

                // Check if command is end of track "EOT"
                if(midifile[track][event[track]].isEndOfTrack()) eot[track] = true;

                ++event[track];
                Cursor.setPosition(0, tracks+cursor_offset);
            }
        }
    }
    Cursor.setPosition(0, tracks + cursor_offset);
    std::cout << "End of File\n";
    Cursor.setColor(WHITE);
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

void print_notes(const MidiEvent& event) {
    if(event.isNoteOn()) {

    }

}