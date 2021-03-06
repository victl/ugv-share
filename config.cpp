#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glog/logging.h>

bool UgvParam::loadParam(std::string configFile)
{
    std::ifstream in(configFile);
    if(!in)
    {
        DLOG(FATAL) << "Couldn't find configuration file: " << configFile;
        return false;
    }
    std::string line;
    std::string key;
    double value;
    while (getline(in, line)) {
        std::stringstream ss;
        ss << line;
        ss >> key;
        if(key[0] == '#')
            continue;
        ss >> value;
        if(key == "EulrChangeThreshold")
            DivideCarTrack.EulrChangeThreshold = value;
        else if(key == "DetectPoints")
            DivideCarTrack.DetectPoints = value;
        else if(key == "DetectDistance")
            DivideCarTrack.DetectDistance = value;
        else if(key == "ValidSegmentPointsNum")
            DivideCarTrack.ValidSegmentPointsNum = value;
        else if(key == "SimilarEulrThreshold")
            LineParallel.SimilarEulrThreshold = value;
        else if(key == "LateralDistanceThreshold")
            SameSeg.LateralDistanceThreshold = value;
        else if(key == "SameDirectionThreshold")
            SameSeg.SameDirectionThreshold = value;
        else if(key == "xMax")
            Scale.xMax = value;
        else if(key == "xMin")
            Scale.xMin = value;
        else if(key == "yMax")
            Scale.yMax = value;
        else if(key == "yMin")
            Scale.yMin = value;
        else if(key == "GridSize")
            Scale.GridSize = value;
        else if(key == "PixelPerGrid")
            Scale.PixelPerGrid = value;
        else if(key == "LeftDetectAngleBoundary")
            ProbMap.LeftDetectAngleBoundary = value;
        else if(key == "RightDetectAngleBoundary")
            ProbMap.RightDetectAngleBoundary = value;
        else if(key == "unitHeight")
            ProbMap.unitHeight = value;
        else if(key == "OccupiedThreshold")
            ProbMap.OccupiedThreshold = value;
        else if(key == "ClearThreshold")
            ProbMap.ClearThreshold = value;
        else if(key == "incrementUnit")
            ProbMap.incrementUnit = value;
        else if(key == "MaxGroundHeight")
            ProbMap.MaxGroundHeight = value;
        else if(key == "MaxAvgMidDiff")
            ProbMap.MaxAvgMidDiff = value;
        else if(key == "SaveNeeded"){
            LocalMap.SaveNeeded.insert(value);
            while(ss){
                ss >> value;
                LocalMap.SaveNeeded.insert(value);
            }
        }
        else if(key == "SaveInterval")
            LocalMap.SaveInterval = value;
        //final case: unknown param:
        else{
            DLOG(FATAL) << "Error while reading configuration file: " << configFile;
            DLOG(FATAL) << "Unknown paramter: " << key <<", value: " << value;
            in.close();
            return false;
        }
        update();
    }
    in.close();
    return true;
}

void UgvParam::restoreDefault()
{
    //Ugv
    Ugv.CorrectionFile = "new_xml.txt";
    //DivideCarTrack
    DivideCarTrack.EulrChangeThreshold = 0.005;
    DivideCarTrack.DetectPoints = 10;
    DivideCarTrack.DetectDistance = 5;
    DivideCarTrack.ValidSegmentPointsNum = 80;

    //LineParallel
    LineParallel.SimilarEulrThreshold = 0.01;

    //SameSeg
    SameSeg.LateralDistanceThreshold = 20;
    SameSeg.SameDirectionThreshold = 0.01;

    //Scale
    Scale.xMax = 5;
    Scale.xMin = -5;
    Scale.yMax = 10;
    Scale.yMin = 0;
    Scale.GridSize = 0.08;
    Scale.PixelPerGrid = 4;

    //ProbMap
    ProbMap.LeftDetectAngleBoundary = 34000;
    ProbMap.RightDetectAngleBoundary = 2000;
    ProbMap.unitHeight = 100;
    ProbMap.HeightThreshold = 250;
    ProbMap.incrementUnit = 0.05;
    ProbMap.ClearThreshold = 0.3;
    ProbMap.OccupiedThreshold = 0.7;
    ProbMap.MaxGroundHeight = -1600; //-1.6 meters
    ProbMap.MaxAvgMidDiff = 0.25;

    //LocalMap
    LocalMap.SaveInterval = 0;

    update();


}


void UgvParam::update()
{
    Scale.Width = (Scale.xMax - Scale.xMin)/Scale.GridSize;
    Scale.xScale = 1/Scale.GridSize;
    Scale.yScale = 1/Scale.GridSize;
    //LocalMap
    // n times of Scale.Width
    LocalMap.initialHeight = 3 * Scale.Width;
    LocalMap.initialWidth = 3 * Scale.Width;
    LocalMap.ExpandUnit = 3 * (Scale.xMax - Scale.xMin);
}
