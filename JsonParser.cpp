#include "JsonParser.h"


#include "SCCFileManager.h"
#include "device.h"

#include <fstream>


using namespace std;
using namespace rapidjson;

/* Frame Format
    START_MARK | HEADER | DATA_LENGTH | DATA | VERIFICATOR| | STOP_MARK
     1            4         8           nn      16              1

     START_MARK: '<'
     STOP_MARK: '>'
*/

/*static const std::string::size_type st_MinFrameSize = (1+8+1+16+1);
static const char st_StartMark = '<';
static const char st_StopMark = '>';*/


JsonParser::JsonParser() : m_chPlainText(NULL), m_iPlainTextSize(0), m_bDocJsoReady(false), m_bMapIDReady(false), m_bMapIdValueReady(false)
{
    //ctor
}

JsonParser::~JsonParser()
{
    //dtor
}

/*bool getFrame(std::string& strBuffer)
{
    std::string::size_type pos;
    bool bFrameDetected = false;
    do
    {
        pos = strBuffer.find('#');
        if (pos != std::string::npos)
        {
            if (pos != 0)
                strBuffer = strBuffer.substr(pos);

        }
    }
    while(pos != std::string::npos);

    return bFrameDetected;
}*/

void JsonParser::loadPlaneText(const std::string& strPlaneText)
{
    /*if (m_chPlainText)
    {
        delete [] m_chPlainText;
        m_chPlainText = NULL;
    }
    m_chPlainText = new char[strPlaneText.length()+1];
    memcpy(m_chPlainText, strPlaneText.c_str(),strPlaneText.length());
    m_iPlainTextSize = strPlaneText.length();
    *(m_chPlainText+m_iPlainTextSize) = 0x00;
    m_bDocJsoReady = !m_docJson.Parse(m_chPlainText).HasParseError();*/
    m_bDocJsoReady = !m_docJson.Parse(strPlaneText.c_str()).HasParseError();
}

void JsonParser::createIdTable()
{
    m_bMapIDReady = false;
    m_mapID.clear();

    if (!m_docJson.IsArray())
        return;

    for (Value::ConstValueIterator itr = m_docJson.Begin(); itr != m_docJson.End(); ++itr)
    {
        if (itr->HasMember(m_strIdKeyLabel.c_str()))
        {
            //printf("%d ", itr->GetInt());
            //Value::MemberIterator hello = document.FindMember("hello");
            char key[m_strIdKeyLabel.length()+1];
            memcpy(key, m_strIdKeyLabel.c_str(), m_strIdKeyLabel.length());
            key[m_strIdKeyLabel.length()] = 0x00;
            auto itId = itr->FindMember(key);
            m_mapID.insert(itId->value.GetString());
            //std::cout << "ID " << itr->name.GetString() << " is" << kTypeNames[itr->value.GetType()] << std::endl;
        }
    }
    if (m_mapID.size())
        m_bMapIDReady = true;
}

void JsonParser::createIdValueMap()
{
    m_bMapIdValueReady = false;
    m_mapIdValue.clear();

    if (!m_docJson.IsArray())
        return;

    for (Value::ConstValueIterator itr = m_docJson.Begin(); itr != m_docJson.End(); ++itr)
    {
        std::string strKey("");
        std::string strVal("");
        if (itr->HasMember(m_strIdKeyLabel.c_str()))
        {
            //printf("%d ", itr->GetInt());
            //Value::MemberIterator hello = document.FindMember("hello");
            char key[m_strIdKeyLabel.length()+1];
            memcpy(key, m_strIdKeyLabel.c_str(), m_strIdKeyLabel.length());
            key[m_strIdKeyLabel.length()] = 0x00;
            auto itId = itr->FindMember(key);
            //m_mapID.insert(itId->value.GetString());
            strKey = itId->value.GetString();
        }
        if (itr->HasMember(m_strValueKeyLabel.c_str()))
        {
            char key[m_strValueKeyLabel.length()+1];
            memcpy(key, m_strValueKeyLabel.c_str(), m_strValueKeyLabel.length());
            key[m_strValueKeyLabel.length()] = 0x00;
            auto itId = itr->FindMember(key);
            //m_mapID.insert(itId->value.GetString());
            if (itId->value.IsString())
                strVal = itId->value.GetString();
            if (itId->value.IsInt())
                strVal = std::to_string(itId->value.GetInt());
        }
        m_mapIdValue.insert(std::make_pair(strKey,strVal));
    }
    if (m_mapIdValue.size())
        m_bMapIdValueReady = true;
}

bool JsonParser::hasID(const std::string& strID)
{
    if (m_bMapIDReady)
    {
        auto it = m_mapID.find(strID);

        return (it != m_mapID.end());
    }
    if (m_bMapIdValueReady)
    {
        auto it = m_mapIdValue.find(strID);

        return (it != m_mapIdValue.end());
    }
    return false;
}

std::string JsonParser::getValue(const std::string& strMember)
{
    if (!m_bMapIdValueReady)
        return "";

    auto it = m_mapIdValue.find(strMember);

    if (it == m_mapIdValue.end())
        return "";

    std::string val = it->second;

    return val;
}

