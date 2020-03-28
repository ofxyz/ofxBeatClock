#pragma once

///TODO:
///+ Add alternative and better timer approach using the audio - buffer to avoid out - of - sync problems of current timers
///(https://forum.openframeworks.cc/t/audio-programming-basics/34392/10). Problems happen when minimizing or moving the app window.. Any help is welcome!
///+ On - the - fly re - sync to bar beat start.
///+ A better link between play button / params in both internal / external modes with one unique play button.
///+ Add filter to smooth / stabilize BPM number when using external midi clock mode.
///NOTE: Sorry, I am not sure why I am using more than one BPM vars... Maybe one of them is from midi clock, other from local daw timer, and other as the global and this is the finaly used, also the one to be smoothed..

#include "ofMain.h"

#include "ofxGuiExtended2.h"
#include "ofxMidi.h"
#include "ofxMidiClock.h"
#include "ofxMidiTimecode.h"
#include "ofxDawMetro.h"

#define BPM_INIT 120
#define ENABLE_PATTERN_LIMITING//comment to disable: to long song mode
#define PATTERN_STEP_BAR_LIMIT 4
#define PATTERN_STEP_BEAT_LIMIT 16

//TODO:
//smooth clock
//#define BPM_MIDI_CLOCK_REFRESH_RATE 1000
////refresh received MTC by clock. disabled/commented to "realtime" by every-frame-update

class ofxBeatClock : public ofxMidiListener, public ofxDawMetro::MetroListener {

private:

	//-

//	//TODO:
//	//audio - buffer alternative mode
//	ofParameter<bool> MODE_BufferTimer;
//	ofSoundStream soundStream;
//public:
//	void audioOut(ofSoundBuffer &buffer);
//private:
//	int samples = 0;
//	int bpm = 120;
//	int ticksPerBeat = 4;
//	int samplesPerTick = (44100 * 60.0f) / bpm / ticksPerBeat;

	//-

public:
	void setup();
	void update();
	void draw();
	void exit();

	//-

#pragma mark - MIDI_IN_CLOCK

private:
	ofxMidiIn midiIn_CLOCK;
	ofxMidiMessage midiCLOCK_Message;
	ofxMidiClock MIDI_clock; //< clock message parser

	void newMidiMessage(ofxMidiMessage& eventArgs);

	bool clockRunning; //< is the clock sync running?
	unsigned int MIDI_beats; //< song pos in beats
	double MIDI_seconds; //< song pos in seconds, computed from beats
	double MIDI_CLOCK_bpm; //< song tempo in bpm, computed from clock length
	int MIDI_quarters; //convert total # beats to # quarters
	int MIDI_bars; //compute # of bars

	//-

	ofParameter<int> MIDI_beatsInBar;//compute remainder as # TARGET_NOTES_params within the current bar
	void Changed_MIDI_beatsInBar(int & beatsInBar);
	int beatsInBar_PRE;//not required

	//-

#pragma mark - EXTERNAL_CLOCK

	void setup_MIDI_CLOCK();

	//-

#pragma mark - MONITOR

private:
	int pos_BeatBoxes_x, pos_BeatBoxes_y, pos_BeatBoxes_w;
	int pos_BeatBall_x, pos_BeatBall_y, pos_BeatBall_w;

public:
	void setPosition_BeatBoxes(int x, int y, int w);
	void setPosition_BeatBall(int x, int y, int w);
	void setPosition_Gui_ALL(int _x, int _y, int _w);

	//beat boxes
	void drawBeatBoxes(int x, int y, int w);

	//beat tick ball
	void draw_BeatBall(int x, int y, int w);

private:
	//beat ball
	ofPoint circlePos;
	float animTime, animCounter;
	bool animRunning;
	float dt = 1.0f / 60.f;

	//main receiver
	void CLOCK_Tick_MONITOR(int beat);

public:
	ofParameter<bool> TRIG_TICK;

	//-

//	//TODO:
//private:
//smooth clock
//#pragma mark - REFRESH_FEQUENCY
//	//used only when BPM_MIDI_CLOCK_REFRESH_RATE is defined
//	unsigned long BPM_LAST_Tick_Time_LAST;//test
//	unsigned long BPM_LAST_Tick_Time_ELLAPSED;//test
//	unsigned long BPM_LAST_Tick_Time_ELLAPSED_PRE;//test
//	long ELLAPSED_diff;//test
//
//	unsigned long bpm_CheckUpdated_lastTime;

	//-

#pragma mark - GUI

public:
	void setup_Gui();
	ofxGui gui;

private:
	ofxGuiGroup2* group_BEAT_CLOCK;//nested folder
	ofxGuiGroup2* group_Controls;
	ofxGuiGroup2* group_BpmTarget;
	ofxGuiGroup2* group_INTERNAL;
	ofxGuiGroup2* group_EXTERNAL;
	ofParameterGroup params_INTERNAL;
	ofParameterGroup params_EXTERNAL;
	//json theme
	ofJson confg_Button, confg_Sliders;

