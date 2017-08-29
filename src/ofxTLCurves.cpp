/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "ofxTLCurves.h"
#include "ofxTimeline.h"
#include "ofxHotKeys.h"

ofxTLCurves::ofxTLCurves(){
    initializeEasings();
    valueRange = ofRange(0.0, 1.0);
    drawingEasingWindow = false;
    defaultEasingType = 0;
    defaultEasingFunction = 0;
}

float ofxTLCurves::interpolateValueForKeys(ofxTLKeyframe* start, ofxTLKeyframe* end, unsigned long long sampleTime){
    ofxTLTweenKeyframe* tweenKeyStart = (ofxTLTweenKeyframe*)start;
    ofxTLTweenKeyframe* tweenKeyEnd = (ofxTLTweenKeyframe*)end;
    return ofxeasing::map(sampleTime, tweenKeyStart->time, tweenKeyEnd->time,
                          tweenKeyStart->value, tweenKeyEnd->value,
                          ofxeasing::easing(tweenKeyStart->easeFunc->easing, tweenKeyStart->easeType->type));
}

string ofxTLCurves::getTrackType() const{
    return TRACK_TYPE;
}

ofJson ofxTLCurves::getStructure() const{
    return {{"name", name}, {"type", getTrackType()}, {"min", valueRange.min}, {"max", valueRange.max}};
}

ofxTLKeyframe* ofxTLCurves::newKeyframe(){
    ofxTLTweenKeyframe* k = new ofxTLTweenKeyframe();
    k->easeFunc = &easingFunctions[defaultEasingFunction];
    k->easeType = &easingTypes[defaultEasingType];
    return k;
}

void ofxTLCurves::drawModalContent(){
    
    //****** DRAW EASING CONTROLS
    if(!drawingEasingWindow){
        return;
    }
    
    ofxTLTweenKeyframe* tweenFrame = (ofxTLTweenKeyframe*) selectedKeyframe;
    if(tweenFrame == NULL){
        if(selectedKeyframes.size() == 0){
            return;
        }
        tweenFrame = (ofxTLTweenKeyframe*)selectedKeyframes[0];
    }
    
    for(int i = 0; i < easingTypes.size(); i++){
        //TODO turn into something like selectionContainsEaseType();
        //so that we can show the multi-selected easies
        if(&easingTypes[i] ==  ((ofxTLTweenKeyframe*)selectedKeyframes[0])->easeType){
            ofSetColor(150, 100, 10);
        }
        else{
            ofSetColor(80, 80, 80);
        }
        ofFill();
        ofDrawRectangle(easingWindowPosition.x + easingTypes[i].bounds.x, easingWindowPosition.y + easingTypes[i].bounds.y,
                        easingTypes[i].bounds.width, easingTypes[i].bounds.height);
        ofSetColor(200, 200, 200);
        timeline->getFont().drawString(easingTypes[i].name,
                                       easingWindowPosition.x + easingTypes[i].bounds.x+11,
                                       easingWindowPosition.y + easingTypes[i].bounds.y+10);
        ofNoFill();
        ofSetColor(40, 40, 40);
        ofDrawRectangle(easingWindowPosition.x + easingTypes[i].bounds.x,
                        easingWindowPosition.y + easingTypes[i].bounds.y,
                        easingTypes[i].bounds.width, easingTypes[i].bounds.height);
    }
    
    for(int i = 0; i < easingFunctions.size(); i++){
        //TODO: turn into something like selectionContainsEaseFunc();
        if(&easingFunctions[i] == tweenFrame->easeFunc){
            ofSetColor(150, 100, 10);
        }
        else{
            ofSetColor(80, 80, 80);
        }
        ofFill();
        ofDrawRectangle(easingWindowPosition.x + easingFunctions[i].bounds.x, easingWindowPosition.y +easingFunctions[i].bounds.y,
                        easingFunctions[i].bounds.width, easingFunctions[i].bounds.height);
        ofSetColor(200, 200, 200);
        //        timeline->getFont().drawString(easingFunctions[i]->name,
        //                           easingWindowPosition.x + easingFunctions[i]->bounds.x+10,
        //                           easingWindowPosition.y + easingFunctions[i]->bounds.y+15);
        ofPushMatrix();
        ofTranslate(easingWindowPosition.x + easingFunctions[i].bounds.x,
                    easingWindowPosition.y + easingFunctions[i].bounds.y);
        if(tweenFrame->easeType->type == ofxeasing::Type::In){
            easingFunctions[i].easeInPreview.draw();
        }
        else if(tweenFrame->easeType->type == ofxeasing::Type::Out){
            easingFunctions[i].easeOutPreview.draw();
        }
        else {
            easingFunctions[i].easeInOutPreview.draw();
        }
        
        ofPopMatrix();
        ofNoFill();
        ofSetColor(40, 40, 40);
        ofDrawRectangle(easingWindowPosition.x + easingFunctions[i].bounds.x, easingWindowPosition.y +easingFunctions[i].bounds.y,
                        easingFunctions[i].bounds.width, easingFunctions[i].bounds.height);
    }
    
}

