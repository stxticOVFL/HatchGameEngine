#ifdef USING_OPENGL

#include <Engine/Rendering/GL/GLShader.h>

#include <Engine/Diagnostics/Log.h>

#define CHECK_GL() \
    GLShader::CheckGLError(__LINE__)

GLShader::GLShader(std::string vertexShaderSource, std::string fragmentShaderSource) {
    GLint compiled = GL_FALSE;
    ProgramID = glCreateProgram(); CHECK_GL();

    const char *vertexShaderSourceStr = vertexShaderSource.c_str();

    VertexProgramID = glCreateShader(GL_VERTEX_SHADER); CHECK_GL();
    glShaderSource(VertexProgramID, 1, &vertexShaderSourceStr, NULL); CHECK_GL();
    glCompileShader(VertexProgramID); CHECK_GL();
    glGetShaderiv(VertexProgramID, GL_COMPILE_STATUS, &compiled); CHECK_GL();
    if (compiled != GL_TRUE) {
        Log::Print(Log::LOG_ERROR, "Unable to compile vertex shader %d!", VertexProgramID);
        CheckShaderError(VertexProgramID);
        return;
    }

    const char *fragmentShaderSourceStr = fragmentShaderSource.c_str();

    FragmentProgramID = glCreateShader(GL_FRAGMENT_SHADER); CHECK_GL();
    glShaderSource(FragmentProgramID, 1, &fragmentShaderSourceStr, NULL); CHECK_GL();
    glCompileShader(FragmentProgramID); CHECK_GL();
    glGetShaderiv(FragmentProgramID, GL_COMPILE_STATUS, &compiled); CHECK_GL();
    if (compiled != GL_TRUE) {
        Log::Print(Log::LOG_ERROR, "Unable to compile fragment shader %d!", FragmentProgramID);
        CheckShaderError(FragmentProgramID);
        return;
    }

    AttachAndLink();
}
GLShader::GLShader(Stream* streamVS, Stream* streamFS) {
    GLint compiled = GL_FALSE;
    ProgramID = glCreateProgram(); CHECK_GL();

    size_t lenVS = streamVS->Length();
    size_t lenFS = streamFS->Length();

    char* sourceVS = (char*)malloc(lenVS);
    char* sourceFS = (char*)malloc(lenFS);

    // const char* sourceVSMod[2];
    const char* sourceFSMod[2];

    streamVS->ReadBytes(sourceVS, lenVS);
    sourceVS[lenVS] = 0;

    streamFS->ReadBytes(sourceFS, lenFS);
    sourceFS[lenFS] = 0;

    #if GL_ES_VERSION_2_0 || GL_ES_VERSION_3_0
        sourceFSMod[0] = "precision mediump float;\n";
    #else
        sourceFSMod[0] = "\n";
    #endif
    sourceFSMod[1] = sourceFS;

    VertexProgramID = glCreateShader(GL_VERTEX_SHADER); CHECK_GL();

    glShaderSource(VertexProgramID, 1, &sourceVS, NULL); CHECK_GL();
    glCompileShader(VertexProgramID); CHECK_GL();
    glGetShaderiv(VertexProgramID, GL_COMPILE_STATUS, &compiled); CHECK_GL();
    if (compiled != GL_TRUE) {
        Log::Print(Log::LOG_ERROR, "Unable to compile vertex shader %d!", VertexProgramID);
        CheckShaderError(VertexProgramID);
        CheckGLError(__LINE__);

        glDeleteProgram(ProgramID); CHECK_GL();
        glDeleteShader(VertexProgramID); CHECK_GL();

        ProgramID = 0;
        VertexProgramID = 0;

        free(sourceVS);
        free(sourceFS);
        return;
    }

    FragmentProgramID = glCreateShader(GL_FRAGMENT_SHADER); CHECK_GL();
    glShaderSource(FragmentProgramID, 2, sourceFSMod, NULL); CHECK_GL();
    glCompileShader(FragmentProgramID); CHECK_GL();
    glGetShaderiv(FragmentProgramID, GL_COMPILE_STATUS, &compiled); CHECK_GL();
    if (compiled != GL_TRUE) {
        Log::Print(Log::LOG_ERROR, "Unable to compile fragment shader %d!", FragmentProgramID);
        CheckShaderError(FragmentProgramID);
        CheckGLError(__LINE__);

        glDeleteProgram(ProgramID); CHECK_GL();
        glDeleteShader(VertexProgramID); CHECK_GL();
        glDeleteShader(FragmentProgramID); CHECK_GL();

        ProgramID = 0;
        VertexProgramID = 0;
        FragmentProgramID = 0;

        free(sourceVS);
        free(sourceFS);
        return;
    }

    free(sourceVS);
    free(sourceFS);

    AttachAndLink();
}
void  GLShader::AttachAndLink() {
    glAttachShader(ProgramID, VertexProgramID);                 CHECK_GL();
    glAttachShader(ProgramID, FragmentProgramID);               CHECK_GL();

    glBindAttribLocation(ProgramID, 0, "i_position");           CHECK_GL();
    glBindAttribLocation(ProgramID, 1, "i_uv");                 CHECK_GL();
    glBindAttribLocation(ProgramID, 2, "i_color");              CHECK_GL();

    glLinkProgram(ProgramID);                                   CHECK_GL();

    GLint isLinked = GL_FALSE;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, (int*)&isLinked); CHECK_GL();
    if (isLinked != GL_TRUE) {
        CheckProgramError(ProgramID);
    }

    LocProjectionMatrix = GetUniformLocation("u_projectionMatrix");
    LocModelViewMatrix = GetUniformLocation("u_modelViewMatrix");

    LocPosition = GetAttribLocation("i_position");
    LocTexCoord = GetAttribLocation("i_uv");
    LocVaryingColor = GetAttribLocation("i_color");

    LocColor = GetUniformLocation("u_color");
    LocTexture = GetUniformLocation("u_texture");
    LocTextureU = GetUniformLocation("u_textureU");
    LocTextureV = GetUniformLocation("u_textureV");
    LocPalette = GetUniformLocation("u_paletteTexture");

    LocFogColor = GetUniformLocation("u_fogColor");
    LocFogLinearStart = GetUniformLocation("u_fogLinearStart");
    LocFogLinearEnd = GetUniformLocation("u_fogLinearEnd");
    LocFogDensity = GetUniformLocation("u_fogDensity");
    LocFogTable = GetUniformLocation("u_fogTable");

    CachedBlendColors[0] = CachedBlendColors[1] = CachedBlendColors[2] = CachedBlendColors[3] = 0.0;
}


