#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_EVENT_LOOP_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_EVENT_LOOP_H_

#include "ev.h"
#include <stdint.h>

namespace lrtc
{
    class EventLoop;
    class IOWatcher;
    class TimerWatcher;

    typedef void (*io_cb_t)(EventLoop *el, IOWatcher *w, int fd, int events, void *data);
    typedef void (*timer_cb_t)(EventLoop *el, TimerWatcher *w, void *data);
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

        IOWatcher* create_io_event(io_cb_t cb, void *data);
        void start_io_event(IOWatcher* watcher,int fd,int mask);
        //停止io event
        void stop_io_event(IOWatcher* watcher,int fd,int mask);
        void delete_io_event(IOWatcher* watcher);
        void destroy_io_event(IOWatcher* watcher,int fd,int mask);

        uint32_t now_time_usec(){
            return static_cast<uint32_t>( ev_now(loop_)*1000000);
        }

        void* get_owner(){
            return owner_;
        }


        TimerWatcher* create_timer_event(timer_cb_t cb, void *data,bool need_repeat);
        void start_timer_event(TimerWatcher* watcher,uint32_t usec);
        void stop_timer_event(TimerWatcher* watcher);
        void delete_timer_event(TimerWatcher* watcher);

    private:
        void *owner_;
        struct ev_loop *loop_;
    };

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_EVENT_LOOP_H_
