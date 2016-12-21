#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ft.setup("");
    video.setDeviceID(0);
//    video.setup(720, 480);
     video.setup(1280,720);
//     video.setup(1920,1080);
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    if(video.isFrameNew()){
        ft.findFaces(video.getPixels(),ofRectangle(0,0,video.getWidth(),video.getHeight()),false);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    video.draw(0, 0);
    ft.draw();
    
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10,10);
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'f')  ofToggleFullscreen();
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
