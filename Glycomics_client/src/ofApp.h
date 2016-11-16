#pragma once

#include "ofMain.h"
#include "Tree.h"
#include "ofxSyphon.h"
#include "defines.h"
#include "ofxOsc.h"



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    vector<Tree> trees;
    ofJson tree_1_json;
    
    
    ofxOscReceiver oscReceiver;
    ofxOscSender oscSender;
    void parseOSC(ofxOscReceiver &receiver);
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    ofFbo render;
    ofxSyphonServer syphon;
    
    bool doDrawDebug = false;
		
};
