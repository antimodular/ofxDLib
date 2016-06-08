//
//  FaceTracker.h
//  example_FaceTracker
//
//  Created by Roy Macdonald on 15-01-16.
//
//

#pragma once
#include "ofxDLib.h"
#include "Tracker.h"

namespace ofxDLib {
    
    enum ShapeType {
        leftEye, rightEye, innerMouth, outerMouth, leftEyebrow, rightEyebrow, jaw, noseBridge, noseTip
    };
    
    enum DrawStyle {
        none, lines, circles
    };
    
    typedef struct {
        int label;
        int age;
        ofRectangle rect;
        ofVec2f velocity, leftEyeCenter, rightEyeCenter,upperLipCenter;
        ofPolyline leftEye, rightEye, innerMouth, outerMouth, leftEyebrow, rightEyebrow, jaw, noseBridge, noseTip;
        vector<ofVec3f> landmarks;
    } Face;
    
    float trackingDistance(const Face& a, const Face& b) {
        ofVec3f aCenter = a.rect.getCenter();
        ofVec3f bCenter = b.rect.getCenter();
        return aCenter.distance(bCenter);
    };
    
    class ShapeTracker : public Tracker<Face> {
    protected:
        float initSmoothingRate;
        std::map<unsigned int, float> smoothingRate;
        std::map<unsigned int, Face> smoothed;
    public:
        ShapeTracker()
        :initSmoothingRate(.5) {
        }
        void setSmoothingRate(float smoothingRate) {
            this->initSmoothingRate = smoothingRate;
        }
        void setSmoothingRate(unsigned int label, float smoothingRate) {
            this->smoothingRate[label] = smoothingRate;
        }
        float getSmoothingRate() const {
            return initSmoothingRate;
        }
        float getSmoothingRate(unsigned int label) {
            if (this->smoothingRate.count(label) > 0) return this->smoothingRate[label];
            else return initSmoothingRate;
        }
        const std::vector<unsigned int>& track(const std::vector<Face>& objects) {
            const std::vector<unsigned int>& labels = Tracker<Face>::track(objects);
            // add new objects, update old objects
            for(int i = 0; i < labels.size(); i++) {
                unsigned int label = labels[i];
                const Face& cur = getCurrent(label);
                const float smoothingRateCur = getSmoothingRate(label);
                if(smoothed.count(label) > 0) {
//                    ofLogNotice("facetracker.h")<<"smoothingRateCur "<<smoothingRateCur;
                    Face& smooth = smoothed[label];
                    smooth.rect.x = ofLerp(smooth.rect.x, cur.rect.x, smoothingRateCur);
                    smooth.rect.y = ofLerp(smooth.rect.y, cur.rect.y, smoothingRateCur);
                    smooth.rect.width = ofLerp(smooth.rect.width, cur.rect.width, smoothingRateCur);
                    smooth.rect.height = ofLerp(smooth.rect.height, cur.rect.height, smoothingRateCur);
                    for (int j=0; j<smooth.landmarks.size(); j++) {
                        ofVec3f & smoothLandmark = smooth.landmarks[j];
                        const ofVec3f & curLandmark = cur.landmarks[j];
                        smoothLandmark.x = ofLerp(smoothLandmark.x, curLandmark.x, smoothingRateCur);
                        smoothLandmark.y = ofLerp(smoothLandmark.y, curLandmark.y, smoothingRateCur);
                    }
                } else {
                    smoothingRate[label] = initSmoothingRate;
                    smoothed[label] = cur;
                }
            }
            std::map<unsigned int, Face>::iterator smoothedItr = smoothed.begin();
            while(smoothedItr != smoothed.end()) {
                unsigned int label = smoothedItr->first;
                if(!existsCurrent(label)) {
                    smoothed.erase(smoothedItr++);
                    smoothingRate.erase(label);
                } else {
                    ++smoothedItr;
                }
            }
            return labels;
        }
        const Face& getSmoothed(unsigned int label) const {
            return smoothed.find(label)->second;
        }
        ofVec2f getVelocity(unsigned int i) const {
            unsigned int label = getLabelFromIndex(i);
            if(existsCurrent(label) && existsPrevious(label)) {
                const Face& previous = getPrevious(label);
                const Face& current = getCurrent(label);
                
                const float prvX = previous.rect.x;
                const float prvY = previous.rect.y;
                const float prvW = previous.rect.width;
                const float prvH = previous.rect.height;
                const float pX = prvX + (prvW / 2.0);
                const float pY = prvY + (prvH / 2.0);
                ofVec2f previousPosition(pX, pY);
                
                const float curX = current.rect.x;
                const float curY = current.rect.y;
                const float curW = current.rect.width;
                const float curH = current.rect.height;
                const float cX = curX + (curW / 2.0);
                const float cY = curY + (curH / 2.0);
                ofVec2f currentPosition(cX, cY);
                
                return currentPosition - previousPosition;
            } else {
                return ofVec2f(0, 0);
            }
        }
    };
    
