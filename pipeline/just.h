#pragma once

#include <memory>
#include "../publisher.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct Just : public Link {
            Just(std::shared_ptr<Publisher> const& publisher) :
            _publisher(publisher) {}

            std::shared_ptr<Publisher> const commit() override {
                return _publisher;
            }

        private:
            std::shared_ptr<Publisher> const _publisher;
        };
    }
}
