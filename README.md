# midi-file-player
## Description
A cross-platform command line midi-file player. Built using [midifile](https://github.com/craigsapp/midifile) and [rtmidi](https://github.com/thestk/rtmidi).

## Compiling
Requires `cmake`

Use the included `CMakeLists.txt` and follow the directions for your OS.

Edit `CMakeLists.txt` and compile with preprocessor definitions found [here](https://www.music.mcgill.ca/~gary/rtmidi/index.html#compiling)

### Unix
make build directory
`mkdir build`

enter build direcotry and run cmake
```
cd build 
cmake ..
```

### Windows
Use the `cmake` gui for Windows

## Usage
```
usage: midiplayer option inputfile.mid
options:
-l list midi out devices
-p midi device out number
```