bool   GLShader::CheckShaderError(GLuint shader) {
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength); CHECK_GL();

    char* infoLog = new char[maxLength];
    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog); CHECK_GL();
    infoLog[strlen(infoLog) - 1] = 0;

    if (infoLogLength > 0)
        Log::Print(Log::LOG_ERROR, "%s", infoLog);

    delete[] infoLog;
    return false;
}
bool   GLShader::CheckProgramError(GLuint prog) {
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &maxLength); CHECK_GL();

    char* infoLog = new char[maxLength];
    glGetProgramInfoLog(prog, maxLength, &infoLogLength, infoLog); CHECK_GL();
    infoLog[strlen(infoLog) - 1] = 0;

    if (infoLogLength > 0)
        Log::Print(Log::LOG_ERROR, "%s", infoLog);

    delete[] infoLog;
    return false;
}

GLuint GLShader::Use() {
    glUseProgram(ProgramID); CHECK_GL();

    // glEnableVertexAttribArray(1); CHECK_GL();
    return ProgramID;
}

GLint  GLShader::GetAttribLocation(const GLchar* identifier) {
    GLint value = glGetAttribLocation(ProgramID, identifier); CHECK_GL();
    return value;
}
GLint  GLShader::GetUniformLocation(const GLchar* identifier) {
    GLint value = glGetUniformLocation(ProgramID, identifier); CHECK_GL();
    return value;
}

GLShader::~GLShader() {
    if (CachedProjectionMatrix) {
        delete CachedProjectionMatrix;
    }
    if (CachedModelViewMatrix) {
        delete CachedModelViewMatrix;
    }
    if (VertexProgramID) {
        glDeleteShader(VertexProgramID); CHECK_GL();
    }
    if (FragmentProgramID) {
        glDeleteShader(FragmentProgramID); CHECK_GL();
    }
    if (ProgramID) {
        glDeleteProgram(ProgramID); CHECK_GL();
    }
}

bool GLShader::CheckGLError(int line) {
    const char* errstr = NULL;
    GLenum error = glGetError();
    switch (error) {
        case GL_NO_ERROR: errstr = "no error"; break;
        case GL_INVALID_ENUM: errstr = "invalid enumerant"; break;
        case GL_INVALID_VALUE: errstr = "invalid value"; break;
        case GL_INVALID_OPERATION: errstr = "invalid operation"; break;
        case GL_OUT_OF_MEMORY: errstr = "out of memory"; break;
        #ifdef GL_STACK_OVERFLOW
        case GL_STACK_OVERFLOW: errstr = "stack overflow"; break;
        case GL_STACK_UNDERFLOW: errstr = "stack underflow"; break;
        case GL_TABLE_TOO_LARGE: errstr = "table too large"; break;
        #endif
        #ifdef GL_EXT_framebuffer_object
        case GL_INVALID_FRAMEBUFFER_OPERATION_EXT: errstr = "invalid framebuffer operation"; break;
        #endif
        #if GLU_H
        case GLU_INVALID_ENUM: errstr = "invalid enumerant"; break;
        case GLU_INVALID_VALUE: errstr = "invalid value"; break;
        case GLU_OUT_OF_MEMORY: errstr = "out of memory"; break;
        case GLU_INCOMPATIBLE_GL_VERSION: errstr = "incompatible OpenGL version"; break;
        // case GLU_INVALID_OPERATION: errstr = "invalid operation"; break;
        #endif
        default:
            errstr = "unknown error";
            break;
    }
    if (error != GL_NO_ERROR) {
        Log::Print(Log::LOG_ERROR, "OpenGL error on line %d: %s", line, errstr);
        return true;
    }
    return false;
}
#undef CHECK_GL

#endif /* USING_OPENGL */
