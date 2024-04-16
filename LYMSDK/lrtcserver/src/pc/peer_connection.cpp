#include "pc/peer_connection.h"
#include "peer_connection.h"

#include <rtc_base/logging.h>

namespace lrtc {
    PeerConnection::PeerConnection(EventLoop *el)
        : el_(el)
    {


    }

    PeerConnection::~PeerConnection()
    {

    }

    std::string PeerConnection::create_offer_sdp()
    {
        RTC_LOG(LS_INFO) << "create_offer_sdp";
        local_session_description_ = std::make_unique<SessionDescription>(SdpType::kOffer);

        return local_session_description_->to_string();
    }






























}  // namespace lrtc

