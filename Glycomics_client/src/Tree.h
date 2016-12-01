//
//  Tree.h
//  Glycomics
//
//  Created by Jonas Fehr on 14/11/16.
//
//
#include "MSAPhysics2D.h"
#include "ofMain.h"
#include "defines.h"
#include "Branch.h"

using namespace msa::physics;



#ifndef Tree_h
#define Tree_h

enum TreeState {
    READY=0,
    BUILD=1,
    TRANSFORM=2,
    REBUILD=3,
    USED=4
};


class Tree{
public:

    bool isInitialised = false;
    World2D_ptr         world;
    map<int, Branch> branches;
    map<int, Particle2D_ptr> nodes;
    map<int, Particle2D_ptr> nodesFixed;

    
    ofTrueTypeFont font;
    ofTrueTypeFont fontTitle;
    string fontPath;
    string title;

    float seedOffset;
    
    int width;
    int height;
    
    int index = 0;
    int adjIndex = 0;
    ofJson tree_json;


    int status = READY;
    
    void initPhysics() {
        width  = RENDER_W;
        height = RENDER_H;
        
        // initialize our physics world
        world = World2D::create();
        
        world->setGravity(ofVec3f(0, GRAVITY, 0));
        
        // set world dimensions, not essential, but speeds up collision
        world->setWorldSize(ofVec2f(-100, 0), ofVec2f(width+100, height));
        world->setSectorCount(SECTOR_COUNT);
        world->setDrag(DRAG);
        //world->enableCollision();
    }
    
    void setup(ofJson _tree_json){

        //load Font
        ofDirectory dir("fonts");
        dir.listDir();
        if(dir.size()>0) fontPath = dir.getPath(0);
        font.load(fontPath, FONT_SIZE);
        fontTitle.load(fontPath, FONT_SIZE_TITLE);
        tree_json = _tree_json;
        
        initPhysics();
       
        // Setup the tree structure
        nodes[index] = world->makeParticle( ofVec2f(RENDER_W/2, RENDER_H) )->setMass(MASS)->setBounce(BOUNCE)->setRadius(RADIUS)->makeFixed();
        
        
        title = tree_json["title"];
        
        seedOffset = ofRandom(0, 10000);
        
        for(auto & b: tree_json["branches"]){
            if(!b.empty()){
                
                
                index++;

                
                Branch branch;
                string adjective = b["adjective"];
                branch.setup( index,  b["parent"], b["angle"], b["changable"], adjective, font);
                branches[index] = branch;
                
                nodes[index] = world->makeParticle(branch.getEndPos( nodes[b["parent"]]->getPosition()))->setMass(MASS)->setBounce(BOUNCE)->setRadius(RADIUS)->makeFree();//->enableCollision();
                nodesFixed[index] = world->makeParticle( nodes[index]->getPosition() )->setMass(MASS)->setBounce(BOUNCE)->setRadius(RADIUS)->makeFixed();//->enableCollision();

                
                // create spring
                float distance = nodes[b["parent"]]->getPosition().distance( nodes[index]->getPosition() );
                world->makeSpring(nodes[ b["parent"] ], nodes[index], SPRING_STRENGTH, distance);
                    
                    world->makeSpring(nodes[index], nodesFixed[index], SPRING_ORIGIN_STRENGTH, 0);


            }
        }
        
        adjIndex = index;
        isInitialised = true;
        
    }
    
