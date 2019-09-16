//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodes_H__
#define __OpenCVNodes_H__

#include "OpenCVNode.h"

class ComponentBase;

// Visual Script node types.
class OpenCVNode_Value_Float;
class OpenCVNode_Value_Color;
class OpenCVNode_Value_GameObject;
class OpenCVNode_Value_Component;
class OpenCVNode_MathOp_Add;
class OpenCVNode_Condition_GreaterEqual;
class OpenCVNode_Condition_Keyboard;
class OpenCVNode_Event_Keyboard;
class OpenCVNode_Disable_GameObject;

//====================================================================================================
// OpenCVNode
//====================================================================================================

class OpenCVNode : public OpenCVNodeGraph::OpenCVNode
{
public:
    OpenCVNode(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, int inputsCount, int outputsCount)
    : OpenCVNodeGraph::OpenCVNode( pNodeGraph, id, name, pos, inputsCount, outputsCount )
    {
    }

    virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) { return 0; }

    virtual float GetValueFloat() { return FLT_MAX; }
};

#define VSNAddVar ComponentBase::AddVariable_Base
#define VSNAddVarEnum ComponentBase::AddVariableEnum_Base

//====================================================================================================
// OpenCVNode_Value_Float
//====================================================================================================

class OpenCVNode_Value_Float : public OpenCVNode
{
protected:
    float m_Float;

public:
    OpenCVNode_Value_Float(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, float value)
    : OpenCVNode( pNodeGraph, id, name, pos, 0, 1 )
    {
        m_Float = value;
        VSNAddVar( &m_VariablesList, "Float", ComponentVariableType_Float, MyOffsetOf( this, &this->m_Float ), true, true, "", nullptr, nullptr, nullptr );
    }

    const char* GetType() { return "Value_Float"; }
    virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    virtual void DrawTitle() override
    {
        if( m_Expanded )
            OpenCVNode::DrawTitle();
        else
            ImGui::Text( "%s: %0.2f", m_Name, m_Float );
    }

    virtual float GetValueFloat() override { return m_Float; }
};

//====================================================================================================
// OpenCVNode_Value_Color
//====================================================================================================

class OpenCVNode_Value_Color : public OpenCVNode
{
protected:
    ColorByte m_Color;

public:
    OpenCVNode_Value_Color(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, const ColorByte& color)
    : OpenCVNode( pNodeGraph, id, name, pos, 0, 1 )
    {
        m_Color = color;
        VSNAddVar( &m_VariablesList, "Color", ComponentVariableType_ColorByte, MyOffsetOf( this, &this->m_Color ), true, true, "", nullptr, nullptr, nullptr );
    }

    const char* GetType() { return "Value_Color"; }

    virtual void DrawTitle() override
    {
        if( m_Expanded )
        {
            OpenCVNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
            ImGui::SameLine();
            ImVec4 color = *(ImVec4*)&m_Color.AsColorFloat();
            ImGui::ColorButton( "", color, ImGuiColorEditFlags_NoPicker );
        }
    }
};

//====================================================================================================
// OpenCVNode_Value_GameObject
//====================================================================================================

class OpenCVNode_Value_GameObject : public OpenCVNode
{
protected:
    GameObject* m_pGameObject;

public:
    OpenCVNode_Value_GameObject(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, GameObject* pGameObject)
    : OpenCVNode( pNodeGraph, id, name, pos, 0, 1 )
    {
        m_pGameObject = pGameObject;
        VSNAddVar( &m_VariablesList, "GameObject", ComponentVariableType_GameObjectPtr, MyOffsetOf( this, &this->m_pGameObject ), true, true, "",
            (CVarFunc_ValueChanged)&OpenCVNode_Value_GameObject::OnValueChanged,
            (CVarFunc_DropTarget)&OpenCVNode_Value_GameObject::OnDrop, nullptr );
    }

    const char* GetType() { return "Value_GameObject"; }

    void* OnDrop(ComponentVariable* pVar, bool changedByInterface, int x, int y)
    {
        DragAndDropItem* pDropItem = g_DragAndDropStruct.GetItem( 0 );

        if( pDropItem->m_Type == DragAndDropType_GameObjectPointer )
        {
            m_pGameObject = (GameObject*)pDropItem->m_Value;
        }

        return nullptr;
    }

    void* OnValueChanged(ComponentVariable* pVar, bool changedByInterface, bool finishedChanging, double oldValue, ComponentVariableValue* pNewValue)
    {
        return nullptr;
    }
};

//====================================================================================================
// OpenCVNode_Value_Component
//====================================================================================================

