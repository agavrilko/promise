#pragma once

#include "../stream.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct Just : public Link {
            Just(std::shared_ptr<Stream> const& stream) :
                _stream(stream) {}

            std::shared_ptr<Stream> const commit() override {
                return _stream;
            }

        private:
            std::shared_ptr<Stream> const _stream;

        };
    }
}
