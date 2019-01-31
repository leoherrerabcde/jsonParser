#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON

#include <cstdio>
#include <iostream>
#include <iostream>

class JsonParser
{
    public:
        JsonParser();
        virtual ~JsonParser();

        static int rapidJsontest1();
        static int rapidJsontest2();

        bool getFrame(std::string& strBuffer);

    protected:

    private:

};

#endif // JSONPARSER_H