class OpenCVNode_Value_Component : public OpenCVNode
{
protected:
    ComponentBase* m_pComponent;

public:
    OpenCVNode_Value_Component(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, ComponentBase* pComponent)
    : OpenCVNode( pNodeGraph, id, name, pos, 0, 1 )
    {
        m_pComponent = pComponent;
        VSNAddVar( &m_VariablesList, "Component", ComponentVariableType_ComponentPtr, MyOffsetOf( this, &this->m_pComponent ), true, true, "",
            (CVarFunc_ValueChanged)&OpenCVNode_Value_Component::OnValueChanged,
            (CVarFunc_DropTarget)&OpenCVNode_Value_Component::OnDrop, nullptr );
    }

    const char* GetType() { return "Value_Component"; }

    void* OnDrop(ComponentVariable* pVar, bool changedByInterface, int x, int y)
    {
        DragAndDropItem* pDropItem = g_DragAndDropStruct.GetItem( 0 );

        if( pDropItem->m_Type == DragAndDropType_ComponentPointer )
        {
            m_pComponent = (ComponentBase*)pDropItem->m_Value;
        }

        return nullptr;
    }

    void* OnValueChanged(ComponentVariable* pVar, bool changedByInterface, bool finishedChanging, double oldValue, ComponentVariableValue* pNewValue)
    {
        return nullptr;
    }
};

//====================================================================================================
// OpenCVNode_MathOp_Add
//====================================================================================================

class OpenCVNode_MathOp_Add : public OpenCVNode
{
protected:
public:
    OpenCVNode_MathOp_Add(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
    : OpenCVNode( pNodeGraph, id, name, pos, 2, 1 ) {}

    const char* GetType() { return "MathOp_Add"; }
    virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    virtual void DrawContents() override
    {
        OpenCVNode::DrawContents();

        ImGui::Text( "+" );
    }

    virtual float GetValueFloat() override
    {
        OpenCVNode* pNode1 = static_cast<OpenCVNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        OpenCVNode* pNode2 = static_cast<OpenCVNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 1 ) );

        if( pNode1 == nullptr || pNode2 == nullptr )
            return FLT_MAX;

        float value1 = pNode1->GetValueFloat();
        float value2 = pNode2->GetValueFloat();

        return value1 + value2;
    }
};

//====================================================================================================
// OpenCVNode_OpenCVNode_Condition_GreaterEqual
//====================================================================================================

class OpenCVNode_Condition_GreaterEqual : public OpenCVNode
{
protected:
public:
    OpenCVNode_Condition_GreaterEqual(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
    : OpenCVNode( pNodeGraph, id, name, pos, 3, 2 ) {}

    const char* GetType() { return "Condition_GreaterEqual"; }
    virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    virtual void DrawContents() override
    {
        OpenCVNode::DrawContents();

        ImGui::Text( ">=" );
    }

    virtual bool Trigger(MyEvent* pEvent) override
    {
        OpenCVNode* pNode1 = static_cast<OpenCVNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 1 ) );
        OpenCVNode* pNode2 = static_cast<OpenCVNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 2 ) );

        if( pNode1 == nullptr || pNode2 == nullptr )
            return false;

        float value1 = pNode1->GetValueFloat();
        float value2 = pNode2->GetValueFloat();

        if( value1 >= value2 )
        {
            int count = 0;
            while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
            {
                pNode->Trigger();
            }
        }
        else
        {
            int count = 0;
            while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 1, count++ ) )
            {
                pNode->Trigger();
            }
        }

        return false;
    }
};

//====================================================================================================
// OpenCVNode_Condition_Keyboard
//====================================================================================================

class OpenCVNode_Condition_Keyboard : public OpenCVNode
{
protected:
    int m_ButtonAction;
    int m_KeyCode;

public:
    OpenCVNode_Condition_Keyboard(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, int buttonAction, int keyCode)
    : OpenCVNode( pNodeGraph, id, name, pos, 1, 1 )
    {
        MyAssert( GCBA_Down == 0 );
        MyAssert( GCBA_Up == 1 );
        MyAssert( GCBA_Held == 2 );

        m_ButtonAction = buttonAction;
        m_KeyCode = keyCode;

        VSNAddVarEnum( &m_VariablesList, "Action", MyOffsetOf( this, &this->m_ButtonAction ), true, true, "", 3, g_GameCoreButtonActionStrings, nullptr, nullptr, nullptr );
        VSNAddVar( &m_VariablesList, "KeyCode", ComponentVariableType_Int, MyOffsetOf( this, &this->m_KeyCode ), true, true, "", nullptr, nullptr, nullptr );
    }

    const char* GetType() { return "Condition_Keyboard"; }
    virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    virtual void DrawTitle() override
    {
        if( m_Expanded )
            OpenCVNode::DrawTitle();
        else
            ImGui::Text( "%s: %c %s", m_Name, m_KeyCode, g_GameCoreButtonActionStrings[m_ButtonAction] );
    }

