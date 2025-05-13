#pragma once

// Forward
class SceneObject;
class BaseEditor;

class SelectionManager{
public:
    static SelectionManager* GetInstance(){
        static SelectionManager instance;
        return &instance;
    }

    // 選択された SceneObject を登録
    void SetSelectedObject(SceneObject* obj);

    // 現在選択されている SceneObject を取得
    SceneObject* GetSelectedObject() const;

    // 選択されたエディタを登録
    void SetSelectedEditor(BaseEditor* editor);

    // 現在選択されているエディタを取得
    BaseEditor* GetSelectedEditor() const;

    void ClearSelectedObject();
    void ClearSelectedEditor();

private:
    // シングルトンのためコンストラクタ等を private に
    SelectionManager() = default;
    ~SelectionManager() = default;

    SelectionManager(const SelectionManager&) = delete;
    SelectionManager& operator=(const SelectionManager&) = delete;

private:
    // 現在選択されている SceneObject
    SceneObject* selectedObject_ = nullptr;

    // 現在選択されている Editor
    BaseEditor* selectedEditor_ = nullptr;
};
