#pragma once

#include <memory>
#include "../publisher.h"
#include "../queue.h"

namespace Promise {
    namespace Pipeline {
        struct Queue : public Link {
            Queue(std::shared_ptr<Publisher> const& publisher) :
            _publisher(publisher) {}

            std::shared_ptr<Publisher> const commit() override {
                return _publisher;
            }

            void to(std::shared_ptr<Promise::Queue> const& queue) {
                queue->add(commit());
            }

        private:
            std::shared_ptr<Publisher> const _publisher;
        };
    }
}
