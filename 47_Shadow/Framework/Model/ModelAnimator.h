#pragma once

class ModelAnimator
{
public:
	ModelAnimator(Shader* shader);
	~ModelAnimator();

	void Update();

private:
	void UpdateTweenMode(UINT index);
	void UpdateBlendMode(UINT index);

public:
	void Render();

public:
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadClip(wstring file);

	void PlayTweenMode(UINT instance, UINT clip, float speed = 1.0f, float takeTime = 1.0f);
	void PlayBlendMode(UINT instance, UINT clip1, UINT clip2, UINT clip3);
	void SetBlendAlpha(UINT instance, float alpha);

	Model* GetModel() { return model; }

	void Pass(UINT pass);

	Transform* AddTransform();
	Transform* GetTransform(UINT index) { return transforms[index]; }
	void UpdateTransforms();
	UINT GetTransformCount() { return transforms.size(); }

	Matrix GetAttachTransform(UINT index, UINT boneIndex);
	void GetAttachTransforms(UINT index, Matrix* outResult);

private:
	void CreateTexture();
	void CreateClipTransform(UINT index);

private:
	struct ClipTransform
	{
		Matrix** Transform;

		ClipTransform()
		{
			Transform = new Matrix*[MAX_MODEL_KEYFRAMES];

			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				Transform[i] = new Matrix[MAX_MODEL_TRANSFORMS];
		}

		~ClipTransform()
		{
			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				SafeDeleteArray(Transform[i]);

			SafeDeleteArray(Transform);
		}
	};
	ClipTransform* clipTransforms = NULL;

	ID3D11Texture2D* texture = NULL;
	ID3D11ShaderResourceView* srv;

private:
	struct KeyframeDesc
	{
		int Clip = 0;

		UINT CurrFrame = 0;
		UINT NextFrame = 0;

		float Time = 0.0f;
		float RunningTime = 0.0f;

		float Speed = 1.0f;

		Vector2 Padding;
	}; //keyframeDesc;

	struct TweenDesc
	{
		float TakeTime = 1.0f;
		float TweenTime = 0.0f;
		float ChangeTime = 0.0f;
		float Padding;

		KeyframeDesc Curr;
		KeyframeDesc Next;

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;
		}
	} tweenDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* tweenBuffer;
	ID3DX11EffectConstantBuffer* sTweenBuffer;

private:
	struct BlendDesc
	{
		UINT Mode = 0;
		float Alpha = 0.0f;
		Vector2 Padding;

		KeyframeDesc Clip[3];
	} blendDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;

private:
	Shader* shader;
	Model* model;
	
	vector<Transform *> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceBuffer;

	Color colors[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceColorBuffer;

private:
	const float frameRate = 30.0f;
	float frameTime = 0.0f;

	Shader* computeShader;
	Matrix** boneTransforms;

	ID3DX11EffectConstantBuffer* sComputeTweenBuffer;
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer;

	ID3DX11EffectShaderResourceVariable* sTransformSrv;

	StructuredBuffer* inputWorldBuffer = NULL;
	ID3DX11EffectShaderResourceVariable* sInputWorldSrv;

	StructuredBuffer* inputBoneBuffer = NULL;
	ID3DX11EffectShaderResourceVariable* sInputBoneSrv;

	TextureBuffer* outputBuffer = NULL;
	ID3DX11EffectUnorderedAccessViewVariable* sOutputUav;
};