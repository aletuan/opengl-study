/*
 tdogl::Texture
 
 Copyright 2012 Thomas Dalling - http://tomdalling.com/
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "Texture.h"
#include <stdexcept>

using namespace tdogl;

static GLenum TextureFormatForBitmapFormat(Bitmap::Format format)
{
    switch (format) {
        case Bitmap::Format_Grayscale: return GL_LUMINANCE;
        case Bitmap::Format_GrayscaleAlpha: return GL_LUMINANCE_ALPHA;
        case Bitmap::Format_RGB: return GL_RGB;
        case Bitmap::Format_RGBA: return GL_RGBA;
        default: throw std::runtime_error("Unrecognised Bitmap::Format");
    }
}

Texture::Texture(const Bitmap& bitmap, GLint minMagFiler, GLint wrapMode) :
    _originalWidth((GLfloat)bitmap.width()),
    _originalHeight((GLfloat)bitmap.height())
{
    // Step 1: Define a texture with an ID
    glGenTextures(1, &_object);
    
    // Step 2: Binding created texture with sub sequence command
    glBindTexture(GL_TEXTURE_2D, _object);
    
    // Step 3: Applying filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagFiler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagFiler);
    
    // Step 4: Applying wrapping mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    // Step 5: When texture is bound and configure, we can generate with glTextImage2D
    glTexImage2D(GL_TEXTURE_2D,
                 0, 
                 TextureFormatForBitmapFormat(bitmap.format()),
                 (GLsizei)bitmap.width(), 
                 (GLsizei)bitmap.height(),
                 0, 
                 TextureFormatForBitmapFormat(bitmap.format()), 
                 GL_UNSIGNED_BYTE, 
                 bitmap.pixelBuffer());
    
    // Step 6: After done, we need to free memory and unbind texture object
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_object);
}

GLuint Texture::object() const
{
    return _object;
}

GLfloat Texture::originalWidth() const
{
    return _originalWidth;
}

GLfloat Texture::originalHeight() const
{
    return _originalHeight;
}
