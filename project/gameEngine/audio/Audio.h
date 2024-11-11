#pragma once
#include <fstream>

#include <xaudio2.h>
#include "wrl.h"

#pragma comment(lib,"xaudio2.lib")

// オーディオ
#pragma region 構造体
	// チャンクヘッダ
struct ChunkHeader {
	char id[4];		 // チャンク毎のID
	int32_t size;	 // チャンクサイズ
};
// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;	// "RIFF"
	char type[4];		// "WAVE"
};
// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk;	// "fmt"
	WAVEFORMATEX fmt;	// 波形フォーマット
};
// 音声データ
struct SoundData {
	WAVEFORMATEX wfex;		// 波形フォーマット
	BYTE* pBuffer;			// バッファの先頭アドレス
	unsigned int bufferSize;// バッファサイズ
};
#pragma endregion 構造体

class Audio
{
#pragma region シングルトンインスタンス
private:
	static Audio* instance;

	Audio() = default;
	~Audio() = default;
	Audio(Audio&) = delete;
	Audio& operator = (Audio&) = delete;

public:
	// シングルトンインスタンスの取得
	static Audio* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

public:
	// 初期化
	void Initialize();

	// 音声データ読み込み
	SoundData LoadWav(const char* filename);

	// 音声データの解放
	void SoundUnload(Microsoft::WRL::ComPtr<IXAudio2> xAudio2, SoundData* soundData);

	// サウンド再生
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);
	// サウンド停止
	void SoundStop(IXAudio2SourceVoice* pSourceVoice);

	// ボリュームを設定
	void SetVolume(float volume);

public:
	// xAudioの取得
	Microsoft::WRL::ComPtr<IXAudio2> GetXAudio2() const { return xAudio2_; }

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	IXAudio2SourceVoice* pSourceVoice_;

	HRESULT hr;
};

