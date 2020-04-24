#pragma once

///TODO:
///+ On-the-fly re-sync to bar beat start.
///+ A better link between play button / params in both internal / external modes with one unique play button.
///+ Add filter to smooth / stabilize BPM number when using external midi clock mode.

#include "ofMain.h"

#include "ofxGuiExtended2.h"
#include "ofxMidi.h"
#include "ofxMidiClock.h"//used for external midi clock sync
#include "ofxMidiTimecode.h"
#include "ofxDawMetro.h"//used for internal clock

#define USE_ofxAbletonLink
#ifdef USE_ofxAbletonLink
#include "ofxAbletonLink.h"//used for external Ableton Live LINK engine compati
#endif

///WIP: alternative and better timer approach using the audio - buffer to avoid out - of - sync problems of current timers
///(https://forum.openframeworks.cc/t/audio-programming-basics/34392/10). Problems happen when minimizing or moving the app window.. Any help is welcome!
///audioBuffer alternative timer mode
///(code is at the bottom)
///un-comment to enable this NOT WORKING yet alternative mode
///THE PROBLEM: clock drift very often.. maybe in wasapi sound api?
///help on improve this is welcome!
//#define USE_AUDIO_BUFFER_TIMER_MODE

#define BPM_INIT 120

//only to long song mode on external midi sync vs simpler 4 bars / 16 beats
#define ENABLE_PATTERN_LIMITING//comment to disable
#define PATTERN_STEP_BAR_LIMIT 4
#define PATTERN_STEP_BEAT_LIMIT 16

//TODO:
//smooth global bpm clock
//could be only visual refreshing the midi clock slower or using a real filter.
//#define BPM_MIDI_CLOCK_REFRESH_RATE 1000
////refresh received MTC by clock. disabled/commented to "realtime" by every-frame-update

//-

class ofxBeatClock : public ofxMidiListener, public ofxDawMetro::MetroListener {

#ifdef USE_ofxAbletonLink
private:
	ofxAbletonLink link;

	ofParameter<bool> ENABLE_LINK_SYNC;

	//TODO:
	//beat callback to LINK?...beat in LINK is a float so it's changing on every frame...
	//link.beat
	//use Changed_MIDI_beatsInBar(int &beatsInBar)

	void LINK_bpmChanged(double &bpm) {
		ofLogNotice("ofxBeatClock") << "LINK_bpmChanged" << bpm;

		BPM_Global = (float)bpm;
		bpm_ClockInternal = (float)bpm;

	}

	void LINK_numPeersChanged(std::size_t &peers) {
		ofLogNotice("ofxBeatClock") << "LINK_numPeersChanged" << peers;
	}

	void LINK_playStateChanged(bool &state) {
		ofLogNotice("ofxBeatClock") << "LINK_playStateChanged" << (state ? "play" : "stop");

		PLAYING_State = state;
	}

	void LINK_setup() {
		link.setup();

		//ofAddListener(link.bpmChanged, this, &ofxBeatClock::LINK_bpmChanged);
		//ofAddListener(link.numPeersChanged, this, &ofxBeatClock::LINK_numPeersChanged);
		//ofAddListener(link.playStateChanged, this, &ofxBeatClock::LINK_playStateChanged);
	}

	void LINK_update();

	void LINK_draw() {
		ofPushStyle();

		int xpos = 250;
		int ypos = 500;

		float x = ofGetWidth() * link.getPhase() / link.getQuantum();
		x += 300;//move to right

		//red vertical line
		ofSetColor(255, 0, 0);
		ofDrawLine(x, 0, x, ofGetHeight());

		std::stringstream ss("");
		ss
			<< "bpm:   " << link.getBPM() << std::endl
			<< "beat:  " << link.getBeat() << std::endl
			<< "phase: " << link.getPhase() << std::endl
			<< "peers: " << link.getNumPeers() << std::endl
			<< "play?: " << (link.isPlaying() ? "play" : "stop");

		ofSetColor(255);
		if (fontMedium.isLoaded())
		{
			fontMedium.drawString(ss.str(), 20, 20);
		}
		else
		{
			ofDrawBitmapString(ss.str(), 20, 20);
		}

		ofPopStyle();
	}

