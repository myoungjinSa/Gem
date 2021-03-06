#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <array>
#include "Types.h"
#include "VideoFile.h"
#include "Types.h"
#include "./String/String.h"


class Renderer;
class TextureLoader;

class Texture
{
	friend class Renderer;
public:
	class RenderContext {
	public:
		Renderer& renderer;
		float prevFrameNo;
		float totalDeltaTime;
		std::unique_ptr<VideoFile> preVideoFile;
		

		RenderContext(Renderer& _renderer) 
			: renderer(_renderer)
			, prevFrameNo(0.0f)
			, totalDeltaTime(0.0f)
		{
		}
	};

	enum class eType {
		TYPE_IMAGE,
		TYPE_VIDEO,
		TYPE_CUBEMAP,
		TYPE_COUNT,
	};

	eType        type;
	unsigned int textureID;
	
	Texture();
	Texture(const Texture& other) = delete;
	virtual ~Texture();

	bool Initialize(Renderer& renderer, const WString& fileName);
	void Shutdown();

	unsigned int GetTextureID() const {	return textureID;}
	Picture& GetPicture() { return picture; }

	bool IsNull()const { return textureID == 0; }
	
	bool Update(float elapsedTime);
private:
	bool Load(const WString& fileName, int& genTextureID);
	
	std::unique_ptr<RenderContext> renderContext;
	Picture      picture;
};

class CubemapTexture : public std::array<std::shared_ptr<Texture>, 6>{
public:
	enum class eCubemapSide {
		CUBEMAP_RIGHT,
		CUBEMAP_LEFT,
		CUBEMAP_TOP,
		CUBEMAP_BOTTOM,
		CUBEMAP_FRONT,
		CUBEMAP_BACK
	};
		
	CubemapTexture();

	bool LoadTexture(Renderer& renderer, eCubemapSide side, const WString& textureName);
	bool Init(Renderer& renderer);

	unsigned int GetTextureID() const { return textureID; }
private:
	unsigned int textureID;
};

class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	
	static const std::shared_ptr<Texture>& GetTexture(Renderer& renderer, const WString& filename);
	static void ResetTexture(Renderer& renderer, const WString& filename);
	static void Release();
private:
	static bool Load(Renderer& renderer, const WString& filename);

	static std::vector<std::pair<WString, std::shared_ptr<Texture>>> textures;
};

class RenderTarget {
public:
	RenderTarget();
	~RenderTarget();

	bool Create(Renderer& renderer, size_t fboCount);
	
	void SetSize(const Size2u& _size);
	void SetSize(size_t width, size_t height);
	Size2u GetSize() const;

	bool IsNull() const { return iFrameBuffer == 0; }

public:
	unsigned int iFrameBuffer;

private:
	Size2u size;
};
#endif