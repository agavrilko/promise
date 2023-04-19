#pragma once

#include <memory>
#include "../publisher.h"

namespace Promise {
    namespace Internal {
        struct Switch {
            Switch(std::shared_ptr<Publisher::Event> const& event) :
            _event(event) {}

            template <typename T>
            Switch on(std::function<void(std::shared_ptr<T> const&)> const& body) {
                if (_event) {
                    auto const event = std::dynamic_pointer_cast<T>(_event);
                    if (event) {
                        body(event);
                        return Switch(nullptr);
                    }
                }
                return Switch(_event);
            }

        private:
            std::shared_ptr<Publisher::Event> const _event;
        };
    }
}

