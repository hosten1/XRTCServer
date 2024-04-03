#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_EVENT_LOOP_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_EVENT_LOOP_H_

#include "ev.h"

namespace lrtc
{
    class EventLoop;
    class IOWatcher;
    typedef void (*io_cb_t)(EventLoop *el, IOWatcher *w, int fd, int events, void *data);
    class EventLoop
    {

    public:
        enum
        {
            READ = 0x1,
            WRITE = 0x2
        };
        EventLoop(void *owner);
        ~EventLoop();
        void start();
        void stop();
        struct ev_loop *GetLoop();

        IOWatcher *create_io_event(io_cb_t cb, void *data);

    private:
        void *owner_;
        struct ev_loop *loop_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_EVENT_LOOP_H_
