#pragma once

#include <functional>
#include "../publisher.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct Future : public Link {
            Future(std::function<void(std::shared_ptr<Publisher::Subscriber> const&)> const& body) :
            _body(body) {}

            std::shared_ptr<Publisher> const commit() override {
                return std::make_shared<Concrete::Future>(_body);
            }

        private:
            std::function<void(std::shared_ptr<Publisher::Subscriber> const&)> const _body;
        };
    }
}
