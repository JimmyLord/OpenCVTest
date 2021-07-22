//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "OpenCVNodes_Generators.h"
#include "OpenCVNodeTypeManager.h"

#include "OpenCVNodeGraph.h"
#include "OpenCVNodes_Base.h"
#include "OpenCVNodes_Core.h"
#include "OpenCVNodes_Face.h"
#include "OpenCVNodes_Mask.h"

#undef AddVar
#define AddVar ComponentBase::AddVariable_Base

OpenCVNodeTypeManager::OpenCVNodeTypeManager()
: OpenCVBaseNodeTypeManager()
{
}

MyNodeGraph::MyNode* OpenCVNodeTypeManager::AddCreateNodeItemsToContextMenu(Vector2 pos, MyNodeGraph* pNodeGraph)
{
    if( ImGui::BeginMenu( "File" ) )
    {
        if( ImGui::MenuItem( "Input" ) )     { ImGui::EndMenu(); return CreateNode( "File_Input", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Output" ) )    { ImGui::EndMenu(); return CreateNode( "File_Output", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Generate" ) )
    {
        if( ImGui::MenuItem( "PoissonSampling" ) ) { ImGui::EndMenu(); return CreateNode( "Generate_PoissonSampling", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Convert" ) )
    {
        if( ImGui::MenuItem( "Crop" ) )      { ImGui::EndMenu(); return CreateNode( "Convert_Crop", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Grayscale" ) ) { ImGui::EndMenu(); return CreateNode( "Convert_Grayscale", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Filter" ) )
    {
        if( ImGui::MenuItem( "Mask" ) )          { ImGui::EndMenu(); return CreateNode( "Filter_Mask", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Threshold" ) )     { ImGui::EndMenu(); return CreateNode( "Filter_Threshold", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Bilateral" ) )     { ImGui::EndMenu(); return CreateNode( "Filter_Bilateral", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Morphological" ) ) { ImGui::EndMenu(); return CreateNode( "Filter_Morphological", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Face" ) )
    {
        if( ImGui::MenuItem( "Detect" ) )    { ImGui::EndMenu(); return CreateNode( "Face_Detect", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    return nullptr;
}

MyNodeGraph::MyNode* OpenCVNodeTypeManager::CreateNode(const char* typeName, Vector2 pos, MyNodeGraph* pNodeGraph)
{
    OpenCVNodeGraph::NodeID newNodeID = pNodeGraph->GetNextNodeIDAndIncrement();

#define TypeIs(name) strcmp( typeName, name ) == 0 )

    if( TypeIs( "File_Input" )                  return MyNew OpenCVNode_File_Input(                 (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Input", pos );
    if( TypeIs( "File_Output" )                 return MyNew OpenCVNode_File_Output(                (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Output", pos );
    if( TypeIs( "Generate_PoissonSampling" )    return MyNew OpenCVNode_Generate_PoissonSampling(   (OpenCVNodeGraph*)pNodeGraph, newNodeID, "PoissonSampling", pos );
    if( TypeIs( "Convert_Grayscale" )           return MyNew OpenCVNode_Convert_Grayscale(          (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Grayscale", pos );
    if( TypeIs( "Convert_Crop" )                return MyNew OpenCVNode_Convert_Crop(               (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Crop", pos );
    if( TypeIs( "Filter_Mask" )                 return MyNew OpenCVNode_Filter_Mask(                (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Mask", pos );
    if( TypeIs( "Filter_Threshold" )            return MyNew OpenCVNode_Filter_Threshold(           (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Threshold", pos );
    if( TypeIs( "Filter_Bilateral" )            return MyNew OpenCVNode_Filter_Bilateral(           (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Bilateral", pos );
    if( TypeIs( "Filter_Morphological" )        return MyNew OpenCVNode_Filter_Morphological(       (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Morph", pos );
    if( TypeIs( "Face_Detect" )                 return MyNew OpenCVNode_Face_Detect(                (OpenCVNodeGraph*)pNodeGraph, newNodeID, "FaceDetect", pos );

#undef TypeIs

    LOGInfo( LOGTag, "NodeType not found: %s", typeName );

    return nullptr;
}
