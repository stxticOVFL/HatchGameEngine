#ifndef ENGINE_PRINTBUFFER_H
#define ENGINE_PRINTBUFFER_H

struct PrintBuffer {
    char** Buffer;
    int WriteIndex;
    int BufferSize;
};

inline int buffer_printf(PrintBuffer* printBuffer, const char *format, ...) {
    va_list args;
    va_list argsCopy;
    va_start(args, format);
    va_copy(argsCopy, args);

    // Do a simple print if printBuffer is missing
    if (!printBuffer || !printBuffer->Buffer) {
        vprintf(format, args);
        va_end(argsCopy);
        va_end(args);
        return 0;
    }

    // Get the character count we would write if we did
    int count = vsnprintf(NULL, 0, format, argsCopy);

    while (printBuffer->WriteIndex + count >= printBuffer->BufferSize) {
        // Increase the buffer size
        printBuffer->BufferSize <<= 1;

        // Reallocate buffer
        *printBuffer->Buffer = (char*)realloc(*printBuffer->Buffer, printBuffer->BufferSize);
        if (!*printBuffer->Buffer) {
            Log::Print(Log::LOG_ERROR, "Could not reallocate print buffer of size %d!", printBuffer->BufferSize);
            va_end(argsCopy);
            va_end(args);
            return -1;
        }
    }

    // Write the characters
    printBuffer->WriteIndex += vsnprintf(*printBuffer->Buffer + printBuffer->WriteIndex, printBuffer->BufferSize - printBuffer->WriteIndex, format, args);
    va_end(argsCopy);
    va_end(args);
    return 0;
}

#endif