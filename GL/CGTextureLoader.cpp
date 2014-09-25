
#include <GL\CGTextureLoader.h>
#include <wincodec.h>
#include <CGApp\CGApp.h>

using namespace std;
using namespace CoreStructures;

static CGApp				*wicServiceProvider = NULL; // weak reference to WIC service provider class
static CGMipmapGenMode		mipmapGenMode = CG_NO_MIPMAP_GEN; // supported MIPMAP generation


// private method to load image using WIC and return new texture object.  If the new texture is created, a non-zero texture object ID (provided by OpenGL) is returned and the new texture is bound to GL_TEXTURE_2D in the currently active texture unit

GLuint newTextureFromFile(const wstring& textureFilePath) {

	if (!wicServiceProvider)
		return 0;

	IWICBitmap *textureBitmap = NULL;
	IWICBitmapLock *lock = NULL;
	GLuint newTexture = 0;
	

	HRESULT hr = wicServiceProvider->loadWICBitmap(textureFilePath.c_str(), &textureBitmap);

	UINT w = 0, h = 0;

	if (SUCCEEDED(hr))
		hr = textureBitmap->GetSize(&w, &h);

	WICRect rect = {0, 0, w, h};

	if (SUCCEEDED(hr))
		hr = textureBitmap->Lock(&rect, WICBitmapLockRead, &lock);

	UINT bufferSize = 0;
	BYTE *buffer = NULL;

	if (SUCCEEDED(hr))
		hr = lock->GetDataPointer(&bufferSize, &buffer);

	if (SUCCEEDED(hr)) {
		
		glGenTextures(1, &newTexture);
		glBindTexture(GL_TEXTURE_2D, newTexture);

		// note: GL_BGRA format used - input image format converted to GUID_WICPixelFormat32bppPBGRA for consistent interface with OpenGL texture setup
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
	}

	SafeRelease(&lock);
	SafeRelease(&textureBitmap);

	return newTexture;
}



//
// public interface
//

void CGTextureLoader::initialise(CGApp *hostApp) {

	wicServiceProvider = hostApp; // store weak reference to WIC service provider

	if (glewIsSupported("GL_ARB_framebuffer_object"))
		mipmapGenMode = CG_CORE_MIPMAP_GEN;
	else if (glewIsSupported("GL_EXT_framebuffer_object"))
		mipmapGenMode = CG_EXT_MIPMAP_GEN;
	else
		mipmapGenMode = CG_NO_MIPMAP_GEN;
}


GLuint CGTextureLoader::loadTexture(const wstring& textureFilePath) {

	GLuint newTexture = newTextureFromFile(textureFilePath);

	// setup default texture properties
	if (newTexture) {

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	return newTexture;
}


GLuint CGTextureLoader::loadTexture(const wstring& textureFilePath, const CGTextureGenProperties& textureProperties) {

	GLuint newTexture = newTextureFromFile(textureFilePath);

	// setup custom texture properties
	if (newTexture) {

		// verify we don't use GL_LINEAR_MIPMAP_LINEAR which has no meaning in non-mipmapped textures.  If not set, default to GL_LINEAR (bi-linear) filtering.
		GLint minFilter = (!textureProperties.genMipMaps && textureProperties.minFilter==GL_LINEAR_MIPMAP_LINEAR)? GL_LINEAR : textureProperties.minFilter;
		GLint maxFilter = (!textureProperties.genMipMaps && textureProperties.maxFilter==GL_LINEAR_MIPMAP_LINEAR)? GL_LINEAR : textureProperties.maxFilter;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, textureProperties.anisotropicLevel);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureProperties.wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureProperties.wrap_t);
		
		if (textureProperties.genMipMaps && mipmapGenMode!=CG_NO_MIPMAP_GEN) {

			if (mipmapGenMode==CG_CORE_MIPMAP_GEN)
				glGenerateMipmap(GL_TEXTURE_2D);
			else if (mipmapGenMode==CG_EXT_MIPMAP_GEN)
				glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
	}

	return newTexture;
}

