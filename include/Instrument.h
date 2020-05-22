//
// Created by wooliii on 5/19/2020.
//

#ifndef MIDI_FILE_PLAYER_INSTRUMENT_H
#define MIDI_FILE_PLAYER_INSTRUMENT_H

#include<string>
#include<vector>
#include "RtMidi.h"

class Instrument {
public:
    Instrument(const Instrument& copy)
    : name_{copy.name_}, channel_{copy.channel_}, midiout_{copy.midiout_} {}

    Instrument(std::shared_ptr<RtMidiOut>& midiout)
    : name_{"Instrument_0"}, channel_{0}, midiout_{midiout} {}

    Instrument(std::shared_ptr<RtMidiOut>& midiout, unsigned int channel)
    : name_{"Instrument_"+ std::to_string(channel)}, channel_{channel}, midiout_{midiout} {}

    void setName(std::string name) {name_ = name;}
    void setChannel(unsigned int channel) {channel_ = channel;}
    void setRtMidiOut(std::shared_ptr<RtMidiOut>& midiout) {midiout_ = midiout;}

    void sendMessage(unsigned int command, unsigned int arg1, unsigned int arg2);
    void sendNoteOn(unsigned int note, unsigned int velocity);
    void sendNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendNoteOff(unsigned int note, unsigned int velocity);
    void sendNoteOff(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendControlChange(unsigned int ctrl_number, unsigned int ctrl_value);
    void sendControlChange(unsigned int channel, unsigned int ctrl_number, unsigned int ctrl_value);
    void sendProgramChange(unsigned int program);
    void sendProgramChange(unsigned int channel, unsigned int program);

private:
    std::string name_;
    unsigned int channel_;
    std::shared_ptr<RtMidiOut> midiout_;
};


#endif //MIDI_FILE_PLAYER_INSTRUMENT_H
