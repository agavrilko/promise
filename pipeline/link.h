#pragma once

#include <memory>
#include "../stream.h"
#include "listen.h"
#include "redirect.h"

namespace Promise {
    namespace Pipeline {
        struct Link {
            virtual std::shared_ptr<Stream> const commit() = 0;

            template<typename T>
            T than() {
                static_assert(std::is_base_of<Link, T>::value, "The T must be a Link derrived entity");
                static_assert(std::is_constructible<T, std::shared_ptr<Stream> const&>::value, "The T doesn't accept stream at initialization");
                return T(commit());
            }

            Listen listen() {
                return Listen::to(commit());
            }

            Redirect redirect() {
                return Redirect::from(commit());
            }
        };
    }
}
