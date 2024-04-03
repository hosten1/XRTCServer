#include "base/event_loop.h"
#include "event_loop.h"

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

} // namespace lrtc