	//TODO:
	//should add some control into the gui too. 
	//maybe creating a LINK dedicated transport bar
	//control
public:
	void LINK_keyPressed(int key) {
		if (key == OF_KEY_LEFT) {
			if (20 < link.getBPM()) link.setBPM(link.getBPM() - 1.0);
		}
		else if (key == OF_KEY_RIGHT) {
			link.setBPM(link.getBPM() + 1.0);
		}
		else if (key == 'b') {
			link.setBeat(0.0);
		}
		else if (key == 'B') {
			link.setBeatForce(0.0);
		}
		else if (key == ' ') {
			link.setIsPlaying(!link.isPlaying());
		}
	}
#endif

	//-

#pragma mark - OF_MAIN

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

	bool bMidiClockRunning; //< is the clock sync running?
	unsigned int MIDI_beats; //< song pos in beats
	double MIDI_seconds; //< song pos in seconds, computed from beats
	double MIDI_CLOCK_bpm; //< song tempo in bpm, computed from clock length
	int MIDI_quarters; //convert total # beats to # quarters
	int MIDI_bars; //compute # of bars

	//-

	ofParameter<int> MIDI_beatsInBar;//compute remainder as # TARGET_NOTES_params within the current bar
	void Changed_MIDI_beatsInBar(int & beatsInBar);//only used in midiIn clock sync 
	int beatsInBar_PRE;//not required

	//-

#pragma mark - EXTERNAL_CLOCK

	void setup_MIDI_IN_Clock();

	//-

#pragma mark - LAYOUT

private:
	bool DEBUG_moreInfo = false;//more debug

	//TODO:
	////layout
	//ofParameter<glm::vec2> position_BeatBoxes;
	//ofParameter<glm::vec2> position_BeatBall;
	//ofParameter<glm::vec2> position_ClockInfo;
	//ofParameter<glm::vec2> position_BpmInfo;

	glm::vec2 pos_Global;//main anchor to reference all other gui elements
	glm::vec2 pos_ClockInfo;
	glm::vec2 pos_BpmInfo;
	int pos_BeatBoxes_x, pos_BeatBoxes_y, pos_BeatBoxes_width;
	int pos_BeatBall_x, pos_BeatBall_y, pos_BeatBall_radius;

public:
	//api setters
	void setPosition_GuiExtra(int x, int y);//global position setter with default layout of the other elements
	void setPosition_BeatBoxes(int x, int y, int w);//position x, y and w = width of all 4 squares
	void setPosition_BeatBall(int x, int y, int w);//position x, y and w = width of ball
	void setPosition_ClockInfo(int _x, int _y);
	void setPosition_BpmInfo(int _x, int _y);
	//void setPosition_Gui_ALL(int _x, int _y, int _w);//TODO:

	//beat boxes
	void draw_BeatBoxes(int x, int y, int w);

	//beat tick ball
	void draw_BeatBall(int x, int y, int radius);

	//beat tick ball
	void draw_ClockInfo(int x, int y);

	//beat tick ball
	void draw_BpmInfo(int x, int y);

	//big clock
	void draw_BigClockTime(int x, int y);

	//--

	//debug helpers

	//red anchor to debug mark
	bool DEBUG_Layout = false;
	void draw_Anchor(int x, int y)
	{
		if (DEBUG_Layout)
		{
			ofPushStyle();
			ofFill();
			ofSetColor(ofColor::red);
			ofDrawCircle(x, y, 3);
			int pad;
			if (y < 15) pad = 20;
			else pad = -20;
			ofDrawBitmapStringHighlight(ofToString(x) + "," + ofToString(y), x, y + pad);
			ofPopStyle();
		}
	}
	void setDebug(bool b)
	{
		DEBUG_Layout = b;
		DEBUG_moreInfo = DEBUG_Layout;
	}
	void toggleDebugMode()
	{
		DEBUG_Layout = !DEBUG_Layout;
		DEBUG_moreInfo = DEBUG_Layout;
	}

	//--

private:
	//main text color
	ofColor colorText;

	//-

#pragma mark - MONITOR_VISUAL_FEEDBACK

private:
	//beat ball
	ofPoint circlePos;
	float fadeOut_animTime, fadeOut_animCounter;
	bool fadeOut_animRunning;
	float dt = 1.0f / 60.f;

	//main receiver
	void beatTick_MONITOR(int beat);///trigs sound and gui drawing ball visual feedback

public:
	ofParameter<bool> BeatTick_TRIG;

