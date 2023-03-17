#pragma once

#include <memory>
#include "stream.h"

namespace Promise {
    namespace Concrete {
        class Empty : public Stream {
        private:
            class EmptyWaiter : public Cancellable {
            public:
                EmptyWaiter() {}

                void cancel() override {}
            };

        public:
            Empty() {}

            std::shared_ptr<Cancellable> const await(std::shared_ptr<Completion> const& completion) override {
                completion->completed();
                return std::make_shared<EmptyWaiter>();
            }

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                return await(subscriber);
            }
        };
    }
}
