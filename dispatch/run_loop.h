#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include "task.h"

namespace Promise {
    namespace Dispatch {
        class RunLoop : public std::enable_shared_from_this<RunLoop> {
        public:
            RunLoop(std::string const& name) :
            _name(name),
            _mutex(),
            _queue() {}

            std::string const name() const {
                return _name;
            }

            void add(std::shared_ptr<Task> const& task) {
                std::lock_guard<std::mutex> guard(_mutex);
                _queue.push_back(task);
            }

            void run() {
                auto const task = _next();
                if (task) {
                    task->execute();
                }
            }

        private:
            std::string const _name;
            std::mutex _mutex;
            std::deque<std::shared_ptr<Task>> _queue;

            std::shared_ptr<Task> const _next() {
                std::lock_guard<std::mutex> guard(_mutex);
                if (_queue.size()) {
                    auto const task = _queue.front();
                    _queue.pop_front();
                    return task;
                }
                return nullptr;
            }
        };
    }
}
