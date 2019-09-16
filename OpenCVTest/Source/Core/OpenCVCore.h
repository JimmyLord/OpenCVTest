//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVCore_H__
#define __OpenCVCore_H__

class ImGuiManager;
class OpenCVNodeGraph;
class OpenCVNodeTypeManager;

class OpenCVCore : public GameCore
{
public:
    ImGuiManager* m_pImGuiManager;
    OpenCVNodeGraph* m_pNodeGraph;
    OpenCVNodeTypeManager* m_pNodeTypeManager;

public:
    OpenCVCore();
    virtual ~OpenCVCore();

    // Initialization.
    virtual void OneTimeInit() override;

    // Input/Event handling.
    virtual void OnFocusLost() override;
    virtual bool OnTouch(int action, int id, float x, float y, float pressure, float size) override;
    virtual bool OnButtons(GameCoreButtonActions action, GameCoreButtonIDs id) override;

    // Update/Draw.
    virtual float Tick(float deltaTime) override;
    virtual void OnDrawFrame(unsigned int canvasid) override;
};

#endif //__OpenCVCore_H__
