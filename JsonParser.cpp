#include "JsonParser.h"

using namespace std;
using namespace rapidjson;

/* Frame Format
    START_MARK | HEADER | DATA_LENGTH | DATA | VERIFICATOR| | STOP_MARK
     1            4         8           nn      16              1

     START_MARK: '<'
     STOP_MARK: '>'
*/

static const std::string::size_type st_MinFrameSize = (1+8+1+16+1);
static const char st_StartMark = '<';
static const char st_StopMark = '<';


JsonParser::JsonParser()
{
    //ctor
}

JsonParser::~JsonParser()
{
    //dtor
}

bool getFrame(std::string& strBuffer)
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
}

