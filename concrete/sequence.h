#pragma once

#include <memory>
#include <queue>
#include "../internal/subscription.h"
#include "../pipeline/listen.h"
#include "../stream.h"

namespace Promise {
    namespace Concrete {
        class Sequence : public Stream {
        private:
            class Impl : public std::enable_shared_from_this<Impl> {
            private:
                enum class State {
                    idle,
                    executing,
                };

            public:
                Impl() :
                    _state(State::idle),
                    _streams(),
                    _subscribers(),
                    _bag() {}

                void add(std::shared_ptr<Stream> const& stream) {
                    _streams.push(stream);
                }

                void add(std::shared_ptr<Stream::Subscriber> const& subscriber) {
                    _subscribers.push_back(subscriber);
                }

                void remove(std::shared_ptr<Stream::Subscriber> const& subscriber) {
                    _subscribers.erase(
                        std::remove(_subscribers.begin(), _subscribers.end(), subscriber),
                        _subscribers.end()
                    );
                    switch (_state) {
                        case State::idle:
                            break;

                        case State::executing:
                            if (_subscribers.size() == 0) {
                                _cleanup();
                            }
                            break;
                    }
                }

                void execute() {
                    switch (_state) {
                        case State::idle:
                            _execute();
                            break;

                        case State::executing:
                            break;
                    }
                }

            private:
                State _state;
                std::queue<std::shared_ptr<Stream>> _streams;
                std::vector<std::shared_ptr<Stream::Subscriber>> _subscribers;
                std::shared_ptr<Cancellable> _bag;

                void _execute() {
                    if (_streams.size()) {
                        auto& stream = _streams.front();
                        auto const impl = shared_from_this();
                        _state = State::executing;
                        _bag = Pipeline::Listen::to(stream)
                            .onReceive<Stream::Event>([impl](auto event) {
                                for (auto& sub : impl->_subscribers) {
                                    sub->received(event);
                                }
                            })
                            .onFail<Future::Error>([impl](auto error) {
                                auto subs = impl->_subscribers;
                                impl->_cleanup();
                                for (auto sub : subs) {
                                    sub->failed(error);
                                }
                            })
                            .onComplete([impl, stream] {
                                if (impl->_streams.front() == stream) {
                                    impl->_streams.pop();
                                    impl->_execute();
                                }
                            })
                            .commit();
                    } else {
                        _state = State::idle;
                        _bag = nullptr;
                        if (_subscribers.size()) {
                            auto subs = _subscribers;
                            _subscribers.clear();
                            for (auto sub : subs) {
                                sub->completed();
                            }
                        }
                    }
                }

                void _cleanup() {
                    _state = State::idle;
                    while (_streams.size()) {
                        _streams.pop();
                    }
                    if (_subscribers.size()) {
                        _subscribers.clear();
                    }
                    if (_bag) {
                        _bag->cancel();
                        _bag = nullptr;
                    }
                }
            };

            class Subscription : public Cancellable {
            public:
                Subscription(std::shared_ptr<Impl> const& impl, std::shared_ptr<Stream::Subscriber> const& subscriber) :
                    _impl(impl),
                    _subscriber(subscriber) {}

                virtual ~Subscription() {
                    if (_subscriber) {
                        _impl->remove(_subscriber);
                    }
                }

                void cancel() override {
                    if (_subscriber) {
                        _impl->remove(_subscriber);
                        _subscriber = nullptr;
                    }
                }

            private:
                std::shared_ptr<Impl> const _impl;
                std::shared_ptr<Stream::Subscriber> _subscriber;

            };

        public:
            Sequence() :
                _impl(std::make_shared<Impl>()) {}

            void add(std::shared_ptr<Stream> const& stream) {
                _impl->add(stream);
            }

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                _impl->add(subscriber);
                _impl->execute();
                return std::make_shared<Subscription>(_impl, subscriber);
            }

        private:
            std::shared_ptr<Impl> const _impl;
        };
    }
}
