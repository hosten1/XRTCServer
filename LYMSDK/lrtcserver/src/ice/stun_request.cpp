#include <rtc_base/helpers.h>
#include <rtc_base/logging.h>
#include <rtc_base/string_encode.h>
#include <rtc_base/time_utils.h>

#include "ice/stun_request.h"

namespace lrtc
{
    StunRequestManager::~StunRequestManager()
    {
        while (requests_.begin() != requests_.end())
        {
            std::shared_ptr<StunRequest> request = requests_.begin()->second;
            requests_.erase(requests_.begin());
            if (request)
            {
                request.reset();
                request = nullptr;
            }
        }
    }

    void StunRequestManager::send(std::shared_ptr<StunRequest> request)
    {
        if (!request)
        {
            return;
        }
        request->set_manager(this);
        request->construct();
        requests_[request->id()] = request;
        request->send();
    }

    void StunRequestManager::remove(StunRequest *request)
    {
        auto iter = requests_.find(request->id());
        if (iter != requests_.end())
        {
            requests_.erase(iter);
        }
    }

    bool StunRequestManager::check_response(std::shared_ptr<StunMessage> msg)
    {
        RTC_DCHECK(msg);
        if (!msg)
        {
            return false;
        }

        auto iter = requests_.find(msg->transaction_id());
        if (iter == requests_.end())
        {
            return false;
        }

        std::shared_ptr<StunRequest> request = iter->second;
        RTC_DCHECK(request);
        if (msg->type() == get_stun_success_response(request->type()))
        {
            request->on_request_response(msg);
        }
        else if (msg->type() == get_stun_error_response(request->type()))
        {
            request->on_request_error_response(msg);
        }
        else
        {
            RTC_LOG(LS_WARNING) << "Received STUN binding response with wrong type="
                                << msg->type() << ", id=" << rtc::hex_encode(msg->transaction_id());

            request.reset();
            request = nullptr;
            return false;
        }

        if (request)
        {
            request.reset();
            request = nullptr;
        }

        return true;
    }

    StunRequest::StunRequest(std::shared_ptr<StunMessage> msg) : msg_(msg)
    {
        msg_->set_transaction_id(rtc::CreateRandomString(k_stun_transaction_id_length));
    }

    StunRequest::~StunRequest()
    {
        if (manager_)
        {
            manager_->remove(this);
        }

        if (msg_)
        {
            msg_.reset();
            msg_ = nullptr;
        }
    }

    void StunRequest::construct()
    {
        prepare(msg_);
    }

    void StunRequest::send()
    {
        ts_ = rtc::TimeMillis();
        rtc::ByteBufferWriter buf;
        if (!msg_->write(&buf))
        {
            return;
        }

        manager_->signal_send_packet(this, buf.Data(), buf.Length());
    }

    int StunRequest::elapsed()
    {
        return rtc::TimeMillis() - ts_;
    }
} // namespace lrtc