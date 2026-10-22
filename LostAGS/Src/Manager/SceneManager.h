// Src/Manager/SceneManager.h
#pragma once
#include <memory>
#include <chrono>

#define mainCamera SceneManager::GetInstance().GetCamera()

class SceneBase;
class Fader;
class Camera;

class SceneManager
{

public:

	// シーン管理用（★ CLEAR を追加）
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		GAME,
		CLEAR
	};

	static void CreateInstance(void);
	static SceneManager& GetInstance(void);

	void Init(void);
	void Init3D(void);
	void Update(void);
	void Draw(void);

	void Destroy(void);

	void ChangeScene(SCENE_ID nextId);

	SCENE_ID GetSceneID(void);

	float GetDeltaTime(void) const;

	Camera& GetCamera(void);

private:

	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;

	std::unique_ptr<SceneBase> scene_;
	std::unique_ptr<Fader> fader_;
	std::unique_ptr<Camera> camera_;

	bool isSceneChanging_;

	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;

	SceneManager(void);
	SceneManager(const SceneManager& manager) = default;
	~SceneManager(void) = default;

	void ResetDeltaTime(void);
	void DoChangeScene(SCENE_ID sceneId);
	void Fade(void);

};