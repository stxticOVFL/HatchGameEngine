#ifndef ENGINE_IO_STREAM_H
#define ENGINE_IO_STREAM_H

#define PUBLIC
#define PRIVATE
#define PROTECTED
#define STATIC
#define VIRTUAL
#define EXPOSED


#include <Engine/Includes/Standard.h>
#include <Engine/Diagnostics/Memory.h>

class Stream {
public:
    virtual void    Close();
    virtual void    Seek(Sint64 offset);
    virtual void    SeekEnd(Sint64 offset);
    virtual void    Skip(Sint64 offset);
    virtual size_t  Position();
    virtual size_t  Length();
    virtual size_t  ReadBytes(void* data, size_t n);
            Uint8   ReadByte();
            Uint16  ReadUInt16();
            Uint16  ReadUInt16BE();
            Uint32  ReadUInt32();
            Uint32  ReadUInt32BE();
            Uint64  ReadUInt64();
            Sint16  ReadInt16();
            Sint16  ReadInt16BE();
            Sint32  ReadInt32();
            Sint32  ReadInt32BE();
            Sint64  ReadInt64();
            float   ReadFloat();
            char*   ReadLine();
            char*   ReadString();
            Uint16* ReadUnicodeString();
            char*   ReadHeaderedString();
    virtual Uint32  ReadCompressed(void* out);
    virtual size_t  WriteBytes(void* data, size_t n);
            void    WriteByte(Uint8 data);
            void    WriteUInt16(Uint16 data);
            void    WriteUInt16BE(Uint16 data);
            void    WriteUInt32(Uint32 data);
            void    WriteUInt32BE(Uint32 data);
            void    WriteUInt64(Uint64 data);
            void    WriteInt16(Sint16 data);
            void    WriteInt16BE(Sint16 data);
            void    WriteInt32(Sint32 data);
            void    WriteInt32BE(Sint32 data);
            void    WriteInt64(Sint64 data);
            void    WriteFloat(float data);
            void    WriteString(const char* string);
            void    WriteHeaderedString(const char* string);
            void    CopyTo(Stream* dest);
    virtual         ~Stream();
};

#endif /* ENGINE_IO_STREAM_H */
