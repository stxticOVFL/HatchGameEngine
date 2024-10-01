#ifndef ENGINE_RESOURCETYPES_ISPRITE_H
#define ENGINE_RESOURCETYPES_ISPRITE_H

#define PUBLIC
#define PRIVATE
#define PROTECTED
#define STATIC
#define VIRTUAL
#define EXPOSED


#include <Engine/Includes/Standard.h>
#include <Engine/Sprites/Animation.h>
#include <Engine/Rendering/Texture.h>

class ISprite {
public:
    char* Filename = nullptr;
    bool LoadFailed = true;
    vector<Texture*> Spritesheets;
    vector<string> SpritesheetFilenames;
    int CollisionBoxCount = 0;
    vector<Animation> Animations;

    ISprite();
    ISprite(const char* filename);
    Texture* AddSpriteSheet(const char* filename);
    void ReserveAnimationCount(int count);
    void AddAnimation(const char* name, int animationSpeed, int frameToLoop);
    void AddAnimation(const char* name, int animationSpeed, int frameToLoop, int frmAlloc);
    void AddFrame(int duration, int left, int top, int width, int height, int pivotX, int pivotY);
    void AddFrame(int duration, int left, int top, int width, int height, int pivotX, int pivotY, int id);
    void AddFrame(int animID, int duration, int left, int top, int width, int height, int pivotX, int pivotY, int id);
    void RemoveFrames(int animID);
    void ConvertToRGBA();
    void ConvertToPalette(unsigned paletteNumber);
    bool LoadAnimation(const char* filename);
    int FindAnimation(const char* animname);
    void LinkAnimation(vector<Animation> ani);
    bool SaveAnimation(const char* filename);
    void Dispose();
    ~ISprite();
};

#endif /* ENGINE_RESOURCETYPES_ISPRITE_H */