	//-

////TODO:
//private:
//smooth clock for midi input clock sync
//#pragma mark - REFRESH_FEQUENCY
////used only when BPM_MIDI_CLOCK_REFRESH_RATE is defined
//unsigned long BPM_LAST_Tick_Time_LAST;//test
//unsigned long BPM_LAST_Tick_Time_ELLAPSED;//test
//unsigned long BPM_LAST_Tick_Time_ELLAPSED_PRE;//test
//long ELLAPSED_diff;//test
//unsigned long bpm_CheckUpdated_lastTime;

	//-

#pragma mark - GUI

public:
	void setup_Gui();
	void refresh_Gui();
	ofxGui gui;

private:
	ofxGuiGroup2* group_BEAT_CLOCK;//nested folder
	ofxGuiGroup2* group_Controls;
	ofxGuiGroup2* group_BpmTarget;
	ofxGuiGroup2* group_INTERNAL;
	ofxGuiGroup2* group_EXTERNAL;
	ofParameterGroup params_INTERNAL;
	ofParameterGroup params_EXTERNAL;
	ofJson confg_Button, confg_Sliders;//json theme

	//-

#pragma mark - PARAMS
private:
	ofParameterGroup params_CONTROL;
	ofParameter<bool> PLAYING_State;//player state
	ofParameter<bool> ENABLE_CLOCKS;//enable clock
	ofParameter<bool> ENABLE_INTERNAL_CLOCK;//enable internal clock
	ofParameter<bool> ENABLE_EXTERNAL_CLOCK;//enable midi clock sync
	ofParameter<int> MIDI_Port_SELECT;
	int num_MIDI_Ports = 0;

	ofParameterGroup params_BpmTarget;

	//----

public:
	//this is the final target bpm, is the destinations of all other clocks (internal, external midi sync or ableton link)
	ofParameter<float> BPM_Global;//global tempo bpm.
	ofParameter<int> BPM_GLOBAL_TimeBar;//ms time of 1 bar = 4 beats

	//----

private:
	ofParameter<bool> BPM_Tap_Tempo_TRIG;//trig the measurements of tap tempo

	//helpers to modify current bpm
	ofParameter<bool> RESET_BPM_Global;
	ofParameter<bool> BPM_half_TRIG;//divide bpm by 2
	ofParameter<bool> BPM_double_TRIG;//multiply bpm by 2

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
	float getBPM();
	int getTimeBar();

//private:
//	bool bBallAutoPos = true;
//
//public:
//	void setPosition_BeatBall_Auto(bool b)
//	{
//		bBallAutoPos = b;
//	}

	//-

private:
	void Changed_Params(ofAbstractParameter& e);

	//-

	//internal clock

	//based on threaded timer using ofxDawMetro
#pragma mark - INTERNAL_CLOCK

	ofxDawMetro clockInternal;

	//callbacks defined inside the addon class. can't be renamed here
	//overide ofxDawMetro::MetroListener's method if necessary
	void onBarEvent(int & bar) override;
	void onBeatEvent(int & beat) override;
	void onSixteenthEvent(int & sixteenth) override;

	ofParameterGroup params_ClockInternal;
	ofParameter<float> bpm_ClockInternal;
	ofParameter<bool> clockInternal_Active;
	void Changed_ClockInternal_Bpm(float & value);
	void Changed_ClockInternal_Active(bool & value);
	//ofxGuiContainer* container_daw;

	//TODO:
	void reSync();
	ofParameter<bool> bSync_Trig;

public:
	void setBpm_ClockInternal(float bpm);//to set bpm from outside

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

#pragma mark - DRAW_STUFF:

	//FONT
	string messageInfo;
	ofTrueTypeFont fontSmall;
	ofTrueTypeFont fontMedium;
	ofTrueTypeFont fontBig;

	//-

	//BEAT BALL
	ofPoint metronome_ball_pos;
	int metronome_ball_radius;

	//-

	ofParameter<bool> SHOW_Extra;//beat boxes, text info and beat ball

	//-

#pragma mark - SOUND
	//sound metronome
	ofParameter<bool> ENABLE_sound;//enable sound ticks
	ofParameter<float> volumeSound;//sound ticks volume
	ofSoundPlayer tic;
	ofSoundPlayer tac;
	ofSoundPlayer tapBell;

