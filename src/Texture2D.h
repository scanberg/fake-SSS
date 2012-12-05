#ifndef GLEN_TEXTURE2D_H
#define GLEN_TEXTURE2D_H

#include "Types.h"

class Texture2D
{
public:
	void bindToSlot(u8 slot);

	void specifyTexture(i32 internalFormat, i32 width, i32 height, )
private:
	u32 textureHandle;

	u32 width;
	u32 height;
};

#endif