//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "OpenCVNodes.h"

#include "OpenCVNodeGraph.h"
#include "OpenCVNode.h"

#undef AddVar
#define AddVar ComponentBase::AddVariable_Base

//====================================================================================================
// Macros to emit code.
//====================================================================================================

#define Emit(numTabs, ...) \
do \
{ \
    for( int i=0; i<static_cast<int>( numTabs ); i++ ) \
    { \
        offset += sprintf_s( &string[offset], bytesAllocated - offset, "\t" ); \
    } \
    offset += sprintf_s( &string[offset], bytesAllocated - offset, __VA_ARGS__ ); \
} while( false )

#define EmitNode(node, numTabs) \
do \
{ \
    offset += node->EmitLua( string, offset, bytesAllocated, numTabs ); \
} \
while ( false )

char* EmitNodeTemp(OpenCVNode* pNode)
{
    char* temp = static_cast<char*>( g_pEngineCore->GetSingleFrameMemoryStack()->AllocateBlock( 32 ) );
    temp[0] = '\0';
    pNode->EmitLua( temp, 0, 32, 0 );
    return temp;
};

//====================================================================================================
// Nodes and overrides for emitting Lua script.
//====================================================================================================

uint32 OpenCVNode_Value_Float::EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth)
{
    int startOffset = offset;
    Emit( 0, "(%f)", m_Float );
    return offset - startOffset;
}

//====================================================================================================
//====================================================================================================

uint32 OpenCVNode_MathOp_Add::EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth)
{
    int startOffset = offset;

    OpenCVNode* pNode1 = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 );
    OpenCVNode* pNode2 = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToInput( m_ID, 1 );

    if( pNode1 == nullptr || pNode2 == nullptr )
    {
        const char* errorMessage = "ERROR_MathOp_Add";
        Emit( 0, errorMessage );
        return (uint32)strlen( errorMessage );
    }

    char* node1String = EmitNodeTemp( pNode1 );
    char* node2String = EmitNodeTemp( pNode2 );
    Emit( 0, "( %s + %s )", node1String, node2String );

    return offset - startOffset;
}

//====================================================================================================
//====================================================================================================

uint32 OpenCVNode_Condition_GreaterEqual::EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth)
{
    int startOffset = offset;

    OpenCVNode* pNode1 = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToInput( m_ID, 1 );
    OpenCVNode* pNode2 = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToInput( m_ID, 2 );

    if( pNode1 == nullptr || pNode2 == nullptr )
    {
        const char* errorMessage = "ERROR_Condition_GreaterEqual";
        Emit( 0, errorMessage );
        return (uint32)strlen( errorMessage );
    }

    // Always emit an "if( condition ) then" block.
    {
        char* node1String = EmitNodeTemp( pNode1 );
        char* node2String = EmitNodeTemp( pNode2 );
        Emit( tabDepth, "if( %s <= %s ) then\n", node1String, node2String );

        int count = 0;
        while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
        {
            EmitNode( pNode, tabDepth+1 );
        }
    }

    // If there are any nodes connected to the 2nd output, then emit an else block.
    if( m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0 ) )
    {
        Emit( tabDepth, "else\n" );

        int count = 0;
        while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 1, count++ ) )
        {
            EmitNode( pNode, tabDepth+1 );
        }
    }

    Emit( tabDepth, "end\n" );

    return offset - startOffset;
}

//====================================================================================================
//====================================================================================================

uint32 OpenCVNode_Condition_Keyboard::EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth)
{
    int startOffset = offset;

    Emit( tabDepth, "if( action == %s and keyCode == %d ) then\n", g_GameCoreButtonActionLuaStrings[m_ButtonAction], m_KeyCode );

    int count = 0;
    while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
    {
        EmitNode( pNode, tabDepth+1 );
    }

    Emit( tabDepth, "end\n" ); // end 'if' statement.

    return offset - startOffset;
}

//====================================================================================================
//====================================================================================================

uint32 OpenCVNode_Event_Keyboard::ExportAsLuaString(char* string, uint32 offset, uint32 bytesAllocated)
{
    int startOffset = offset;

    uint32 tabDepth = 0;

    Emit( tabDepth, "OnKeys = function(action, keyCode)\n" );

    int count = 0;
    while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
    {
        EmitNode( pNode, tabDepth+1 );
    }

    Emit( tabDepth, "end,\n" ); // end function.

    return offset - startOffset;
}

//====================================================================================================
//====================================================================================================

uint32 OpenCVNode_Disable_GameObject::ExportAsLuaVariablesString(char* string, uint32 offset, uint32 bytesAllocated)
{
    if( m_pGameObject == nullptr )
        return 0;

    int startOffset = offset;

    uint32 tabDepth = 1;

    Emit( tabDepth, "this.GO_%s = g_pComponentSystemManager:FindGameObjectByName( \"%s\" );\n", m_pGameObject->GetName(), m_pGameObject->GetName() );

    return offset - startOffset;
}

uint32 OpenCVNode_Disable_GameObject::EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth)
{
    int startOffset = offset;

    if( m_pGameObject == nullptr )
    {
        const char* errorMessage = "ERROR_Disable_GameObject";
        Emit( 0, errorMessage );
        return (uint32)strlen( errorMessage );
    }

    Emit( tabDepth, "this.GO_%s:SetEnabled( not this.GO_%s:IsEnabled() );\n", m_pGameObject->GetName(), m_pGameObject->GetName() );

    return offset - startOffset;
}
