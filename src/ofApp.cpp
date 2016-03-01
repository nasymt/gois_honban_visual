#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    //    sender.setup(HOST,S_PORT);
    receiver.setup(R_PORT);
    //    ofEnableAlphaBlending();
    //    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    targetTime=ofGetElapsedTimeMillis()+10000;
    ambientBangTime=ofGetElapsedTimeMillis()+120000;
    bAutoChange=true;
    /*---------FFT-----------------------*/
    nBandsToGet=1024;
    ofSoundStreamSetup(0, 2, 44100, BUFFER_SIZE, 4);
    /*---------FBO/PostGlitch------------*/
    myFbo.allocate(WIDTH,HEIGHT);
    myGlitch.setup(&myFbo);
    /*---DEBUG----*/
    sender.setup(HOST,S_PORT);
    for(int i=0;i<FFT_GRID_NUM;i++){
     fftGrid[i].setup();
    }

}


//--------------------------------------------------------------
void ofApp::update(){
    nowTime=ofGetElapsedTimeMillis();
    if(bAutoChange)sceneChange();
    /*---------------FBO-------------------*/
    myFbo.begin();
    ofClear(0, 0, 0,255);
    
    if(mode==1){
        ofSetCircleResolution(3);//宇宙背景
        for(int i=0;i<BG_NUM;i++){
            myBgParticle[i].update(i);
            myBgParticle[i].draw();
            if(myBgParticle[i].pos.z>700)myBgParticle[i].reset();
        }
        ofSetCircleResolution(20);
        for(int i=0;i<RIP_NUM;i++){
            for(int j=0;j<8;j++){
                if(i>=j*RIP_PERIOD&&i<j*RIP_PERIOD+RIP_PERIOD)ripple_index=j+1;
                if(myRipples[i].pos.z>600)myRipples[i].reset(i);
            }
            myRipples[i].update(ripple_index,i);
            myRipples[i].draw();
        }
    }
    else if(mode==2){
        for(int i=0;i<RIPPLE_NUM;i++){
            fftRipple[i].update(buffer);
            fftRipple[i].draw();
        }
        for(int i=0;i<FFT_WAVE_NUM;i++){
            fftWave[i].update(buffer,i);
            fftWave[i].draw(i);
        }
        
      /*  for(int i=0;i<LIGHT_NUM;i++){
            //            if(bLight[i])cout<<"TRUE::"<<i<<":"<<bLight[i]<<endl;
            fftWave[i].light(i,bLight[i]);
            if(bLight[i])bLight[i]=false;
        }
        
        if(lightCount<50){
            lightCount++;
        }else {
            bLight[lightIndex]=true;
            lightCount=0;
            if(lightIndex<LIGHT_NUM)lightIndex++;
            else lightIndex=0;
        }*/

    }
    else if(mode==3){
        for(int i=0;i<FFT_GRID_NUM/10;i++){
            for(int j=0;j<FFT_GRID_NUM/10;j++){
                fftGrid[i*10+j].update(buffer,i,j);
                fftGrid[i*10+j].draw();
            }
        }
    }
    
    myFbo.end();
    oscEvent();
    visualEffect();
}

//--------------------------------------------------------------
void ofApp::draw(){
    myGlitch.generateFx();
    ofSetColor(255);
    myFbo.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='1'){
        mode=1;
        for(int i=0;i<BG_NUM;i++){
            myBgParticle[i].set();
        }
        for(int i=0;i<RIP_NUM;i++){
            for(int j=0;j<8;j++){
                if(i>=j*RIP_PERIOD&&i<j*RIP_PERIOD+RIP_PERIOD)ripple_index=j+1;
            }
            myRipples[i].set(ripple_index);
        }
    }
    else if(key=='2'){
        mode=2;
        for(int i=0;i<RIPPLE_NUM;i++){
            fftRipple[i].setup();
        }
        
        for(int i=0;i<FFT_WAVE_NUM;i++){
            fftWave[i].setup();
            simpleLineEffects[i].setup();
        }
    }
    else if(key=='3'){
        mode=3;
        for(int i=0;i<FFT_GRID_NUM;i++){
            fftGrid[i].setup();
        }
    }
    
/*    if(key=='4')chairCondition[0]=1;
    else if(key=='5')chairCondition[1]=1;
    else if(key=='6')chairCondition[2]=1;
    else if(key=='7')chairCondition[3]=1;
    else if(key=='r')chairCondition[4]=1;
    else if(key=='t')chairCondition[5]=1;
    else if(key=='y')chairCondition[6]=1;
    else if(key=='u')chairCondition[7]=1;
    else if(key=='f')chairCondition[8]=1;
    else if(key=='g')chairCondition[9]=1;
    else if(key=='h')chairCondition[10]=1;
    else if(key=='j')chairCondition[11]=1;
    else if(key=='v')chairCondition[12]=1;
    else if(key=='b')chairCondition[13]=1;
    else if(key=='n')chairCondition[14]=1;
    else if(key=='m')chairCondition[15]=1;*/
    if(bManualGlitch){
        if(key=='f')myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER, true);
        else if(key=='g')myGlitch.setFx(OFXPOSTGLITCH_SWELL, true);
        else if(key=='h')myGlitch.setFx(OFXPOSTGLITCH_SHAKER, true);
        else if(key=='j')myGlitch.setFx(OFXPOSTGLITCH_TWIST, true);
        else if(key=='k')myGlitch.setFx(OFXPOSTGLITCH_OUTLINE, true);
        else if(key=='l')myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
        else if(key=='v')myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, true);
        else if(key=='b')myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE, true);
        else if(key=='n')myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN, true);
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key==OF_KEY_RETURN)ofToggleFullscreen();
    else if(key==OF_KEY_COMMAND)cout<<"FPS:"<<ofGetFrameRate()<<endl;
    else if(key==OF_KEY_SHIFT){
        if(!bAutoChange)bAutoChange=true;
        else bAutoChange=false;
    }
    
    if(key=='z'){
        if(!bAutoAudio)bAutoAudio=true;
        else bAutoAudio=false;
        cout<<"bAutoAudio: "<<bAutoAudio<<endl;
    }
    if(key=='s'){
        if(!bManualGlitch)bManualGlitch=true;
        else bManualGlitch=false;
        cout<<"bManualGlitch:"<<bManualGlitch<<endl;
    }
    if(bManualGlitch){
        if(key=='f')myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER, false);
        else if(key=='g')myGlitch.setFx(OFXPOSTGLITCH_SWELL, false);
        else if(key=='h')myGlitch.setFx(OFXPOSTGLITCH_SHAKER, false);
        else if(key=='j')myGlitch.setFx(OFXPOSTGLITCH_TWIST, false);
        else if(key=='k')myGlitch.setFx(OFXPOSTGLITCH_OUTLINE, false);
        else if(key=='l')myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
        else if(key=='v')myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, false);
        else if(key=='b')myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE, false);
        else if(key=='n')myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN,false);
    }
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT,true);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT, false);
    
}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------

