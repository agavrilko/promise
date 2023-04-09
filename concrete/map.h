#pragma once

#include <vector>
#include <memory>
#include "../stream.h"

namespace Promise {
    namespace Concrete {
        class Map : public Stream {
        private:
            class Impl {
            public:
                typedef std::function<void(std::shared_ptr<Stream::Event> const&, std::function<void(std::shared_ptr<Stream::Event> const&)>)> Mapper;

                Impl() :
                    _mappers() {}

                void add(Mapper const& mapper) {
                    _mappers.push_back(mapper);
                }

                void map(std::shared_ptr<Stream::Subscriber> const& subscriber, std::shared_ptr<Stream::Event> const& event) const {
                    for (auto& mapper : _mappers) {
                        mapper(event, std::bind(&Stream::Subscriber::received, subscriber, std::placeholders::_1));
                    }
                }

            private:
                std::vector<Mapper> _mappers;
            };

            class Mapper : public Stream::Subscriber {
            public:
                Mapper(std::shared_ptr<Impl> const& impl, std::shared_ptr<Stream::Subscriber> const& subscriber) :
                    _impl(impl),
                    _subscriber(subscriber) {}

                void completed() override {
                    _subscriber->completed();
                }

                void failed(std::shared_ptr<Error> const& error) override {
                    _subscriber->failed(error);
                }

                void received(std::shared_ptr<Event> const& event) override {
                    _impl->map(_subscriber, event);
                }

            private:
                std::shared_ptr<Impl> const _impl;
                std::shared_ptr<Stream::Subscriber> const _subscriber;

            };

        public:
            Map(std::shared_ptr<Stream> const& stream) :
                _stream(stream),
                _impl(std::make_shared<Impl>()) {}

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                return _stream->listen(std::make_shared<Mapper>(_impl, subscriber));
            }

            template <class E>
            void map(std::function<void(std::shared_ptr<E> const&, std::function<void(std::shared_ptr<Stream::Event> const&)>)> const& func) {
                _impl->add([func](auto event, auto receive) {
                    auto const casted = std::dynamic_pointer_cast<E>(event);
                    if (casted) {
                        func(casted, receive);
                    }
                });
            }

        private:
            std::shared_ptr<Stream> const _stream;
            std::shared_ptr<Impl> const _impl;

        };
    }
}