std::string JsonParser::getDocJsonValue(const std::string& strMember)
{
    if (!m_docJson.HasMember(strMember.c_str()))
        return "";
    std::string val = m_docJson[strMember.c_str()].GetString();
    return val;
}

bool JsonParser::getPlaneText2(const std::string& strMember, const std::string& strValue,
                            std::list<std::string>& fileList,
                            std::list<std::string>& memberList,
                            std::string& strPlainText)
{
    if (strMember == "" || strValue == "" || !fileList.size() || !memberList.size())
        return false;

    Document document;
    Value obj(kObjectType);
    {
        Value key; //(strMember, document.GetAllocator());
        Value val; //(strValue , document.GetAllocator());
        key.SetString(StringRef(strMember.c_str()), strMember.size());
        val.SetString(StringRef( strValue.c_str()), strMember.size());
        obj.AddMember(key, val, document.GetAllocator());

        Value aReg(kArrayType);
        Document::AllocatorType& alloc = document.GetAllocator();

        for (auto file : fileList)
        {
            std::string strData;
            std::string filename(file);
            if (!readFile(file, strData))
                continue;
            Value regObj(kObjectType);
            for (auto member : memberList)
            {
                std::string strValue;
                std::string strLbl(member);
                if (!getValueMessage(strData, member, strValue))
                    continue;
                Value memb; //(strMember, document.GetAllocator());
                Value membVal; //(strValue , document.GetAllocator());
                memb.SetString(StringRef(member.c_str()), member.size());
                membVal.SetString(StringRef( strValue.c_str()), strValue.size());
                regObj.AddMember(memb, membVal, document.GetAllocator());
            }
            aReg.PushBack(regObj, alloc);
        }
        obj.AddMember("registers", aReg, document.GetAllocator());
    }

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    // Output {"project":"rapidjson","stars":11}
    strPlainText = buffer.GetString();

    return true;
}

bool JsonParser::getValueMessage(const std::string& msg, const std::string& valueName, std::string& value)
{
    std::string::size_type pos_ini = msg.find(valueName);

    if (pos_ini == std::string::npos)
        return false;

    pos_ini += valueName.length()+1;

    if (pos_ini >= msg.length())
        return false;

    std::string::size_type pos_end = msg.find(',', pos_ini + 1);

    if (pos_end == std::string::npos)
    {
        pos_end = msg.find('\n', pos_ini + 1);
        if (pos_end == std::string::npos)
            pos_end = msg.length();
    }
    if (pos_end <= pos_ini)
        return false;

    value = msg.substr(pos_ini, pos_end - pos_ini);

    return true;
}

bool JsonParser::readFile(const std::string& filename, std::string& container)
{
    std::ifstream f;

    if (filename == "")
        return false;

    f.open(filename.c_str(), std::ios::in);

    if (!f.is_open())
        return false;

    f.seekg(0, std::ifstream::end);
    size_t fileSize = f.tellg();
    if (fileSize == 0)
        return false;

    f.seekg(0,std::ifstream::beg);

    char buffer[fileSize+1];

    f.read(buffer, fileSize);
    f.close();

    buffer[fileSize] = NULL_CHAR;

    container = buffer;
    return true;
}

//#define COMILLAS        std:;string('""');

void addMember(std::string& jsonText, const std::string& key, const std::string& val)
{
    jsonText += "\"";
    jsonText += key;
    jsonText += "\"";
    jsonText += ":";
    jsonText += "\"";
    jsonText += val;
    jsonText += "\"";
}

void addSeparator(std::string& jsonText)
{
    jsonText += ",";
}

void addKey(std::string& jsonText, const std::string& key)
{
    jsonText += "\"";
    jsonText += key;
    jsonText += "\"";
    jsonText += ":";
}

void addArray(std::string& jsonText)
{
    jsonText += "[";
}

void addObject(std::string& jsonText)
{
    jsonText += "{";
}

void closeArray(std::string& jsonText)
{
    jsonText += "]";
}

void closeObject(std::string& jsonText)
{
    jsonText += "}";
}

bool JsonParser::getPlaneText(const std::string& strMember, const std::string& strValue,
                            std::list<std::string>& fileList,
                            std::list<std::string>& memberList,
                            std::string& strPlainText)
{
    if (strMember == "" || strValue == "" || !fileList.size() || !memberList.size())
        return false;

    strPlainText = "";
    /*strPlainText = "{";
    addMember(strPlainText, strMember, strValue);
    addSeparator(strPlainText);
    addKey(strPlainText, "registers");*/
    addArray(strPlainText);

    int i = 0;
    for (auto file : fileList)
    {
        std::string strData;
        if (!readFile(file, strData))
            continue;
        if (i)
            addSeparator(strPlainText);
        addObject(strPlainText);
        int j = 0;
        for (auto member : memberList)
        {
            std::string strValue;
            std::string strLbl(member);
            if (!getValueMessage(strData, member, strValue))
                continue;
            if (j)
                addSeparator(strPlainText);
            addMember(strPlainText, member, strValue);
            ++j;
        }
        closeObject(strPlainText);
        ++i;
    }
    closeArray(strPlainText);
    //closeObject(strPlainText);
    /*std::cout  << "Len:\t" << strPlainText.length() << std::endl;
    std::cout << std::endl << strPlainText << std::endl;*/
    //strPlainText = buffer.GetString();

    return true;
}

