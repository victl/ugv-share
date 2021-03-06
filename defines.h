/**
  * This header file contains many preprocessor defines and some fundamental types.
  * Most of them do not change unless the physical model of HDL Lader changed.
  * Adjustable parameters should be put in config.h file.
  *
  * Author: Zou Lu (victl@163.com)
  * Date: 2015-9-1
  */
#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include <glog/logging.h>
#include "config.h"

#ifndef DEFINES_H
#define DEFINES_H

//define some constant
#define ANGLE_NUM 36000 //divide the whole 360 degree to small pieces
#define LASER_NUM 64    //the total num of laser beam from the ladar is 64
#define MAX_CLOUD_NUM 256000    //maximum possible point number

//struct used to store raw hdl data
typedef struct
{
    unsigned short distance;    //corresponse to original 'dist'
    unsigned short rotAngle;   //corresponse to original 'rot'
    unsigned char intensity;    //corresponse to original 'i'
    unsigned char beamId;    //corresponse to original 'c'
}RawHdlPoint;

typedef struct{
    int x;
    int y;
    int z;
}HdlPointXYZ;

typedef struct{
    double x;
    double y;
    float eulr;
}Carpose;


//NOTE: "unsigned char" for enum is a new feature introduced in c++11 standard
//so you have to enable c++11 support of compilers.
enum PointType : unsigned char {
    LANELINE = 254,
    ZEBRA = 253,
    INTERSECTION = 3,
    CURB = 4,
    TREE = 210,
    TRUNK = 6,
    PIT = 252,
    LANECENTER = 8,
    CARTRACK = 128,
    TRAFFICSIGN = 255,
    DOTTEDLANELINE = 251,
    SOLIDLANELINE = 250,
    CLEAR = 249, //'CLEAR' means point could pass trough
    OCCUPIED = 0,
    UNKNOWN = 127, //'BLANK' means no specific type. Blank point on road should turn into 'CLEAR' point before '3b map file' output
};

enum MapType : unsigned char {
    DYNAMICMAP,//dynamic map
    ACCUMMAP,//accumulated map
    LOCALMAP,//local map
    THREEBIT,//3b format
};

//Point3B is used to output final grid map format (formerly .3b file, now has switched to .png)
struct Point3B {
public:
    //public member variable
    unsigned char base;
    unsigned char road;
    unsigned char sig;
    //public method:
    Point3B():
        base(0)
      , road(0)
      , sig(0)
    {
    }
};

//Grid type. It is used for storing dynamic map and accumulated map infos, but not for local map
typedef struct Grid_t{
    //the probability of occupation:
    float p;
    //highest 'z' value
    short highest;
    //lowest 'z' value
    short lowest;
    //the average height//testing
    short average;//testing
    //how many points in the grid
    unsigned short pointNum;
    unsigned char HitCount;
    //type. including occupation and other properties
    PointType type;

    Grid_t(){
        p = 0.5;
        highest = 0;
        lowest = 0;
        average = 0;//tesing
        pointNum = 0;
        HitCount = 0;
        type = UNKNOWN;
    }

    Grid_t &operator+=(const Grid_t& other);
}Grid;


//Range indicates the boundary of the map under process. The origin is at Differential GPS Station!
class Range{
public/*constructor && destructor && helper*/:
    explicit Range(const UgvParam& param);
    Range(const Carpose& currentPose, const UgvParam& param);

public/*data member*/:
    const UgvParam &params;
    double bottom;
    double top;
    double left;
    double right;
    unsigned short maxX;
    unsigned short maxY;

public/*function member*/:
    //if point (x,y) didn't fall into the range, return false
    inline bool contains(double x, double y);

    //translate global coordinate to local coordinate (origin at bottom-left corner of the range)
    //NOTE: the function changes x and y directly as they are passed as reference,
    //It returns false if the point fall outside the range
    bool toLocal(double &x, double &y);

    //translate local coordinate to global. Unlike toLocal(), this function leave x,y untouched (pass by value)
    //and returns the result coordinate as a cv::Point2d.
    //NOTE: grid's global position is on its mid-center
    cv::Point2d toGlobal(unsigned short x, unsigned short y);

    //translate (x,y) to another range 'other'. values is return by oldx, oldy. if (x,y) does not fall in oldRange, return false
    bool translate(unsigned short x, unsigned short y, Range& oldRange, unsigned short& oldx, unsigned short& oldy);

    //copy assignment operator
    Range &operator=(const Range& source);

    //update maxX, maxY whenever boundaries are adjusted explicitly (because they are 'public' members).
    //THIS IS VERY IMPORTANT!!!
    bool update();
};


//define binary values of Point3B.base
static const unsigned char ROADEDGE_UNKNOWN = 0;
static const unsigned char ROADEDGE_CLEAR = 64;
static const unsigned char ROADEDGE_OCCUPIED = 128;
static const unsigned char ROADEDGE_DYNAMIC = 192;
static const unsigned char OBSTACLE_NONE = 0;
static const unsigned char OBSTACLE_STATIC = 8;
static const unsigned char OBSTACLE_PEDESTRIAN = 16;
static const unsigned char OBSTACLE_BIKE = 24;
static const unsigned char OBSTACLE_MOTO = 32;
static const unsigned char OBSTACLE_CAR = 40;
static const unsigned char OBSTACLE_BUS = 48;
static const unsigned char OBSTACLE_TRUCK = 56;
static const unsigned char LANELINE_NONE = 0;
static const unsigned char LANELINE_DOTTED = 2;
static const unsigned char LANELINE_SOLID = 4;
static const unsigned char LANELINE_DOUBLE = 6;
static const unsigned char STOPLINE_NO = 0;
static const unsigned char STOPLINE_YES = 1;

//define binary values of Point3B.road
static const unsigned char CURB_NO = 0;
static const unsigned char CURB_YES = 128;
static const unsigned char FENCERAMP_NO = 0;
static const unsigned char FENCERAMP_CITY = 32;
static const unsigned char FENCERAMP_HIGHWAY = 64;
static const unsigned char FENCERAMP_RAMP = 96;
static const unsigned char REGION_STRUCTURED = 0;
static const unsigned char REGION_INTERSECTION = 8;
static const unsigned char REGION_UTURN = 16;
static const unsigned char REGION_RIM = 24;
static const unsigned char ARROW_NONE = 0;
static const unsigned char ARROW_STRAIGHT = 1;
static const unsigned char ARROW_LEFT = 2;
static const unsigned char ARROW_RIGHT = 3;
static const unsigned char ARROW_UTURN = 4;
static const unsigned char ARROW_STRAIGHTLEFT = 5;
static const unsigned char ARROW_STRAIGHTRIGHT = 6;

//define binary values of Point3B.sig
static const unsigned char LAMP_NONE = 0;
static const unsigned char LAMP_ROUND = 64;
static const unsigned char LAMP_ARROW = 128;
static const unsigned char LAMP_GROUND = 192;
//the definition of traffic sign is uncertain for the moment, to be added in the future


#endif // DEFINES_H

