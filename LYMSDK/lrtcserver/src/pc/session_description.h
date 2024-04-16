#ifndef __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_
#define __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_

#include <string>
namespace lrtc {
     enum SdpType {
            kOffer  = 0,
            kAnswer = -1,
            kPranswer,
            kRollback
    };
    class SessionDescription
    {

    public:

        SessionDescription(SdpType type);
        ~SessionDescription();

        SdpType type() const { return type_; }
        std::string to_string();

     private:
        SdpType type_;
    };

    
 
    


}  // namespace lrtc

#endif  // __LYMSDK_LRTCSERVER_SRC_PC_SESSION_DESCRIPTION_H_
