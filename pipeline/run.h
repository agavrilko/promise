#pragma once

#include "../concrete/generic.h"
#include "../stream.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct Run : public Link {
            Run(std::function<void(std::shared_ptr<Stream::Subscriber> const&)> const& body) :
                _body(body) {}

            Run() :
                _body() {}

            std::shared_ptr<Stream> const commit() override {
                return std::make_shared<Concrete::Generic>(_body);
            }

            Run main(std::function<void(std::shared_ptr<Stream::Subscriber> const&)> const& body) {
                return Run(body);
            }

        private:
            std::function<void(std::shared_ptr<Stream::Subscriber> const&)> _body;

        };
    }
}
