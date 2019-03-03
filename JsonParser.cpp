#include "JsonParser.h"

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


JsonParser::JsonParser() : m_chPlainText(NULL), m_iPlainTextSize(0), m_bDocJsoReady(false)
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
    m_chPlainText = new char[strPlaneText.length()+1];
    memcpy(m_chPlainText, strPlaneText.c_str(),strPlaneText.length());
    m_iPlainTextSize = strPlaneText.length();
    *(m_chPlainText+m_iPlainTextSize) = 0x00;
    m_bDocJsoReady = !m_docJson.Parse(m_chPlainText).HasParseError();

}

void JsonParser::createIdTable()
{
    if (m_docJson.IsArray())
        return;

    static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
    for (Value::ConstMemberIterator itr = m_docJson.MemberBegin(); itr != m_docJson.MemberEnd(); ++itr)
    {
        //printf("Type of member %s is %s\n", itr->name.GetString(), kTypeNames[itr->value.GetType()]);
        std::cout << "Type of member " << itr->name.GetString() << " is" << kTypeNames[itr->value.GetType()] << std::endl;
    }
    std::cout << std::endl;

}
