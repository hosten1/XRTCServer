
#include <sstream>

#include "pc/session_description.h"


namespace lrtc
{
    SessionDescription::SessionDescription(SdpType type)
    : type_(type)
    {
    }

    SessionDescription::~SessionDescription()
    {
    }

    std::string SessionDescription::to_string()
    {
        std::stringstream ss;
        //  version
        ss << "v=0\r\n";
        // session orifin
        // RFC 4566
        // o= <username> <sessionid> <sessionversion> <nettype> <addrtype> <unicast-address>
        ss << "o=- 0 2 IN IP4 0.0.0.0\r\n";
        // session name
        ss << "s=LymSDK\r\n";
        // time description 也就是会话的存在时间
        ss << "t=0 0\r\n";
        //  BUNDLE

        ss<< "a=msid-semantic: WMS\r\n";
        //RFC 4566
        // audio

        return ss.str();
    }

} // namespace lrtc