bool ofxTLCurves::mousePressed(ofMouseEventArgs& args, long millis){
    if(drawingEasingWindow){
        return true;
    }
    else {
        return ofxTLKeyframes::mousePressed(args,millis);
    }
}

bool ofxTLCurves::mouseDragged(ofMouseEventArgs& args, long millis){
    if(!drawingEasingWindow){
        return ofxTLKeyframes::mouseDragged(args, millis);
    }else{
        return false;
    }
}

bool ofxTLCurves::mouseReleased(ofMouseEventArgs& args, long millis){
    if(drawingEasingWindow && (args.button == 0 && !ofGetModifierControlPressed()) ){
        drawingEasingWindow = false;
        timeline->dismissedModalContent();
        ofVec2f screenpoint(args.x,args.y);
        for(int i = 0; i < easingFunctions.size(); i++){
            if(easingFunctions[i].bounds.inside(screenpoint-easingWindowPosition)){
                for(int k = 0; k < selectedKeyframes.size(); k++){
                    ((ofxTLTweenKeyframe*)selectedKeyframes[k])->easeFunc = &easingFunctions[i];
                }
                timeline->flagTrackModified(this);
                shouldRecomputePreviews = true;
                return true;
            }
        }
        
        for(int i = 0; i < easingTypes.size(); i++){
            if(easingTypes[i].bounds.inside(screenpoint-easingWindowPosition)){
                for(int k = 0; k < selectedKeyframes.size(); k++){
                    ((ofxTLTweenKeyframe*)selectedKeyframes[k])->easeType = &easingTypes[i];
                }
                timeline->flagTrackModified(this);
                shouldRecomputePreviews = true;
                return true;
            }
        }
        return false;
    }
    else{
        return ofxTLKeyframes::mouseReleased(args, millis);
    }
}

bool ofxTLCurves::keyPressed(ofKeyEventArgs& args){
    auto ret = false;
    if ( args.key == 'e')
    {
        if ( selectedKeyframes.size() > 0 )
        {
            
            
            for(int k = 0; k < selectedKeyframes.size(); k++){
                ((ofxTLTweenKeyframe*)selectedKeyframes[k])->easeType = &easingTypes[defaultEasingType];
                ((ofxTLTweenKeyframe*)selectedKeyframes[k])->easeFunc = &easingFunctions[defaultEasingFunction];
            }
            timeline->flagTrackModified(this);
            shouldRecomputePreviews = true;
        }
        ret = true;
    }
    
    return ret | ofxTLKeyframes::keyPressed( args );
    
    
}


void ofxTLCurves::setDefaultEasingType( int index ){
    if ( index < 0 ) index = 0;
    if ( index >= easingTypes.size()) index = easingTypes.size() - 1;
    defaultEasingType = index;
}

int ofxTLCurves::getDefaultEasingType(){
    return defaultEasingType;
}

void ofxTLCurves::setDefaultEasingFunction( int index ){
    if ( index < 0 ) index = 0;
    if ( index >= easingFunctions.size()) index = easingFunctions.size() - 1;
    defaultEasingFunction = index;
}

int ofxTLCurves::getDefaultEasingFunction(){
    return defaultEasingFunction;
}

void ofxTLCurves::selectedKeySecondaryClick(ofMouseEventArgs& args){
    float easingBoxHeight = tweenBoxHeight*easingFunctions.size();
    easingWindowPosition = ofVec2f(MIN(args.x, bounds.width - easingBoxWidth*2),
                                   MIN(args.y, timeline->getBottomLeft().y - easingBoxHeight));
    
    //keep on screen at all costs.
    
    easingWindowPosition.x = ofClamp(easingWindowPosition.x, timeline->getDrawRect().x, ofGetWidth()-easingBoxWidth*2);
    easingWindowPosition.y = ofClamp(easingWindowPosition.y, timeline->getDrawRect().y, ofGetHeight()-easingBoxHeight);
    
    drawingEasingWindow = true;
    timeline->presentedModalContent(this);
}

