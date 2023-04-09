#pragma once

#include <memory>
#include "../stream.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct Switch : public Link {
            Switch(std::shared_ptr<Stream> const& stream) :
                _stream(stream) {}

            std::shared_ptr<Stream> const commit() override {
                return _stream;
            }

            template <class E>
            Switch onReceive(std::function<std::shared_ptr<Stream> const (std::shared_ptr<E> const&)> const& func) {
                static_assert(std::is_base_of<Stream::Event, E>::value, "The E must be a Stream::Event derrived entity");
                return Switch(_stream);
            }

        private:
            std::shared_ptr<Stream> const _stream;
        };
    }
}
