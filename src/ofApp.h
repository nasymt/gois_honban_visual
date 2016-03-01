#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "particle_ripples.hpp"
#include "particle_bg.hpp"
#include "ofxPostGlitch.h"
#include "Ripple.hpp"
#include "FFTWave.hpp"
#include "SimpleLineEffects.hpp"
#include "FFTGrid.hpp"

#define S_PORT 9000
#define HOST "localhost"

#define R_PORT 12000
#define NUM_MSG_STRINGS 20
#define WIDTH 1280
#define HEIGHT 800

/*----FFT-----*/
#define BUFFER_SIZE 1024
#define AUDIO_INDEX 10

/*-----VJ Class----*/
#define BG_NUM 300
#define RIP_NUM 2000
#define RIP_PERIOD 250
#define CHAIR_NUM 16
#define RIPPLE_NUM 5
#define FFT_WAVE_NUM  256
#define LIGHT_NUM 20
#define FFT_GRID_NUM 100






class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void audioReceived(float *buf,int bufSize,int nChan);
    void sceneChange();
    void visualEffect();
    void oscEvent();
    
    ofFbo myFbo;
    ofTrueTypeFont font;
    ofxPostGlitch myGlitch;
    ofxOscReceiver receiver;
    ParticleRipples myRipples[RIP_NUM];
    particle_bg myBgParticle[BG_NUM];
    Ripple fftRipple[RIPPLE_NUM];
    SimpleLineEffects simpleLineEffects[FFT_WAVE_NUM];
    FFTGrid fftGrid[FFT_GRID_NUM];
    FFTWave fftWave[FFT_WAVE_NUM];
    
    int ripple_index;
    int mode,chairCondition[CHAIR_NUM];
    int nowTime,targetTime;
    bool bAutoChange;
    bool bManualGlitch;
    int ambientBangTime;
    int count;
    
    /*-------OSC--------*/
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    int beat;
    float A_Vol;
    
    bool bSLITSCAN,bNOISE;
    int effectTime;
    
    
    /*--------FFT----------*/
    float * fft;
    int nBandsToGet;
    ofSoundStream soundStream;
    float pre_buf,pre_val;
    float buffer[BUFFER_SIZE];
    
    /*-------FFTWave-------*/
    int lightCount;
    int lightIndex;
    bool bLight[LIGHT_NUM];
    
    /*----DEBUG-----*/
    ofxOscSender sender;
    bool bAutoAudio;
    
    
    
};