void ofxTLCurves::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLTweenKeyframe* tweenKey =  (ofxTLTweenKeyframe*)key;
    tweenKey->easeFunc = &easingFunctions[ofClamp(xmlStore.getValue("easefunc", 0), 0, easingFunctions.size()-1)];
    tweenKey->easeType = &easingTypes[ofClamp(xmlStore.getValue("easetype", 0), 0, easingTypes.size()-1)];
}

void ofxTLCurves::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLTweenKeyframe* tweenKey =  (ofxTLTweenKeyframe*)key;
    xmlStore.addValue("easefunc", tweenKey->easeFunc->id);
    xmlStore.addValue("easetype", tweenKey->easeType->id);
}

void ofxTLCurves::initializeEasings(){
    
    //FUNCTIONS ----
    easingFunctions.emplace_back(ofxeasing::Function::Linear, "linear");
    easingFunctions.emplace_back(ofxeasing::Function::Sine, "sine");
    easingFunctions.emplace_back(ofxeasing::Function::Circular, "circular");
    easingFunctions.emplace_back(ofxeasing::Function::Quadratic, "quadratic");
    easingFunctions.emplace_back(ofxeasing::Function::Cubic, "cubic");
    easingFunctions.emplace_back(ofxeasing::Function::Cubic, "quartic");
    easingFunctions.emplace_back(ofxeasing::Function::Quintic, "quintic");
    easingFunctions.emplace_back(ofxeasing::Function::Exponential, "exponential");
    easingFunctions.emplace_back(ofxeasing::Function::Back, "back");
    easingFunctions.emplace_back(ofxeasing::Function::Bounce, "bounce");
    easingFunctions.emplace_back(ofxeasing::Function::Elastic, "elastic");
    
    ///TYPES -------
    easingTypes.emplace_back(ofxeasing::Type::In, "ease in");
    easingTypes.emplace_back(ofxeasing::Type::Out, "ease out");
    easingTypes.emplace_back(ofxeasing::Type::InOut, "ease in-out");
    
    
    tweenBoxWidth = 40;
    tweenBoxHeight = 30;
    easingBoxWidth  = 80;
    easingBoxHeight = 15;
    
    //    easingWindowSeperatorHeight = 4;
    
    for(int i = 0; i < easingTypes.size(); i++){
        easingTypes[i].bounds = ofRectangle(0, i*easingBoxHeight, easingBoxWidth, easingBoxHeight);
        easingTypes[i].id = i;
    }
    
    for(int i = 0; i < easingFunctions.size(); i++){
        easingFunctions[i].bounds = ofRectangle(easingBoxWidth + tweenBoxWidth * (i/3), (i%3)*tweenBoxHeight, tweenBoxWidth, tweenBoxHeight);
        easingFunctions[i].id = i;
        //build preview
        for(int p = 1; p < tweenBoxWidth-1; p++){
            float percent;
            percent = ofxeasing::map(1.0*p/tweenBoxWidth, 0, 1.0, tweenBoxHeight-5, 5, ofxeasing::easing(easingFunctions[i].easing, ofxeasing::Type::In));
            easingFunctions[i].easeInPreview.addVertex(ofPoint(p, percent));
            percent = ofxeasing::map(1.0*p/tweenBoxWidth, 0, 1.0, tweenBoxHeight-5, 5, ofxeasing::easing(easingFunctions[i].easing, ofxeasing::Type::Out));
            easingFunctions[i].easeOutPreview.addVertex(ofPoint(p, percent));
            percent = ofxeasing::map(1.0*p/tweenBoxWidth, 0, 1.0, tweenBoxHeight-5, 5, ofxeasing::easing(easingFunctions[i].easing, ofxeasing::Type::InOut));
            easingFunctions[i].easeInOutPreview.addVertex(ofPoint(p, percent));
        }
        
        easingFunctions[i].easeInPreview.simplify();
        easingFunctions[i].easeOutPreview.simplify();
        easingFunctions[i].easeInOutPreview.simplify();
        
    }
    
}
