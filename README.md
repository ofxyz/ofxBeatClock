# ofxBeatClock

**openFrameworks** add-on to run a *DAW-Styled* BPM Beat-Clock with **Internal Clock**, **Tap Tempo** and **External MIDI Sync** and **Ableton Link**.

<!-- 
## Screencast

![gif](/readme_images/ofxBeatClock.gif?raw=true "gif") -->

## Screenshots

![image](/readme_images/Capture1.PNG?raw=true "image")

<!-- 1 - Internal Clock mode:  
![image](/readme_images/Capture1.PNG?raw=true "image")

2 - External MidiClock mode:  
![image](/readme_images/Capture2.PNG?raw=true "image")

3 - Ableton Link mode:  
![image](/readme_images/Capture3.PNG?raw=true "image") -->


## Usage

Create the example project or your own with **OF PROJECT GENERATOR**` as usual.  
Take care of required add-ons and the included **/bin/data/** files.  

### ofApp.h
```cpp
#include "ofxBeatClock.h"

ofxBeatClock beatClock;
ofEventListener listenerBeat;
void Changed_BeatTick();
```

### ofApp.cpp
```cpp
void ofApp::setup()
{
  beatClock.setup();
  listenerBeat = beatClock.BeatTick_TRIG.newListener([&](bool&) {this->Changed_BeatTick(); });
}
void ofApp::Changed_BeatTick() // callback to receive BeatTicks
{
  if (beatClock.BeatTick_TRIG) ofLogNotice(__FUNCTION__) << "BeatTick ! #" << beatClock.Beat_current;
}
```

## Features

* **NEW FEATURE**:  
**Ableton LINK** sync engine (master/slave).  
  **_[WIP but almost working]_**  
  Link engine is (maybe) disabled by default. To enable Link:
  * Add ```ofxAbletonLink``` add-on to your app project. 
  * Uncomment ```#define USE_ofxAbletonLink``` on ```ofxBeatClock.h```. 
**ImGUi** based **GUI**.  
* **Internal Clock** based in a threaded timer from **ofxDawMetro** from **@castovoid**.  
You can uncomment `#define USE_AUDIO_BUFFER_TIMER_MODE` on `ofxBeatClock.h` to enable *BETA* alternative timer. (*WIP*)
* **External Clock Source** as **input MIDI Clock** (*Slave*) using **ofxMidi** from **@danomatika**.  
Easy to Sync to **Ableton Live** or any sequencer app with **Midi Clock**.
* **Tap Tempo Engine**.
* **Auto Save/Load** of all settings.
* Nice **Metronome Sound Ticks**.

## Requeriments

* [ofxMidi](https://github.com/danomatika/ofxMidi)  
* [ofxAbletonLink](https://github.com/2bbb/ofxAbletonLink) [optional]  
* [ofxImGuiSurfing](https://github.com/moebiussurfing/ofxImGuiSurfing)
* [ofxImGui](https://github.com/Daandelange/ofxImGui/) FORK from @**Daandelange**  
* [ofxScaleDragRect](https://github.com/moebiussurfing/ofxScaleDragRect)
(Allready included into ```OF_add-on/libs```. No need to add manually!)  
* [ofxDawMetro](https://github.com/castovoid/ofxDawMetro)  

## Tested Systems
- **Windows10** / **VS2017** / **OF ~0.11**

## Author
add-on by **@moebiusSurfing**  
*(ManuMolina). 2020.*

**_Thanks to the developers of the included core add-ons!  
@danomatika, @2bb and @castovoid._**

## License
*MIT License.*

**_PLEASE FEEL FREE TO ADD MODIFICATIONS OR FEATURES AND TO SEND ME PULL REQUESTS OR ISSUES!_**
