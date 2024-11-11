#include "Audio.h"
#include <cassert>

Audio* Audio::instance = nullptr;

Audio* Audio::GetInstance()
{
	if (instance == nullptr) {
		instance = new Audio;
	}
	return instance;
}

void Audio::Finalize()
{
	delete instance;
	instance = nullptr;
}

void Audio::Initialize()
{
	// xAudio生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイス生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

SoundData Audio::LoadWav(const char* filename)
{
	std::ifstream file;
	// 基本パスを指定（"Resources/audio/"）
	std::string basePath = "Resources/audio/";
	std::string fullPath = basePath + filename;

	// .wavファイルをバイナリモードで展開
	file.open(fullPath.c_str(), std::ios_base::binary);
	// ファイル展開失敗時
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// チャンクのループを開始
	ChunkHeader chunkHeader;
	FormatChunk format = {};

	while (file.read((char*)&chunkHeader, sizeof(chunkHeader))) {
		// チャンクIDが "fmt" か確認
		if (strncmp(chunkHeader.id, "fmt ", 4) == 0) {
			// Formatチャンクのサイズを確認し、データを読み込む
			assert(chunkHeader.size <= sizeof(format.fmt));

			format.chunk = chunkHeader; // チャンクヘッダーをコピー
			file.read((char*)&format.fmt, chunkHeader.size); // fmtのデータを読み込み
			
			break;
		}
		else {
			// 次のチャンクに移動
			file.seekg(chunkHeader.size, std::ios_base::cur);
		}
	}

	// "fmt"チャンクが見つからなかった場合のエラーとしてだす
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データの読み込み）
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Wavファイルを閉じる
	file.close();


	// returnする為のデータ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void Audio::SoundUnload(Microsoft::WRL::ComPtr<IXAudio2> xAudio2, SoundData* soundData)
{
	// 音声データ解放
	xAudio2.Reset();

	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
{
	// 波形フォーマットをもとにSourceVoiceの生成
	hr = xAudio2->CreateSourceVoice(&pSourceVoice_, &soundData.wfex);
	assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	hr = pSourceVoice_->SubmitSourceBuffer(&buf);
	hr = pSourceVoice_->Start();
}

void Audio::SoundStop(IXAudio2SourceVoice* pSourceVoice)
{
	// サウンド停止
	pSourceVoice->Stop();

	// 再生をリセット
	pSourceVoice->FlushSourceBuffers();
}

void Audio::SetVolume(float volume)
{
	// 0.0f ~ 1.0f の範囲で音量を設定
	HRESULT hr = pSourceVoice_->SetVolume(volume);

	// 失敗時にエラーを出力
	assert(SUCCEEDED(hr) && "Failed to set volume");
}