    class FaceTracker {
    protected:
        // face tracker
        dlib::frontal_face_detector detector;
        dlib::shape_predictor predictor;

        map<unsigned int, vector<ofVec3f>> shapeHistory;
        map<unsigned int, float> smoothingRatePerFace;
        float smoothingRate;
        DrawStyle drawStyle;
        ShapeTracker tracker;
        
        Face& assignFeatures(Face & face);
        
        int dlibImg_width, dlibImg_height;
        ofPixels dlibImg;
    public:
        FaceTracker();
        void setup(string predictorDatFilePath);
        void findFaces(const ofPixels& pixels, bool bUpscale = false);
        unsigned int size();
        ShapeTracker& getTracker();
        Face getFace(unsigned int i);
        vector<Face> getFaces();
        ofRectangle getRectangle(unsigned int i);
        vector<ofVec3f> getLandmarks(unsigned int i);
        ofPolyline getShape(unsigned int i, ShapeType t);
        unsigned int getLabel(unsigned int i);
        int getIndexFromLabel(unsigned int label);
        ofVec2f getVelocity(unsigned int i);
        void setSmoothingRate(float smoothingRate);
        void setSmoothingRate(unsigned int label, float smoothingRate);
        float getSmoothingRate();
        float getSmoothingRate(unsigned int label);
        void setDrawStyle(DrawStyle style);
        void draw();
        int getWidth();
        int getHeight();
        ofPixels getPixels();
    };
    
    
    template <class F>
    class FaceTrackerFollower : public FaceTracker {
    protected:
        std::vector<unsigned int> labels;
        std::vector<F> followers;
    public:
        FaceTrackerFollower() : FaceTracker() {};
        const std::vector<unsigned int>& findFaces(const ofPixels& pixels, bool bUpscale = false) {
            FaceTracker::findFaces(pixels);
            ShapeTracker & tracker = FaceTracker::getTracker();
            
            // kill missing, update old
            for(int i = 0; i < labels.size(); i++) {
                unsigned int curLabel = labels[i];
                F& curFollower = followers[i];
                if(!tracker.existsCurrent(curLabel)) {
                    curFollower.kill();
                } else {
                    Face face = tracker.getSmoothed(curLabel);
                    face.label = curLabel;
                    face.age = tracker.getAge(curLabel);
                    face.velocity = tracker.getVelocity(i);
                    face = FaceTracker::assignFeatures(face);
                    
                    curFollower.update(face);
                }
            }
            // add new
            int index = 0;
            for(auto & curLabel : tracker.getNewLabels()) {
                Face face = tracker.getSmoothed(curLabel);
                face.label = curLabel;
                face.age = tracker.getAge(curLabel);
                face.velocity = tracker.getVelocity(index);
                face = FaceTracker::assignFeatures(face);
                
                labels.push_back(curLabel);
                followers.push_back(F());
                followers.back().setup(face);
                followers.back().setLabel(curLabel);
                
                index++;
            }
            // remove dead
            for(int i = labels.size() - 1; i >= 0; i--) {
                if(followers[i].getDead()) {
                    followers.erase(followers.begin() + i);
                    labels.erase(labels.begin() + i);
                }
            }
            return labels;
        };
        std::vector<F>& getFollowers() {
            return followers;
        };
    };
    
}