#pragma once

#include "pebble_os.h"


#define SIZE 3
#define HR_LEN 25
#define MIN_LEN 32

//static const GPathInfo HOUR_HAND_POINTS = {
//  4, (GPoint []){
//    {-SIZE, SIZE},
//    {SIZE, SIZE},
//    {SIZE, -HR_LEN},
//    {-SIZE, -HR_LEN},
//  }
//};

//static const GPathInfo MINUTE_HAND_POINTS = {
 // 4, (GPoint []) {
 //   { -SIZE, SIZE },
 //   { SIZE, SIZE },
 //   { SIZE, -MIN_LEN },
 //   { -SIZE, -MIN_LEN },
 // }
//};


const GPathInfo HOUR_HAND_POINTS = {
	4,
	(GPoint []) {
		{-3, 3},
		{3, 3},
		{3, -25},
		{-3,  -25},
	}
};



const GPathInfo MINUTE_HAND_POINTS = {
	4,
	(GPoint []) {
		{-3, 3},
		{3, 3},
		{3, -30},
		{-3,  -30},
	}
};

