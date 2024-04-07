#include "base/event_loop.h"
#include <cassert> // 用于断言非空指针

#include "rtc_base/logging.h"

#define TRANS_TO_EV_MASK(mask) \
    (((mask) & EventLoop::READ ? EV_READ : 0) | ((mask) & EventLoop::WRITE ? EV_WRITE : 0))

#define TRANS_FROM_EV_MASK(mask) \
    (((mask) & EV_READ ? EventLoop::READ : 0) | ((mask) & EV_WRITE ? EventLoop::WRITE : 0))


namespace lrtc
{
    // uv_loop_t* EventLoop::loop{ nullptr };
    EventLoop::EventLoop(void *owner) : owner_(owner),
                                        loop_(ev_loop_new(EVFLAG_AUTO))
    {
        assert(loop_ != nullptr);
    }

    EventLoop::~EventLoop()
    {
    }
    void EventLoop::start()
    {
        // This should never happen.
        if (loop_ == nullptr){
            // MS_ABORT("DepLibUV::loop was not allocated");

        }

            ev_run(loop_);
    }
    void lrtc::EventLoop::stop()
    {
        // This should never happen.
        if (loop_ == nullptr){
            // MS_ABORT("DepLibUV::loop was not allocated");

        }

            ev_break(loop_, EVBREAK_ALL);
        // delete loop_;
    }

    struct ev_loop *EventLoop::GetLoop()
    {
        return loop_;
    }
    class IOWatcher
    {

    public:
        IOWatcher(EventLoop *el, io_cb_t cb, void *data) : el_(el), cb_(cb), data_(data)
        {
            io_.data = this;
        }
        ~IOWatcher() {}

    public:
        EventLoop *el_;
        ev_io io_;
        io_cb_t cb_;
        void *data_;
    };
    static void genric_io_cb(struct ev_loop */*loop*/, struct ev_io* io,int events){
        IOWatcher *watcher = (IOWatcher *)(io->data);
        watcher->cb_(watcher->el_,watcher,io->fd,TRANS_FROM_EV_MASK(events),watcher->data_);
   
    }
   
    IOWatcher *EventLoop::create_io_event(io_cb_t cb, void *data)
    {
        IOWatcher *w = new IOWatcher(this, cb, data);
        ev_init(&(w->io_), genric_io_cb);
        return w;
    }
    void EventLoop::start_io_event(IOWatcher *watcher, int fd, int mask)
    {
        assert(watcher != nullptr); // 确保 watcher 不是空指针
        struct ev_io *io = &(watcher->io_);
        // 如果之前有启动过 需要先停止然后 启动
        if (ev_is_active(io))
        {
            int active_events = TRANS_FROM_EV_MASK(io->events);
            int events = active_events | (~mask);
            if (events == active_events)
            {
                return; // 没有事件变化不需要处理
            }
            int events_ = TRANS_TO_EV_MASK(mask);
            ev_io_stop(loop_, io);
            ev_io_set(io, fd, events_);
            ev_io_start(loop_, io);
        }
        else
        {
            int events_ = TRANS_TO_EV_MASK(mask);
            ev_io_set(io, fd, events_);
            ev_io_start(loop_, io);
        }
    }

    void EventLoop::stop_io_event(IOWatcher *watcher,int fd,int mask)
    {
        assert(watcher != nullptr); // 确保 watcher 不是空指针

        struct  ev_io *io = &(watcher->io_);
        if (ev_is_active(io))
        {
            int active_events = TRANS_FROM_EV_MASK(io->events);
            int events = active_events & (~mask);
            if (events == active_events)
            {
                return; // 没有事件变化不需要处理
            }
            int events_ = TRANS_TO_EV_MASK(events);
            ev_io_stop(loop_, io);
            if (events != EV_NONE)
            {
                ev_io_set(io, fd, events_);
                ev_io_start(loop_, io);
            }
        }
        
    }
   void EventLoop::delete_io_event(IOWatcher *watcher)
    {
        struct  ev_io *io = &(watcher->io_);
        ev_io_stop(loop_, io);
        delete watcher;
    }
    void EventLoop::destroy_io_event(IOWatcher *watcher,int fd,int mask)
    {
        struct  ev_io *io = &(watcher->io_);
        ev_io_stop(loop_, io);
        delete watcher;
    }

   class TimerWatcher
   {
   public:
       TimerWatcher(EventLoop *el, timer_cb_t cb, void *data, bool need_repeat) : 
                                el_(el), cb_(cb), data_(data),need_repeat_(need_repeat)
       {
           timer_.data = this;
       }
       ~TimerWatcher() {}

   public:
       EventLoop *el_;
       ev_timer timer_;
       timer_cb_t cb_;
       void *data_;
       bool need_repeat_;
   };

    static void genric_timer_cb(struct ev_loop */*loop*/, struct ev_timer* timer,int events){
        TimerWatcher *watcher = (TimerWatcher *)(timer->data);
        watcher->cb_(watcher->el_,watcher,watcher->data_);
        if (watcher->need_repeat_)
        {
            ev_timer_again(watcher->el_->GetLoop(),timer);
        }
    }

    TimerWatcher* EventLoop::create_timer_event(timer_cb_t cb, void *data, bool need_repeat)
    {

        TimerWatcher *watcher = new TimerWatcher(this, cb, data, need_repeat);
        ev_init(&(watcher->timer_), genric_timer_cb);
        return watcher;
    }

    void EventLoop::start_timer_event(TimerWatcher *watcher, uint32_t usec)
    {
        struct  ev_timer *timer = &(watcher->timer_);
        float sec = float(usec) / 1000000.0;
        RTC_LOG(LS_INFO) << "EventLoop::start_timer_event timer :" << timer << ", loop_ = " << loop_;
        if (!watcher->need_repeat_)
        {
            ev_timer_stop(loop_, timer);
            ev_timer_set(timer, sec, 0);
            ev_timer_start(loop_, timer);
        }else{
            timer->repeat = sec;
            // ev_timer_set(timer, sec, sec);
            ev_timer_again(loop_,timer);
        }  
    }

    void EventLoop::stop_timer_event(TimerWatcher *watcher)
    {
       struct ev_timer *timer = &(watcher->timer_);
       ev_timer_stop(loop_, timer);
    }
    void EventLoop::delete_timer_event(TimerWatcher *watcher)
    {
        stop_timer_event(watcher);
        delete watcher;
    }

} // namespace lrtc
