#ifndef XMLNODE_H
#define XMLNODE_H

#include <map>

class XMLAttributes {
public:
    vector<char*> KeyVector;
    HashMap<Token> ValueMap;

    void  Put(char* key, Token value) {
        Uint32 hash = ValueMap.HashFunction(key, strlen(key));
        ValueMap.Put(hash, value);
        KeyVector.push_back(key);
    }
    Token Get(const char* key) {
        return ValueMap.Get(key);
    }
    bool  Exists(const char* key) {
        return ValueMap.Exists(key);
    }
    void  Dispose() {
        for (size_t i = 0; i < KeyVector.size(); i++)
            free(KeyVector[i]);

        KeyVector.clear();
    }
    ~XMLAttributes() {
        Dispose();
    }
};

struct XMLNode {
    Token            name;
    XMLAttributes    attributes;
    vector<XMLNode*> children;
    XMLNode*         parent;
    Stream*          base_stream;
};

#endif /* XMLNODE_H */
