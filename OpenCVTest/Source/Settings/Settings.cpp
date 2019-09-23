//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "Settings.h"
#include "NodeGraph/OpenCVNodeTypeManager.h"

// Complete hack for now, but since I'm forking this project,
//    I moved this into a single file that will be customized in the other project to avoid merge issues
OpenCVNodeTypeManager* CreateNodeTypeManager()
{
    return MyNew OpenCVNodeTypeManager();
}