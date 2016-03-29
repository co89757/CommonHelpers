//
// Created by colin on 3/5/16.
//

#ifndef PROJECT_BLOCKQUEUE_H
#define PROJECT_BLOCKQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

namespace colinli {
    namespace utility {

        template<typename T>
        class BlockQueue {
        public:
            /** default constructor*/
            BlockQueue() { }

            void Push(T item) {
                std::lock_guard <std::mutex> lock(m);
                q.push(std::move(item));
                data_cond.notify_one();
            }

            bool TryPop(T &item) {
                std::lock_guard <std::mutex> lock(m);
                if (q.empty())
                    return false;
                item = std::move(q.front());
                q.pop();

            }

            void WaitAndPop(T& item)
            {
                std::unique_lock <std::mutex> lock(m);
                while(q.empty()){
                    data_cond.wait(lock);
                }
                item = std::move(q.front());
                q.pop();

            }

            bool Empty() const
            {
                std::lock_guard <std::mutex> lock(m);
                return q.empty();
            }

            size_t Size() const
            {
                std::lock_guard <std::mutex> lock(m);
                return q.size();
            }

        private:
            BlockQueue(const BlockQueue &) = delete;

            BlockQueue &operator=(const BlockQueue &) = delete;

            std::queue <T> q;
            mutable std::mutex m;
            std::condition_variable data_cond;


        };

    }
}


#endif //PROJECT_BLOCKQUEUE_H
