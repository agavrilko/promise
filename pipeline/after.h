#pragma once

#include "../concrete/sequence.h"
#include "../stream.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct After : public Link {
            After(std::shared_ptr<Concrete::Sequence> const& sequence) :
                _sequence(sequence) {}

            After(std::shared_ptr<Stream> const& stream) :
                After(std::make_shared<Concrete::Sequence>()) {
                    _sequence->add(stream);
                }

            std::shared_ptr<Stream> const commit() override {
                return _sequence;
            }

            After add(std::shared_ptr<Stream> const& stream) {
                _sequence->add(stream);
                return After(_sequence);
            }

        private:
            std::shared_ptr<Concrete::Sequence> const _sequence;

        };
    }
}
