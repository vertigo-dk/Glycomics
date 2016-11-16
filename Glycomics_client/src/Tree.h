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

using namespace msa::physics;



#ifndef Tree_h
#define Tree_h
class Letter{
public:
    
};

class Branch{
public:
    int index;
    int parent;
    float angle;
    bool changable;
    string adjectiv;
    
    



    
    void setup(int _index, int _parent, float _angle, bool _changable, string _adjectiv, ofTrueTypeFont &font){
        index = _index;
        parent = _parent;
        angle = _angle;
        changable = _changable;
        adjectiv = _adjectiv;
        
        adjMesh = font.getStringMesh(adjectiv,0,0);
        width   = font.stringWidth(adjectiv);
        height  = font.stringHeight(adjectiv);
        
    }
    
    
    void draw(ofVec2f startPos, ofVec2f endPos){
        ofTranslate(startPos);
        ofRotateDeg(ofVec2f(1,0).angle(endPos-startPos) );
        
        ofTranslate(WORDSPACING, height/2);
        adjMesh.draw();
    }
    
    ofVec2f getEndPos( ofVec2f startPos){
        return startPos.operator+( ofVec2f(width+2*WORDSPACING, 0).getRotated(-90+angle) );
    }
    
    
private:
    ofMesh adjMesh;
    ofTexture *fontText;
    float width;
    float height;

    
};


class Tree{
public:

    bool isInitialised = false;
    World2D_ptr         world;
    map<int, Branch> branches;
    map<int, Particle2D_ptr> nodes;
    
    ofTrueTypeFont font;
    ofTrueTypeFont fontTitle;
    string fontPath;
    string title;
    
    int width;
    int height;

    void initPhysics() {
        width  = RENDER_W;
        height = RENDER_H;
        
        // initialize our physics world
        world = World2D::create();
        
        world->setGravity(ofVec3f(0, GRAVITY, 0));
        
        // set world dimensions, not essential, but speeds up collision
        world->setWorldSize(ofVec2f(BORDER_L, 0), ofVec2f(width-BORDER_R, height));
        world->setSectorCount(SECTOR_COUNT);
        world->setDrag(0.97f);
        //world->enableCollision();
    }
    
    void setup(ofJson tree_json){

        //load Font
        ofDirectory dir("fonts");
        dir.listDir();
        if(dir.size()>0) fontPath = dir.getPath(0);
        font.load(fontPath, FONT_SIZE);
        fontTitle.load(fontPath, FONT_SIZE_TITLE);
        
        
        initPhysics();
       
        // Setup the tree structure
        int index = 0;
        nodes[index] = world->makeParticle( ofVec2f(RENDER_W/2, RENDER_H) )->setMass(MASS)->setBounce(BOUNCE)->setRadius(RADIUS)->makeFixed();
        
        title = tree_json["title"];
        
        
        for(auto & b: tree_json["branches"]){
            if(!b.empty()){
                index++;

                
                Branch branch;
                string adjective = b["adjective"];
                branch.setup( index,  b["parent"], b["angle"], b["changable"], ( adjective ), font);
                branches[index] = branch;
                
                nodes[index] = world->makeParticle(branch.getEndPos( nodes[b["parent"]]->getPosition()))->setMass(MASS)->setBounce(BOUNCE)->setRadius(RADIUS)->makeFree();//->enableCollision();
                
                
                // create spring
                float distance = nodes[b["parent"]]->getPosition().distance( nodes[index]->getPosition() );
                world->makeSpring(nodes[ b["parent"] ], nodes[index], SPRING_STRENGTH, distance);
                
                // create attractors
                for(int i=0; i<world->numberOfParticles()-1; i++){
                    Particle2D_ptr p = world->getParticle(i);
                    if(p == nodes[index] && p == nodes[b["parent"]]) cout << "got here";

                    if(p != nodes[index] && p != nodes[b["parent"]]){

                        world->makeAttraction(nodes[index], p, RETRACTION);
                    }
                }
                
            }
        }
        
        isInitialised = true;
        
    }
    
    void update(){
        world->update();
    }
    
    void drawDebug(){
        for(auto & p : world->getParticles()){
            ofSetColor(255,0,0);
            ofFill();
            ofDrawCircle(p->getPosition(), 10);
            
        }
        
        
        
        for(int i=0; i<world->numberOfSprings(); i++) {
            Spring2D_ptr s  = world->getSpring(i);
            ofSetColor(0,0,255);
            ofSetLineWidth(4);
            ofPoint a = s->getA()->getPosition();
            ofPoint b = s->getB()->getPosition();
            ofDrawLine(a, b);
            
        }
    }
    
    void draw(){
        
        ofSetColor(255);
        font.getFontTexture().bind();
        
        for (auto & branch : branches)
        {
            ofPushMatrix();
            ofVec2f startPos = nodes[branch.second.parent]->getPosition();
            ofVec2f endPos = nodes[branch.second.index]->getPosition();

            branch.second.draw(startPos, endPos);
            ofPopMatrix();

        }
        font.getFontTexture().unbind();

    }
    
    
};




#endif /* Tree_h */
