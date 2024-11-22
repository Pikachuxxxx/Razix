#pragma once

class NodeScene;

class SceneClipboard
{
public:
    SceneClipboard(NodeScene* scene) : m_Scene(scene) {}
    ~SceneClipboard() {}



private:
    NodeScene* m_Scene;
};

