#ifndef __LYMSDK_LRTCSERVER_SRC_BASE_LOCK_FREE_QUEUE_H_
#define __LYMSDK_LRTCSERVER_SRC_BASE_LOCK_FREE_QUEUE_H_

#include <atomic>

namespace lrtc
{
    // 一个生产者 一个消费者，同时指针的操作要是原子性
    template <typename T>

    class LockFreeQueue
    {
    private:
        struct Node
        {
            T value;
            Node *next;
            Node(const T& value) : value(value), next(nullptr) {}
            Node(T&& value) : value(value), next(nullptr) {}
        };
        Node *first_;
        Node *divider_;
        Node *last_;
        std::atomic<int> size_;

    public:
        LockFreeQueue(/* args */){
            first_ = last_ = divider_ = new Node(T());
            size_ = 0;
        }
        ~LockFreeQueue(){
            while (first_ != nullptr)
            {
                Node *tmp = first_;
                first_ = first_->next;
                delete tmp;
                size_--;

            }
            size_ = 0;
            
        }

        void produce(const T &value){
            Node *node = new Node(value);
            last_->next = node;
            last_ = last_->next;
            ++size_;
            //如果在生产的时候发现有人消费了，则删除
            while (divider_ != first_)
            {
                Node *tmp = first_;
                first_ = first_->next;
                delete tmp;
            }
            
        }
        bool consumer(T *result){
            if (divider_ != last_)
            {
                //这里可能有多个线程调用
                *result = divider_->next->value;
                divider_->next = divider_->next;
                --size_;
                return true;
            }
            return false;
            
        }

        bool empty()
        {
            return size_ == 0;
        }
        int size()
        {
            return size_;
        }


        void clear()
        {
            while (first_ != nullptr)
            {
                Node *tmp = first_;
                first_ = first_->next;
                delete tmp;
                size_--;
            }
            size_ = 0;
        }
    };// class LockFreeQueue end

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_LOCK_FREE_QUEUE_H_
