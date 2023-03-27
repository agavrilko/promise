#pragma once

#include <memory>
#include "../internal/closure.h"
#include "../stream.h"

namespace Promise {
    namespace Pipeline {
        struct Listen {
            static Listen to(std::shared_ptr<Stream> const& stream) {
                return Listen(stream, std::make_shared<Internal::Closure>());
            }

            Listen onComplete(std::function<void(void)> const& handler) {
                _closure->onComplete(handler);
                return Listen(_stream, _closure);
            }

            template <typename E>
            Listen onFail(std::function<void(std::shared_ptr<E> const&)> const& handler) {
                _closure->onFail<E>(handler);
                return Listen(_stream, _closure);
            }

            template <typename T>
            Listen onReceive(std::function<void(std::shared_ptr<T> const&)> const& handler) {
                _closure->onReceive<T>(handler);
                return Listen(_stream, _closure);
            }

            std::shared_ptr<Cancellable> const commit() {
                return _stream->listen(_closure);
            }

        private:
            std::shared_ptr<Stream> const _stream;
            std::shared_ptr<Internal::Closure> const _closure;

            Listen(std::shared_ptr<Stream> const& stream, std::shared_ptr<Internal::Closure> const& closure) :
                _stream(stream),
                _closure(closure) {}
        };
    }
}
