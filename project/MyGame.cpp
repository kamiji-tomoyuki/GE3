#include "MyGame.h"

void MyGame::Initialize()
{
	// --- リークチェック ---
	//D3DResourceLeakChecker leakCheck;

#pragma region 基礎システムの初期化

	// WindowsAPI
	winApp = new WinApp();
	winApp->Initialize();

	// DirectX
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	// キーボード入力
	input = new Input();
	input->Initialize(winApp);

	// オーディオ
	audio = new Audio();
	audio->Initialize();

	// スプライト
	TextureManager::GetInstance()->Initialize(dxCommon);
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	// 3Dオブジェクト
	ModelManager::GetInstance()->Initialize(dxCommon);
	object3dCommon = new Object3dCommon();
	object3dCommon->Initialize(dxCommon);

#pragma endregion 基礎システムの初期化

	// --- カメラ ---
	camera = new Camera();
	camera->SetRotate({ 0.3f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,4.0f,-10.0f });
	object3dCommon->SetDefaultCamera(camera);

	// --- スプライト ---
	for (uint32_t i = 0; i < spriteNum; ++i) {
		Sprite* sprite = new Sprite();
		std::string textureFile = { "resources/images/uvChecker.png" };
		if (i % 2 == 1) {
			textureFile = { "resources/images/monsterBall.png" };
		}
		sprite->Initialize(spriteCommon, textureFile);
		sprites.push_back(sprite);
	}

	// --- 3Dオブジェクト ---
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	for (uint32_t i = 0; i < objectNum; ++i) {
		Object3d* object = new Object3d();
		object->Initialize(object3dCommon);

		Vector3 position;
		position.x = i * 2.0f;

		object->SetPosition(position);

		if (i == 0) {
			object->SetModel("plane.obj");
		}
		if (i == 1) {
			object->SetModel("axis.obj");
		}

		object3ds.push_back(object);
	}

	// --- オーディオ ---
	soundData = audio->LoadWav("fanfare.wav");
	audio->SoundPlayWave(audio->GetXAudio2().Get(), soundData);
	audio->SetVolume(0.1f);
}

void MyGame::Finalize()
{
	winApp->Finalize();
	delete winApp;
	winApp = nullptr;

	delete dxCommon;
	delete input;
	delete audio;

	delete spriteCommon;
	delete object3dCommon;


	delete camera;

	TextureManager::GetInstance()->Finalize();
	for (uint32_t i = 0; i < spriteNum; ++i) {
		delete sprites[i];
	}

	ModelManager::GetInstance()->Finalize();
	for (auto& obj : object3ds) {
		delete obj;
	}

}

void MyGame::Update()
{
	// ゲームループを抜ける
	if (winApp->ProcessMessage()) {
		endRequest = true;
	}

	//入力の更新
	input->Update();

	//カメラの更新
	camera->Update();

#pragma region スプライト

	for (uint32_t i = 0; i < spriteNum; ++i) {
		sprites[i]->Update();

		Vector2 position = { 200.0f * i, 0.0f };
		sprites[i]->SetPosition(position);

		float rotation = sprites[i]->GetRotate();
		sprites[i]->SetRotate(rotation);

		Vector2 size = { 200.0f,200.0f };
		sprites[i]->SetSize(size);

		Vector4 color = sprites[i]->GetColor();
		sprites[i]->SetColor(color);
	}

#pragma endregion スプライト

#pragma region 3Dオブジェクト

	for (uint32_t i = 0; i < objectNum; ++i) {
		Object3d* obj = object3ds[i];
		obj->Update();

		Vector3 rotate = obj->GetRotate();
		if (i == 0) {
			rotate.x += 0.01f;
		}
		else if (i == 1) {
			rotate.y += 0.01f;
		}

		obj->SetRotate(rotate);
	}

#pragma endregion 3Dオブジェクト
}

void MyGame::Draw()
{
	//描画前処理(DirectX)
	dxCommon->PreDraw();

	// 描画前処理(Object)
	object3dCommon->PreDraw();

	// 描画前処理(Sprite)
	spriteCommon->PreDraw();

	// ↓ ↓ ↓ ↓ Draw を書き込む ↓ ↓ ↓ ↓

	for (uint32_t i = 0; i < spriteNum; ++i) {
		sprites[i]->Draw();
	}

	for (auto& obj : object3ds) {
		obj->Draw();
	}

	// ↑ ↑ ↑ ↑ Draw を書き込む ↑ ↑ ↑ ↑

	// 描画後処理
	dxCommon->PostDraw();
}
