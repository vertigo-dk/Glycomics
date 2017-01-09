#pragma once

#include "ofMain.h"
#include "Tree.h"
#include "ofxSyphon.h"
#include "defines.h"
#include "ofxOsc.h"
#include "ofxGui.h"





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
    
    
    
    
    ofxOscReceiver oscReceiver;
    ofxOscSender oscSender;
    ofxOscSender oscSenderWatchdog;
    void parseOSC(ofxOscReceiver &receiver);
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    
    struct Output {
        ofFbo render;
        ofxSyphonServer syphon;
        vector<Tree> trees;
        ofJson current_tree_json;
        ofJson new_tree_json;

    };
    Output outputs[3];
    
    
    
    bool doDrawDebug = false;
    
    
    // GUI
    ofxPanel gui;
    bool showGui;
    
    ofParameter<float> windSpeed;
    ofParameter<float> windStrength;
    ofParameter<float> stringNodes;

		
};
