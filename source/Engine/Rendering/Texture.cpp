#if INTERFACE
#include <Engine/Includes/Standard.h>

need_t Texture;

class Texture {
public:
    Uint32   Format;
    Uint32   Access;
    Uint32   Width;
    Uint32   Height;

    void*    Pixels;
    int      Pitch;

    Uint32   ID;
    void*    DriverData;

    Texture* Prev;
    Texture* Next;

    bool     Paletted;
    Uint32*  PaletteColors;
    unsigned NumPaletteColors;
};
#endif

#include <Engine/Rendering/Texture.h>
#include <Engine/Diagnostics/Memory.h>
#include <Engine/Utilities/ColorUtils.h>

PUBLIC STATIC Texture* Texture::New(Uint32 format, Uint32 access, Uint32 width, Uint32 height) {
    Texture* texture = (Texture*)Memory::TrackedCalloc("Texture::Texture", 1, sizeof(Texture));
    texture->Format = format;
    texture->Access = access;
    texture->Width = width;
    texture->Height = height;
    texture->Pixels = Memory::TrackedCalloc("Texture::Pixels", 1, sizeof(Uint32) * texture->Width * texture->Height);
    return texture;
}

PUBLIC void            Texture::SetPalette(Uint32* palette, unsigned numPaletteColors) {
    Memory::Free(PaletteColors);

    if (palette && numPaletteColors) {
        Paletted = true;
        PaletteColors = palette;
        NumPaletteColors = numPaletteColors;
    }
    else {
        Paletted = false;
        PaletteColors = nullptr;
        NumPaletteColors = 0;
    }
}

PUBLIC         bool  Texture::ConvertToRGBA() {
    if (!Paletted)
        return false;

    Uint32 *pixels = (Uint32*)Pixels;

    for (size_t i = 0; i < Width * Height; i++) {
        if (pixels[i])
            pixels[i] = PaletteColors[pixels[i]];
    }

    SetPalette(nullptr, 0);

    return true;
}
PUBLIC         bool  Texture::ConvertToPalette(Uint32 *palColors, unsigned numPaletteColors) {
    ConvertToRGBA();

    Uint32 *pixels = (Uint32*)Pixels;
    Uint32 colors[4];

    for (size_t i = 0; i < Width * Height; i++) {
        ColorUtils::Separate(pixels[i], colors);
        if (colors[3])
            pixels[i] = ColorUtils::NearestColor(colors[0], colors[1], colors[2], palColors, numPaletteColors);
        else
            pixels[i] = 0;
    }

    Paletted = true;

    return true;
}

PUBLIC void            Texture::Copy(Texture* source) {
    if (!Pixels || !source || !source->Pixels)
        return;

    Uint32 *dest = (Uint32*)Pixels;
    Uint32 *src = (Uint32*)source->Pixels;
    size_t stride = 4;

    for (unsigned y = 0; y < Height; y++) {
        size_t sz = source->Width;
        if (sz > Width)
            sz = Width;
        memcpy(&dest[y * (Width * stride)], &src[y * (source->Width * stride)], sz * stride);
    }
}

PUBLIC void            Texture::Dispose() {
    Memory::Free(PaletteColors);
    Memory::Free(Pixels);

    PaletteColors = nullptr;
    Pixels = nullptr;
}
