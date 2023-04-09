#pragma once

#include "../stream.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct StartWith : public Link {
            StartWith(std::shared_ptr<Stream> const& stream) :
                _stream(stream) {}

            std::shared_ptr<Stream> const commit() override {
                return _stream;
            }

            StartWith add(std::shared_ptr<Stream::Event> const& event) {
                return StartWith(_stream);
            }

        private:
            std::shared_ptr<Stream> const _stream;
        };
    }
}
