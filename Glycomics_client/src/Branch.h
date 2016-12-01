//
//  Branch.h
//  Glycomics
//
//  Created by Jonas Fehr on 18/11/16.
//
//
#include "ofMain.h"
#include "ofxEasing.h"

#ifndef Branch_h
#define Branch_h

enum Status {
    INVISIBLE = 0,
    MAKE_VISIBLE = 1,
    VISIBLE = 2,
    MAKE_INVISIBLE = 3
};

class Branch{
public:
    int index;
    int parent;
    float angle;
    bool changable;
    string adjective;
    ofPolyline line;
    
    ofVec2f startPos;
    ofVec2f endPos;
    
    ofPoint startBezier;
    ofPoint endBezier;
    ofTrueTypeFont font;
    
    
    bool hasChild = false;
    
    float alpha = 0;
    

    int status = INVISIBLE;
    
    void setup(int _index, int _parent, float _angle, bool _changable, string _adjective, ofTrueTypeFont _font){
        index = _index;
        parent = _parent;
        angle = _angle;
        changable = _changable;
        adjective = oldAdj = _adjective;
        font = _font;
        
        adjMesh = font.getStringMesh(adjective,0,0);
        width   = font.stringWidth(adjective);
        height  = font.stringHeight(adjective);
        
    }
    
    void update(ofVec2f _startPos, ofVec2f _endPos, ofVec2f startParent){
        startPos = _startPos;
        endPos = _endPos;
        
        
        rotation = ofVec2f(1,0).angle(endPos-startPos);

        startBezier = startPos+ofVec2f(0, -BEZIER_LENGTH).rotate(angle);
        endBezier = endPos+ofVec2f(0, BEZIER_LENGTH);//.rotate(angle+90);

        if(angle == 90)  endBezier = endPos+ofVec2f(-BEZIER_LENGTH, 0);//.rotate(angle+90);
        if(angle == -90){
            endBezier = endPos+ofVec2f(BEZIER_LENGTH, 0);
            createBeizerLine(endPos, startPos, endBezier, startBezier);
        }else{
        
        createBeizerLine(startPos, endPos, startBezier, endBezier);
        }
        
    }
    
    
    void draw(){
        

//        cout << "s: " << status << " a: " << alpha << endl;
        
        
        switch(status)
        {
            case INVISIBLE  :
                break;
            case MAKE_VISIBLE:
                alpha+=ALPHA_STEP;
                if(alpha > 1.) {
                    status = VISIBLE;
                    alpha = 1.;
                }
                break;
            case VISIBLE :
                break;
            case MAKE_INVISIBLE :
                alpha-=ALPHA_STEP;
                if(alpha < 0) {
                    status = INVISIBLE;
                    alpha = 0.;
                }
                break;
        }
        
    
        
        if(oldAdj != adjective){
            adjMesh = font.getStringMesh( adjective ,0,0);
            oldAdj = adjective;
        }

        
        ofMesh tempMesh = adjMesh;
        
        // modifie 4 vertices at a time
        
        float lengthPoly  = line.getLengthAtIndex(line.getVertices().size());
        float lengthStart = (lengthPoly-width)/2;
        
        for( int i = 0; i < tempMesh.getVertices().size(); i+=4){
            // substract an offset to get a rotation center at the coordinate 0,0
            float offsetY = -height/4;
            float offsetX = (tempMesh.getVertex(i).x + tempMesh.getVertex(i+1).x)/2;
//            cout << "offsetX " << offsetX  << " offsetY " << offsetY << endl;
            
            ofVec2f vertex0 = tempMesh.getVertex(i+0);
            ofVec2f vertex1 = tempMesh.getVertex(i+1);
            ofVec2f vertex2 = tempMesh.getVertex(i+2);
            ofVec2f vertex3 = tempMesh.getVertex(i+3);
            
            vertex0.operator-=(ofVec2f(offsetX, offsetY));
            vertex1.operator-=(ofVec2f(offsetX, offsetY));
            vertex2.operator-=(ofVec2f(offsetX, offsetY));
            vertex3.operator-=(ofVec2f(offsetX, offsetY));

            
            // find Rotation and new Center;
//            int j = i/4+1+SPACES;
//            ofVec2f newCenter = line.operator[](j);
//            float rotationAngle = ofVec2f(1,0).angle(line.operator[](j+1)-line.operator[](j-1));
            
            // find rotation and new Center based on length
            ofVec2f newCenter = line.getPointAtLength(lengthStart+offsetX);
            ofVec2f tangentVector = line.getTangentAtIndexInterpolated(line.getIndexAtLength(lengthStart+offsetX));
            float rotationAngle = ofVec2f(1,0).angle(tangentVector);
            

            // transform the points
            vertex0.rotate(rotationAngle).operator+=(newCenter);
            vertex1.rotate(rotationAngle).operator+=(newCenter);
            vertex2.rotate(rotationAngle).operator+=(newCenter);
            vertex3.rotate(rotationAngle).operator+=(newCenter);


            tempMesh.setVertex(i+0, (ofVec3f)vertex0);
            tempMesh.setVertex(i+1, (ofVec3f)vertex1);
            tempMesh.setVertex(i+2, (ofVec3f)vertex2);
            tempMesh.setVertex(i+3, (ofVec3f)vertex3);
            
        }
        
        ofSetColor(255, ofxeasing::map_clamp(alpha, 0, 1., 0, 255, &ofxeasing::quad::easeOut));
        
        tempMesh.draw();
        

        
    }
    
    void drawPoint(){
        ofFill();
        ofSetColor(255, ofxeasing::map_clamp(alpha, 0, 1., 0, 255, &ofxeasing::quad::easeOut));
        ofDrawCircle(startPos.x, startPos.y, 0, POINTS_SIZE);
    }
    
    void drawBezier(){
        line.draw();
    }
    
    ofVec2f getEndPos( ofVec2f startPos){
        return startPos.operator+( ofVec2f(width+2*WORDSPACING, 0).getRotated(-90+angle) );
    }
    
    
private:
    ofMesh adjMesh;
    ofTexture *fontText;
    float width;
    float height;
    float rotation;
    string oldAdj;

    
    bool isOrtho = false;


    
    
    void createBeizerLine(ofPoint startP, ofPoint endP, ofPoint startB, ofPoint endB){
        line.clear();
        line.addVertex(startP);

        line.bezierTo(startB, endB, endP, adjective.length()+SPACES*2);
    }
    
    
};

#endif /* Branch_h */