    virtual void DrawContents() override
    {
        OpenCVNode::DrawContents();

        ImGui::Text( "Key: %c", m_KeyCode );
    }

    virtual bool Trigger(MyEvent* pEvent) override
    {
        MyAssert( pEvent->IsType( "Keyboard" ) );

        int action = pEvent->GetInt( "Action" );
        int keyCode = pEvent->GetInt( "KeyCode" );

        if( action == m_ButtonAction && keyCode == m_KeyCode )
        {
            int count = 0;
            while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
            {
                if( pNode->Trigger() == true )
                    return true;
            }

            return true;
        }

        return false;
    }
};

//====================================================================================================
// OpenCVNode_Event_Keyboard
//====================================================================================================

class OpenCVNode_Event_Keyboard : public OpenCVNode
{
protected:
    EventManager* m_pEventManager;

public:
    OpenCVNode_Event_Keyboard(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, EventManager* pEventManager)
    : OpenCVNode( pNodeGraph, id, name, pos, 0, 1 )
    {
        m_pEventManager = pEventManager;

        // Don't allow node graph to be triggered directly.
        // This will now get triggered through lua script when attached to an object.
        m_pEventManager->RegisterForEvents( "Keyboard", this, &OpenCVNode_Event_Keyboard::StaticOnEvent );
    }

    ~OpenCVNode_Event_Keyboard()
    {
        m_pEventManager->UnregisterForEvents( "Keyboard", this, &OpenCVNode_Event_Keyboard::StaticOnEvent );
    }

    const char* GetType() { return "Event_Keyboard"; }
    virtual uint32 ExportAsLuaString(char* string, uint32 offset, uint32 bytesAllocated) override;

    virtual void DrawTitle() override
    {
        OpenCVNode::DrawTitle();
    }

    virtual void DrawContents() override
    {
        OpenCVNode::DrawContents();
    }

    static bool StaticOnEvent(void* pObjectPtr, MyEvent* pEvent) { return ((OpenCVNode_Event_Keyboard*)pObjectPtr)->OnEvent( pEvent ); }
    bool OnEvent(MyEvent* pEvent)
    {
        MyAssert( pEvent->IsType( "Keyboard" ) );

        int count = 0;
        while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
        {
            if( pNode->Trigger( pEvent ) == true )
                return true;
        }

        return false;
    }
};

//====================================================================================================
// OpenCVNode_Disable_GameObject
//====================================================================================================

class OpenCVNode_Disable_GameObject : public OpenCVNode
{
protected:
    GameObject* m_pGameObject;

public:
    OpenCVNode_Disable_GameObject(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, GameObject* pGameObject)
    : OpenCVNode( pNodeGraph, id, name, pos, 1, 0 )
    {
        m_pGameObject = pGameObject;
        VSNAddVar( &m_VariablesList, "GameObject", ComponentVariableType_GameObjectPtr, MyOffsetOf( this, &this->m_pGameObject ), true, true, "",
            (CVarFunc_ValueChanged)&OpenCVNode_Disable_GameObject::OnValueChanged,
            (CVarFunc_DropTarget)&OpenCVNode_Disable_GameObject::OnDrop, nullptr );
    }

    const char* GetType() { return "Disable_GameObject"; }
    virtual uint32 ExportAsLuaVariablesString(char* string, uint32 offset, uint32 bytesAllocated) override;
    virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    virtual void DrawTitle() override
    {
        if( m_Expanded )
        {
            OpenCVNode::DrawTitle();
        }
        else
        {
            if( m_pGameObject )
                ImGui::Text( "%s: %s", m_Name, m_pGameObject->GetName() );
            else
                ImGui::Text( "%s: not set" );
        }
    }

    void* OnDrop(ComponentVariable* pVar, bool changedByInterface, int x, int y)
    {
        DragAndDropItem* pDropItem = g_DragAndDropStruct.GetItem( 0 );

        if( pDropItem->m_Type == DragAndDropType_GameObjectPointer )
        {
            m_pGameObject = (GameObject*)pDropItem->m_Value;
        }

        return nullptr;
    }

    void* OnValueChanged(ComponentVariable* pVar, bool changedByInterface, bool finishedChanging, double oldValue, ComponentVariableValue* pNewValue)
    {
        return nullptr;
    }

    virtual bool Trigger(MyEvent* pEvent) override
    {
        if( m_pGameObject )
        {
            m_pGameObject->SetEnabled( !m_pGameObject->IsEnabled(), true );
        }

        return false;
    }
};

#endif //__OpenCVNodes_H__
