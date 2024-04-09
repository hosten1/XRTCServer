#ifndef __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_

#include <string>
namespace lrtc {
    class SessionDescription
    {

    public:
        enum SdpType {
            kOffer,
            kAnswer,
            kPranswer,
            kRollback
        };
        SessionDescription(SdpType type);
        ~SessionDescription();

        SdpType type() const { return type_; }
        std::string to_string();

     private:
        SdpType type_;
    };

    
 
    


}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_
