//
// Created by wooliii on 5/19/2020.
//

#include "Instrument.h"

void Instrument::sendNoteOn(unsigned int note, unsigned int velocity) {
    std::vector<unsigned char> message;
    message.push_back(144 + channel_);
    message.push_back(note);
    message.push_back(velocity);
    try {
        midiout_->sendMessage(&message);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
}

void Instrument::sendNoteOn(unsigned int channel, unsigned int note, unsigned int velocity) {
    std::vector<unsigned char> message;
    message.push_back(144 + channel);
    message.push_back(note);
    message.push_back(velocity);
    try {
        midiout_->sendMessage(&message);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
}

void Instrument::sendNoteOff(unsigned int note, unsigned int velocity) {
    std::vector<unsigned char> message;
    message.push_back(128 + channel_);
    message.push_back(note);
    message.push_back(velocity);
    try {
        midiout_->sendMessage(&message);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
}

void Instrument::sendNoteOff(unsigned int channel, unsigned int note, unsigned int velocity) {
    std::vector<unsigned char> message;
    message.push_back(128 + channel);
    message.push_back(note);
    message.push_back(velocity);
    try {
        midiout_->sendMessage(&message);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
}

void Instrument::sendControlChange(unsigned int ctrl_number, unsigned int ctrl_value) {
    std::vector<unsigned char> message;
    message.push_back(176 + channel_);
    message.push_back(ctrl_number);
    message.push_back(ctrl_value);
    try {
        midiout_->sendMessage(&message);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
}

void Instrument::sendControlChange(unsigned int channel, unsigned int ctrl_number, unsigned int ctrl_value) {
    std::vector<unsigned char> message;
    message.push_back(176 + channel);
    message.push_back(ctrl_number);
    message.push_back(ctrl_value);
    try {
        midiout_->sendMessage(&message);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
}

void Instrument::sendProgramChange(unsigned int program) {
    std::vector<unsigned char> message;
    message.push_back(192 + channel_);
    message.push_back(program);
    try {
        midiout_->sendMessage(&message);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
}

void Instrument::sendProgramChange(unsigned int channel, unsigned int program) {
    std::vector<unsigned char> message;
    message.push_back(192 + channel);
    message.push_back(program);
    try {
        midiout_->sendMessage(&message);
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
}

void Instrument::sendMessage(unsigned int command, unsigned int arg1, unsigned int arg2 = 0) {

}