	//-

#pragma mark - CURRENT_BPM_CLOCK_VALUES
public:
	void reset_clockValuesAndStop();

	//TODO: could be nice to add some listener system..
	ofParameter<int> Bar_current;
	ofParameter<int> Beat_current;
	ofParameter<int> Tick_16th_current;

	//TODO: LINK
#ifdef USE_ofxAbletonLink
	int Beat_current_PRE;
	float Beat_float_current;
	string Beat_float_string;
#endif

private:

	//strings for monitor drawing
	//1:1:1
	string Bar_string;
	string Beat_string;
	string Tick_16th_string;

	string clockActive_Type;//internal/external/link clock types name
	string clockActive_Info;//midi in port, and extra info for any clock source

	//----

#pragma mark - API
	
	//----

public:

	//transport control
	void start();
	void stop();
	void togglePlay();

	//----

	//layout
	void setPosition_GuiPanel(int x, int y, int w);//gui panel
	ofPoint getPosition_GuiPanel();
	void setVisible_GuiPanel(bool b);
	void setVisible_BeatBall(bool b);

	bool get_Gui_visible()
	{
		return gui.getVisible();
	}
	void toggle_Gui_visible()
	{
		bool b = get_Gui_visible();
		setVisible_GuiPanel(!b);
	}

	bool isPlaying()
	{
		return bIsPlaying;
	}

	//NOTE: take care with the path font defined on the config json 
	//because ofxGuiExtended crashes if fonts are not located on /data
	void loadTheme(string s)
	{
		group_BEAT_CLOCK->loadTheme(s);
		group_Controls->loadTheme(s);
		group_BpmTarget->loadTheme(s);
		group_INTERNAL->loadTheme(s);
		group_EXTERNAL->loadTheme(s);
	}

private:
	int gui_Panel_W, gui_Panel_posX, gui_Panel_posY, gui_Panel_padW;

	bool bIsPlaying;

	//-

#pragma mark - TAP_ENGINE

public:
	void tap_Trig();
	void tap_Update();

private:
	float tap_BPM;
	int tap_Count, tap_LastTime, tap_AvgBarMillis;
	vector<int> tap_Intervals;
	bool bTap_running;
	bool SOUND_wasDisabled = false;//sound disbler to better user workflow

	//-

#pragma mark - CHANGE_MIDI_PORT

	int midiIn_CLOCK_port_OPENED;
	void setup_MidiIn_Port(int p);
	int MIDI_Port_PRE = -1;
	ofParameter <string> midiPortName{ "","" };

	//-

#pragma mark - STEP LIMITING

	//we don't need to use long song patterns
	//and we will limit bars to 4 like a simple step sequencer.
	bool ENABLE_pattern_limits;
	int pattern_BEAT_limit;
	int pattern_BAR_limit;

	//--

	//string myTTF;//gui font path
	//int sizeTTF;

	//----

	//TODO:
	//audioBuffer alternative timer mode to get a a more accurate clock
	//based on:
	//https://forum.openframeworks.cc/t/audio-programming-basics/34392/10?u=moebiussurfing
	//by davidspry:
	//"the way I�m generating the clock is naive and simple.I�m simply counting the number of samples written to the buffer and sending a notification each time the number of samples is equal to one subdivided �beat�, as in BPM.
	//Presumably there�s some inconsistency because the rate of writing samples to the buffer is faster than the sample rate, but it seems fairly steady with a small buffer size."

	//we can maybe use the same soundStream used for the sound tick also for the audiBuffer timer..
	//maybe will be a better solution to put this timer into ofxDawMetro class!!

#ifdef USE_AUDIO_BUFFER_TIMER_MODE
private:
	void setupAudioBuffer(int _device);
	void closeAudioBuffer();
	ofParameter<bool> MODE_AudioBufferTimer;
	ofSoundStream soundStream;
	int deviceOut;
	int samples = 0;
	int ticksPerBeat = 4;
	//default is 4. 
	//is this a kind of resolution if we set bigger like 16?
	int samplesPerTick;
	int sampleRate;
	int bufferSize;
	int DEBUG_ticks = 0;
	bool DEBUG_bufferAudio = false;
public:
	void audioOut(ofSoundBuffer &buffer);
#endif

	//----
};

