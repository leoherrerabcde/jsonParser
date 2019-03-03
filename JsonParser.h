#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON

#include <cstdio>
#include <iostream>
#include <iostream>
#include <unordered_map>


class JsonParser
{
    public:
        JsonParser();
        virtual ~JsonParser();

        static int rapidJsontest1();
        static int rapidJsontest2();

        //bool getFrame(std::string& strBuffer);

        void loadPlaneText(const std::string& strPlaneText);
        bool isDocJsonReady() {return m_bDocJsoReady;}
        bool isMapIDReady() {return m_bMapIDReady;}
        void createIdTable();

    protected:

    private:

        char*                   m_chPlainText;
        size_t                  m_iPlainTextSize;
        rapidjson::Document     m_docJson;
        bool                    m_bDocJsoReady;
        std::unordered_map<std::string,std::string> m_mapID;
        bool                    m_bMapIDReady;
};

#endif // JSONPARSER_H
