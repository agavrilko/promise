#pragma once

#include <memory>
#include "stream.h"

namespace Promise {
    namespace Concrete {
        class Empty : public Stream {
        private:
            class Waiter : public Cancellable {
            public:
                Waiter() {}

                void cancel() override {}
            };

        public:
            Empty() {}

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                return std::make_shared<Waiter>();
            }
        };
    }
}
