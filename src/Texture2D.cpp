#include "Texture2D.h"
#include "Log.h"

void Texture2D::bindToSlot(u8 slot)
{
	if(glIsTexture(textureHandle))
	{
		slot = glm::clamp(slot,0,8);
		glActiveTexture(GL_TEXTURE0 + slot);
	}
	else
	{
		logWarning("texture was not bound to slot, texture handle is not valid");
	}
}