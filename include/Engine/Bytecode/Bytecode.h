#ifndef ENGINE_BYTECODE_BYTECODE_H
#define ENGINE_BYTECODE_BYTECODE_H

#define PUBLIC
#define PRIVATE
#define PROTECTED
#define STATIC
#define VIRTUAL
#define EXPOSED


#include <Engine/Bytecode/Types.h>
#include <Engine/Bytecode/CompilerEnums.h>

class Bytecode {
public:
    static const char*         Magic;
    static Uint32              LatestVersion;
    static vector<const char*> FunctionNames;
    vector<ObjFunction*> Functions;
    Uint8 Version;
    bool HasDebugInfo = false;
    const char*                SourceFilename = nullptr;

    Bytecode();
    ~Bytecode();
    bool Read(BytecodeContainer bytecode, HashMap<char*>* tokens);
    void Write(Stream* stream, const char* sourceFilename, HashMap<Token>* tokenMap);
};

#endif /* ENGINE_BYTECODE_BYTECODE_H */