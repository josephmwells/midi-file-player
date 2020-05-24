cmake_minimum_required(VERSION 3.16)
project(midi_file_player)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-D__WINDOWS_MM__)

include_directories(include/)

add_executable(midiplayer
        main.cpp
        src/RtMidi.cpp
        src/Binasc.cpp
        src/MidiEvent.cpp
        src/MidiEventList.cpp
        src/MidiFile.cpp
        src/MidiMessage.cpp
        src/Instrument.cpp
        src/swti.cpp)

target_link_libraries(midiplayer winmm.lib)
