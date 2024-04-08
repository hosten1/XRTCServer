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
        alignas(64) std::atomic<Node *> first_;
        alignas(64) std::atomic<Node *> divider_;
        alignas(64) std::atomic<Node *> last_;
        alignas(64) std::atomic<int> size_;

    public:
        LockFreeQueue(): first_(new Node(T())), divider_(first_.load()), last_(first_.load()){
            // first_ = last_ = divider_ = new Node(T());
            size_ = 0;
        }
        ~LockFreeQueue(){
           Node *current = first_.load();
            while (current)
            {
                Node *temp = current;
                current = current->next;
                delete temp;
            }
            size_ = 0;
            
        }

        void produce(const T &value){
            Node *node = new Node(value);
            last_.load()->next = node;
            last_.store(node);
            ++size_;
            // //如果在生产的时候发现有人消费了，则删除
            // while (divider_ != first_)
            // {
            //     Node *tmp = first_;
            //     first_ = first_->next;
            //     delete tmp;
            // }
            
        }
        bool consumer(T *result){
            Node *current = divider_.load();
            Node *next = current->next;
            if (next)
            {
                *result = next->value;
                divider_.store(next);
                delete current;
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
           Node *current = first_.load();
            while (current)
            {
                Node *temp = current;
                current = current->next;
                delete temp;
            }
            divider_ = last_ = first_.load();
            size_ = 0;
        }
    };// class LockFreeQueue end

} // namespace lrtc

#endif // __LYMSDK_LRTCSERVER_SRC_BASE_LOCK_FREE_QUEUE_H_
