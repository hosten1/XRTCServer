/**
 * Copyright © 2024 L YongMeng. All rights reserved.
 *
 * @file stun_request.h
 * @author: L YongMeng
 * @email: 283018350@qq.com
 * @date: 12:15:47, 四月 20, 2024
 */

#ifndef __LYMSDK_LRTCSERVER_SRC_ICE_STUN_REQUEST_H_
#define __LYMSDK_LRTCSERVER_SRC_ICE_STUN_REQUEST_H_
#include <map>

#include <rtc_base/third_party/sigslot/sigslot.h>

#include "ice/stun.h"

namespace lrtc
{

    class StunRequest;

    class StunRequestManager
    {
    public:
        StunRequestManager() = default;
        ~StunRequestManager();

        void send(StunRequest *request);
        bool check_response(StunMessage *msg);
        void remove(StunRequest *request);

        sigslot::signal3<StunRequest *, const char *, size_t> signal_send_packet;

    private:
        typedef std::map<std::string, StunRequest *> RequestMap;
        RequestMap requests_;
    };

    class StunRequest
    {
    public:
        StunRequest(StunMessage *request);
        virtual ~StunRequest();

        int type() const { return msg_->type(); }
        const std::string &id() { return msg_->transaction_id(); }
        void construct();
        void send();
        void set_manager(StunRequestManager *manager) { manager_ = manager; }
        int elapsed();

    protected:
        virtual void prepare(StunMessage *) {}
        virtual void on_request_response(StunMessage *) {}
        virtual void on_request_error_response(StunMessage *) {}

        friend class StunRequestManager;

    private:
        StunMessage *msg_ = nullptr;
        StunRequestManager *manager_ = nullptr;
        int64_t ts_ = 0;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_ICE_STUN_REQUEST_H_
