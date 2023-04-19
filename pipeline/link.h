#pragma once

#include <memory>
#include "../publisher.h"
#include "listen.h"

namespace Promise {
    namespace Pipeline {
        struct Link {
            virtual ~Link() {}

            virtual std::shared_ptr<Publisher> const commit() = 0;

            template <typename T>
            T than() {
                return T(commit());
            }

            Listen listen() {
                return Listen(commit());
            }
        };
    }
}