	//-

#pragma mark - PARAMS
private:
	ofParameterGroup params_CONTROL;
	ofParameter<bool> PLAYER_state;//player state
	ofParameter<bool> ENABLE_CLOCKS;//enable clock
	ofParameter<bool> ENABLE_INTERNAL_CLOCK;//enable internal clock
	ofParameter<bool> ENABLE_EXTERNAL_CLOCK;//enable midi clock sync
	ofParameter<int> MIDI_Port_SELECT;
	int num_MIDI_Ports = 0;

	ofParameterGroup params_BpmTarget;

public:
	ofParameter<int> BPM_GLOBAL_TimeBar;//ms time of 1 bar = 4 beats
	ofParameter<float> BPM_Global;//tempo bpm global

private:
	ofParameter<bool> BPM_Tap_Tempo_TRIG;//trig measurements of tap tempo

	ofParameter<bool> RESET_BPM_Global;
	ofParameter<bool> BPM_half_TRIG;
	ofParameter<bool> BPM_double_TRIG;

	//-

	//API

public:
	bool getInternalClockModeState()
	{
		return ENABLE_INTERNAL_CLOCK;
	}
	bool getExternalClockModeState()
	{
		return ENABLE_EXTERNAL_CLOCK;
	}
	float get_BPM();
	int get_TimeBar();

private:
	bool bBallAutoPos = true;
public:
	void setPosition_BeatBall_Auto(bool b)
	{
		bBallAutoPos = b;
	}

	//-

private:
	void Changed_Params(ofAbstractParameter& e);

	//-

	//internal clock
#pragma mark - DAW METRO
	void reSync();
	ofParameter<bool> bSync_Trig;

	//overide ofxDawMetro::MetroListener's method if necessary
	void onBarEvent(int & bar) override;
	void onBeatEvent(int & beat) override;
	void onSixteenthEvent(int & sixteenth) override;
	ofxDawMetro metro;

	ofParameterGroup params_daw;
	ofParameter<float> DAW_bpm;
	ofParameter<bool> DAW_active;
	void Changed_DAW_bpm(float & value);
	void Changed_DAW_active(bool & value);
	//ofxGuiContainer* container_daw;

public:
	void set_DAW_bpm(float bpm);//to set bpm from outside

	//-

	//settings
#pragma mark - XML SETTINGS
private:
	void saveSettings(string path);
	void loadSettings(string path);
	string pathSettings;
	string filenameControl = "BeatClock_Settings.xml";
	string filenameMidiPort = "MidiInputPort_Settings.xml";

	//-

#pragma mark - DRAW STUFF:

	//FONT

	string TTF_message;
	ofTrueTypeFont TTF_small;
	ofTrueTypeFont TTF_medium;
	ofTrueTypeFont TTF_big;

	//-

	//BEAT BALL

	ofPoint metronome_ball_pos;
	int metronome_ball_radius;

	//-

	ofParameter<bool> SHOW_Extra;//beat boxes and beat ball

	//-

#pragma mark - SOUND

	ofParameter<bool> ENABLE_sound;//enable sound ticks
	ofSoundPlayer tic;
	ofSoundPlayer tac;
	ofSoundPlayer tapBell;

	//-

#pragma mark - CURRENT BPM CLOCK VALUES
public:
	void RESET_clockValues();

	//TODO: could be nice to add listener system..

	ofParameter<int> BPM_bar_current;
	ofParameter<int> BPM_beat_current;
	ofParameter<int> BPM_16th_current;

private:
	//STRINGS FOR MONITOR DRAWING

	//1:1:1
	string BPM_bar_str;
	string BPM_beat_str;
	string BPM_16th_str;

	string BPM_input_str;//internal/external
	string BPM_name_str;//midi in port

	//-

#pragma mark - API

public:
	void draw_BigClockTime(int x, int y);

	void PLAYER_START();
	void PLAYER_STOP();
	void PLAYER_TOGGLE();

	//layout
	void setPosition_Gui(int x, int y, int w);
	ofPoint getPosition_Gui();
	void set_Gui_visible(bool b);
	void set_BeatBall_visible(bool b);

	bool get_Gui_visible()
	{
		return gui.getVisible();
	}
	void toggle_Gui_visible()
	{
		bool b = get_Gui_visible();
		set_Gui_visible(!b);
	}

	bool isPlaying()
	{
		return bIsPlaying;
	}

private:
	int gui_Panel_W, gui_Panel_posX, gui_Panel_posY, gui_Panel_padW;

	bool bIsPlaying;

	//-

#pragma mark - TAP_ENGINE

public:
	void Tap_Trig();
	void Tap_update();

private:
	int tapCount, lastTime, avgBarMillis;
	float Tap_BPM;
	vector<int> tapIntervals;
	bool bTap_running;
	bool SOUND_wasDisabled = false;//sound disbler to better flow

	//-

#pragma mark - CHANGE MIDI PORT

	int midiIn_CLOCK_port_OPENED;
	void setup_MIDI_PORT(int p);
	int MIDI_Port_PRE = -1;

	ofParameter <string> midiPortName{ "","" };

	//-

#pragma mark - STEP LIMITING

	//we don't need to use long song patterns
	bool ENABLE_pattern_limits;
	int pattern_BEAT_limit;
	int pattern_BAR_limit;

	//--

	string myTTF;//gui font path
	int sizeTTF;

	//-
};

