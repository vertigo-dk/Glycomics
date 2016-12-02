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
    int i = 0;
    for(auto & output : outputs){
        output.render.allocate(RENDER_W, RENDER_H);
        output.render.begin();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        output.render.end();
        i++;
        output.syphon.setName("TREE_"+ofToString(i));
    }

    
}

//--------------------------------------------------------------
void ofApp::update(){
    parseOSC(oscReceiver);
    for(auto & output : outputs){
        int numOfTrees = output.trees.size();
        
        
        
        
        // Built Tree if solo
        if( numOfTrees == 1 && output.trees[0].status == READY) output.trees[0].status = BUILD;
        
        if( numOfTrees > 1){
            if(output.trees[0].status == TRANSFORM) output.trees[0].status = REBUILD;
            if(output.trees[0].status == USED) output.trees.erase(output.trees.begin());
        }
        for(auto & t : output.trees){
            t.update();
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    int i = 0;
    for(auto & output : outputs){
        output.render.begin();
        {
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            if(BG_GRAD){
                ofBackgroundGradient(ofColor(BG_TOP), ofColor(BG_BUT));
            }  else{
               ofBackground(0);
            }
            
            
            
            for(auto & t : output.trees){
                // t.font.drawString(t.title, 40, 40);
                
                //            ofPushMatrix();
                //            ofTranslate(60, RENDER_H-40);
                //            ofRotateDeg(-90);
                //            t.fontTitle.drawString(t.title, 0, 0);
                //            ofPopMatrix();
                
                if(t.isInitialised){
                    if(doDrawDebug)t.drawDebug();
                    t.draw();
                }
            }
            
        }
        output.render.end();
        
        output.render.draw(i*(RENDER_W/5+20)+20,20, RENDER_W/5, RENDER_H/5);
        ofFill();
        output.syphon.publishTexture(&output.render.getTexture());
        i++;
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'x'){
        ofxOscMessage m;
        m.setAddress("/newContent");
        m.addFloatArg(ceil(ofRandom(0, 3)));
        m.addFloatArg(ceil(ofRandom(0, 3)));
        m.addFloatArg(ceil(ofRandom(0, 3)));
        oscSender.sendMessage(m, false);
    }
    
    if(key == '1'){
        ofxOscMessage m;
        m.setAddress("/newContent");
        m.addFloatArg(1);
        oscSender.sendMessage(m, false);
    }
    if(key == '2'){
        ofxOscMessage m;
        m.setAddress("/newContent");
        m.addFloatArg(2);
        oscSender.sendMessage(m, false);
    }
    if(key == '3'){
        ofxOscMessage m;
        m.setAddress("/newContent");
        m.addFloatArg(3);
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
            string newContent = m.getArgAsString(1);
            int outIndex = m.getArgAsInt(0)-1;
            
                stringstream s;
                s << newContent;
                s >> outputs[outIndex].new_tree_json;
                
//                outputs[outIndex].trees.clear();
                Tree tree;
                tree.setup(outputs[outIndex].new_tree_json);
                outputs[outIndex].trees.push_back(tree);
            
        }else {
            std::cout << "Not found\n";
        }
        
    }
}




//--------------------------------------------------------------

