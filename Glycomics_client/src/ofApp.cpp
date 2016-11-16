#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // OSC setup
    oscReceiver.setup(RECEIVE_PORT);
    oscSender.setup(SEND_HOST, SEND_PORT);
    
    
    
//    ofFile file("tree_struktures/tree_1.json");
//    if(file.exists()){
//        file >> tree_1_json;
//        Tree tree;
//        tree.setup(tree_1_json);
//        trees.push_back(tree);
//    }

    // RENDER SETUP
    render.allocate(RENDER_W, RENDER_H);
    render.begin();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render.end();
    
    syphon.setName("TREE_1");
    
}

//--------------------------------------------------------------
void ofApp::update(){
    parseOSC(oscReceiver);
    
     for(auto & t : trees){
  //       t.update();
     }
}

//--------------------------------------------------------------
void ofApp::draw(){
    render.begin();
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ofBackground(0);
        
        
        
        for(auto & t : trees){
           // t.font.drawString(t.title, 40, 40);

            ofPushMatrix();
            ofTranslate(40, RENDER_H-40);
            ofRotateDeg(-90);
            t.fontTitle.drawString(t.title, 0, 0);

            ofPopMatrix();
            
            if(t.isInitialised){
                if(doDrawDebug)t.drawDebug();
                t.draw();
            }
        }

    }
    render.end();
    
    render.draw(0,0, RENDER_W/4, RENDER_H/4);
    
    
    ofFill();
    syphon.publishTexture(&render.getTexture());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'x'){
        ofxOscMessage m;
        m.setAddress("/newContent");
        m.addFloatArg(1);
        oscSender.sendMessage(m, false);
    }
    if(key == 'd'){
        doDrawDebug = !doDrawDebug;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
//--------------------------------------------------------------
void ofApp::parseOSC(ofxOscReceiver &receiver){
    // hide old messages
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msg_strings[i] = "";
        }
    }
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        // cout << m.getAddress() << endl;
        std::vector<std::string> address = ofSplitString(m.getAddress(),"/",true);
        
        // change Syphon remote
        if(address[0]=="newTree") {
            string newContent = m.getArgAsString(0);
            stringstream s;
            s << newContent;
            s >> tree_1_json;
            
            trees.clear();
            Tree tree;
            tree.setup(tree_1_json);
            trees.push_back(tree);
        }else {
            std::cout << "Not found\n";
        }
        
    }
}




//--------------------------------------------------------------

