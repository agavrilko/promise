#pragma once

#include <memory>
#include "../internal/closure.h"
#include "../internal/subscription.h"
#include "../stream.h"

namespace Promise {
    namespace Concrete {
        class Switch : public Stream {
        private:
            class Mapper {
            public:
                std::shared_ptr<Stream> const map(std::shared_ptr<Stream::Event> const& event) {
                    return nullptr;
                }

            private:
            };

            class Switcher : public Stream::Subscriber {
            public:
                Switcher(std::shared_ptr<Mapper> const& mapper, std::shared_ptr<Stream::Subscriber> const& subscriber) :
                    _mapper(mapper),
                    _subscriber(subscriber) {}

                void completed() override {
                    _disposeBag();
                    _subscriber->completed();
                }

                void failed(std::shared_ptr<Future::Error> const& error) override {
                    _disposeBag();
                    _subscriber->failed(error);
                }

                void received(std::shared_ptr<Stream::Event> const& event) override {
                    _disposeBag();
                    _stream = _mapper->map(event);
                    if (_stream) {
                        _bag = _stream->listen(_subscriber);
                    }
                }

            private:
                std::shared_ptr<Mapper> const _mapper;
                std::shared_ptr<Subscriber> const _subscriber;
                std::shared_ptr<Stream> _stream;
                std::shared_ptr<Cancellable> _bag;

                void _disposeBag() {
                    if (_bag) {
                        _bag->cancel();
                        _bag = nullptr;
                    }
                }
            };

        public:
            Switch(std::shared_ptr<Stream> const& stream) :
                _stream(stream),
                _mapper(std::make_shared<Mapper>()) {}

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                return _stream->listen(std::make_shared<Switcher>(_mapper, subscriber));
            }

        private:
            std::shared_ptr<Stream> const _stream;
            std::shared_ptr<Mapper> const _mapper;
        }
    };
}
