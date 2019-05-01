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
#include <set>
#include <list>


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
        bool isMapIdValueReady() {return m_bMapIdValueReady;}
        void createIdTable();
        void createIdValueMap();
        void setIdKeyLabel(const std::string& strLbl) {m_strIdKeyLabel=strLbl;}
        void setValueKeyLabel(const std::string& strLbl) {m_strValueKeyLabel=strLbl;}
        bool hasID(const std::string& strID);
        std::string getValue(const std::string& strMember);
        std::string getDocJsonValue(const std::string& strMember);
        static bool getPlaneText(const std::string& strMember, const std::string& strValue,
                            std::list<std::string>& fileList,
                            std::list<std::string>& memberList,
                            std::string& strPlainText);
        static bool getValueMessage(const std::string& msg, const std::string& valueName, std::string& value);
        static bool readFile(const std::string& filename, std::string& container);

    protected:



    private:

        static bool getPlaneText2(const std::string& strMember, const std::string& strValue,
                            std::list<std::string>& fileList,
                            std::list<std::string>& memberList,
                            std::string& strPlainText);

        char*                   m_chPlainText;
        size_t                  m_iPlainTextSize;
        rapidjson::Document     m_docJson;
        bool                    m_bDocJsoReady;
        std::set<std::string>   m_mapID;
        std::unordered_map<std::string,std::string> m_mapIdValue;
        bool                    m_bMapIDReady;
        bool                    m_bMapIdValueReady;
        std::string             m_strIdKeyLabel;
        std::string             m_strValueKeyLabel;
};

#endif // JSONPARSER_H