    void update(){
        world->update();
        
        
        for ( auto & node : nodes)
        {
            ofVec2f pos = node.second->getPosition();
            
            float perPointNoise = (ofNoise(pos.x/NOISE_SPACING_DIV, pos.y/NOISE_SPACING_DIV, ofGetElapsedTimef()/NOISE_TIMER_FINE_DIV + seedOffset)-0.5)*2;
            float masterNoise = (ofNoise(ofGetElapsedTimef()/NOISE_TIMER_ALL_DIV + seedOffset)-0.5)*2;
            node.second->addVelocity( ofVec2f( ((perPointNoise*(1-MIX_NOISE)) +  (masterNoise*MIX_NOISE)) * NOISE_FORCE_MUL, 0) );
        }
        
        // START CREATING TREE
        if( status==BUILD && branches[1].status == INVISIBLE ) branches[1].status = MAKE_VISIBLE;

        
        for (auto & branch : branches)
        {
            
            
            switch(status)
            {
                case READY  :
                    break;
                    
                case BUILD :
                    if(branches[branch.second.parent].status == VISIBLE && !branch.second.changable && branch.second.alpha < 1) branch.second.status = MAKE_VISIBLE;
                    break;
                case TRANSFORM :
                    if(branch.second.changable){
                        if(branches[branch.second.parent].status == VISIBLE && !branch.second.hasChild){
                            if(ofRandom(0,100) <= PROB_TRANSFORM){
                                if( branch.second.status == INVISIBLE){
                                    branch.second.status = MAKE_VISIBLE;
                                    branches[branch.second.parent].hasChild = true;
                                }
                                if( branch.second.status == VISIBLE){
                                    branch.second.status = MAKE_INVISIBLE;
                                    branches[branch.second.parent].hasChild = false;
                                }
                            }
                        }
                    }

                    break;
                    
                case REBUILD :
                    if(branch.second.status != MAKE_INVISIBLE && branch.second.status != INVISIBLE ) branch.second.status = MAKE_INVISIBLE;
                    
                    break;
            }
            

            
//            if((i-1)>0)if(branches[i-1].isVisible) branch.second.makeVisible = true;
//            i++;
            
            ofVec2f startPos = nodes[branch.second.parent]->getPosition();
            ofVec2f endPos = nodes[branch.second.index]->getPosition();
            ofVec2f startParent = nodes[branches[branch.second.parent].parent]->getPosition();

            branch.second.update(startPos, endPos, startParent);
            
//            cout << "status Branch: " <<  branch.second.status <<endl;

            
  
            
        }
        
//        cout << endl;
        // MAKE TRANSFORM IF BUILD
        if(status == BUILD){
            bool isMakeVisible = false;
            for (auto & branch : branches)
            {
                if( branch.second.status == MAKE_VISIBLE && !branch.second.changable) isMakeVisible = true;
            }
            if(!isMakeVisible) status = TRANSFORM;
        }
        // MAKE USED IF REBUILT
        if(status == REBUILD){
            bool isMakeInvisible = false;
            for (auto & branch : branches)
            {
                if( branch.second.status == MAKE_INVISIBLE){
                 isMakeInvisible = true;
                }
            }
            if(!isMakeInvisible) status = USED;
        }
        
        
        
        // DRAW POINTS
//        if(true){
//            for (auto & branch : branches)
//            {
//                
//                ofFill();
//                ofSetColor(255, branch.second.alpha);
//                ofPoint point = ofPoint(100, 100);//nodes[branch.second.index]->getPosition();
//                ofDrawCircle(point.x, point.y, 0, POINTS_SIZE);
//            }
//        }

        
    }
    
    void drawDebug(){
        // draw Particles
        ofSetColor(255,0,0);
        ofFill();
        for(auto & p : world->getParticles()){
            ofDrawCircle(p->getPosition(), 10);
        }
        
        // draw springs
        ofSetColor(0,0,255);
        ofSetLineWidth(4);
        for(int i=0; i<world->numberOfSprings(); i++) {
            Spring2D_ptr s  = world->getSpring(i);
            ofPoint a = s->getA()->getPosition();
            ofPoint b = s->getB()->getPosition();
            ofDrawLine(a, b);
            
        }
    
        // draw Beziers
        ofSetLineWidth(5);
        for (auto & branch : branches)
        {
            ofSetColor(255,150,0);
            ofDrawLine((ofPoint)branch.second.startPos, (ofPoint)branch.second.startBezier);
            ofDrawLine((ofPoint)branch.second.endPos, (ofPoint)branch.second.endBezier);
            ofDrawCircle(branch.second.endBezier, 5);
            ofDrawCircle(branch.second.startBezier, 5);
            ofSetColor(255,255,0);
            branch.second.drawBezier();
        }
                
    }
    
    void draw(){
        
        ofSetColor(255);
        font.getFontTexture().bind();
        
        for (auto & branch : branches)
        {
            branch.second.draw();
        }
        font.getFontTexture().unbind();
        
        if(HAS_POINTS){
            for (auto & branch : branches)
            {
                branch.second.drawPoint();
            }
        }


    }
    
    
};




#endif /* Tree_h */