void ofApp::audioReceived(float *buf,int bufSize,int nChan){
    for(int i=0;i<BUFFER_SIZE;i++){
        buffer[i]=buf[i];
    }
}

/*-----------シーン変更用メソッド-----------*/
void ofApp::sceneChange(){
    nowTime=ofGetElapsedTimeMillis();
    //    cout<<"target:"<<targetTime<<":"<<nowTime<<endl;
    if(mode==1&&nowTime>targetTime){
        mode=2;
        targetTime=nowTime+25000;
        for(int i=0;i<RIPPLE_NUM;i++){
            fftRipple[i].setup();
        }
        
        for(int i=0;i<FFT_WAVE_NUM;i++){
            fftWave[i].setup();
            simpleLineEffects[i].setup();
        }
        if(bAutoAudio){
            ofxOscMessage m;
            m.setAddress("/live/play/clip");
            m.addIntArg(16);
            m.addIntArg(0);
            sender.sendMessage(m,false);
        }
    }else if(mode==2&&nowTime>targetTime){
        mode=3;
        targetTime=nowTime+40000;
/*        for(int i=0;i<FFT_GRID_NUM;i++){
            fftGrid[i].setup();
        }*/
        if(bAutoAudio){
            ofxOscMessage m2;
            m2.setAddress("/live/play/clip");
            m2.addIntArg(16);
            m2.addIntArg(1);
            sender.sendMessage(m2,false);
        }
    }else if(mode==3&&nowTime>targetTime){
        mode=1;
        targetTime=nowTime+20000;
        for(int i=0;i<BG_NUM;i++){
            myBgParticle[i].set();
        }
        for(int i=0;i<RIP_NUM;i++){
            for(int j=0;j<8;j++){
                if(i>=j*RIP_PERIOD&&i<j*RIP_PERIOD+RIP_PERIOD)ripple_index=j+1;
            }
            myRipples[i].set(ripple_index);
        }
        if(bAutoAudio){
            ofxOscMessage m;
            m.setAddress("/live/stop/track");
            m.addIntArg(16);
            sender.sendMessage(m,false);
        }
    }
}

/*-------------OSC処理用メソッド----------------*/
void ofApp::oscEvent(){
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msg_strings[i] = "";
        }
    }
    
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        if(m.getAddress() == "/gois/beat"){
            beat = m.getArgAsInt32(0);
//            cout<<beat<<endl;
        }
        else if(m.getAddress()=="/gois/chair_num"){
            int i=m.getArgAsInt32(0);
            bool isSitting=m.getArgAsBool(1);
            if(!isSitting)chairCondition[i]=0;
            else if(isSitting)chairCondition[i]=1;
            cout<<"RECEIVE!! "<<i<<":"<<chairCondition[i]<<endl;

        }
        else{
            string msg_string;
            msg_string = m.getAddress();
            msg_string += ": ";
            for(int i = 0; i < m.getNumArgs(); i++){
                msg_string += m.getArgTypeName(i);
                msg_string += ":";
                
                if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                    msg_string += ofToString(m.getArgAsInt32(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    msg_string += ofToString(m.getArgAsFloat(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                    msg_string += m.getArgAsString(i);
                }
                else{
                    msg_string += "unknown";
                }
            }
            
            msg_strings[current_msg_string] = msg_string;
            timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
            current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
            
            msg_strings[current_msg_string] = "";
        }
    }
    if(nowTime>ambientBangTime){
        ambientBangTime=nowTime+120000;
        ofxOscMessage m;
        m.setAddress("/live/play/clip");
        m.addIntArg(14);
        m.addIntArg(0);
        sender.sendMessage(m,false);
    }
}

void ofApp::visualEffect(){
    if(beat==3){
        if(!bSLITSCAN){
            bSLITSCAN=true;
            effectTime=ofGetElapsedTimeMillis()+200;
        }
    }
    else if(beat==4){
        if(!bNOISE&&chairCondition[0]==1){
            bNOISE=true;
        }
    }
    else if(beat==5){
        if(bNOISE)bNOISE=false;
    }
    
    if(nowTime>effectTime){
        if(bSLITSCAN)bSLITSCAN=false;
    }
    
    if(bSLITSCAN)myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN, true);
    else myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN, false);
    if(bNOISE)myGlitch.setFx(OFXPOSTGLITCH_NOISE, true);
    else myGlitch.setFx(OFXPOSTGLITCH_NOISE, false);
}
