//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//

// Framework Headers
#include "Libraries/Framework/MyFramework/SourceCommon/MyFramework.h"
#include "Libraries/Engine/MyEngine/SourceCommon/MyEngine.h"

// OpenCV headers, with the warnings disabled that are enabled by the framework headers.
#pragma warning( push )
#pragma warning( disable : 4946 ) // reinterpret_cast used between related classes: 'class1' and 'class2'
#pragma warning( disable : 4623 ) // 'derived class': default constructor could not be generated because a base class default constructor is inaccessible
#pragma warning( disable : 4263 ) // member function does not override any base class virtual member function
#pragma warning( disable : 4264 ) // no override available for virtual member function from base 'class'; function is hidden
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#pragma warning( pop )
