#pragma once

#include <vector>
#include <memory>
#include "../stream.h"

namespace Promise {
    namespace Concrete {
        class Generic : public Stream {
            class Impl : public Stream::Subscriber, public std::enable_shared_from_this<Impl> {
            private:
                enum class State {
                    idle,
                    executing,
                    failed,
                    completed,
                };

            public:
                Impl(std::function<void(std::shared_ptr<Stream::Subscriber> const&)> const& body) :
                    _body(body),
                    _subscribers(),
                    _failure(),
                    _state(State::idle) {}


                virtual void completed() override {
                    switch (_state) {
                        case State::idle:
                            _state = State::completed;
                            break;

                        case State::executing: {
                            auto const tmp(_subscribers);
                            _subscribers.clear();
                            _state = State::completed;
                            for (auto& sub : tmp) {
                                sub->completed();
                            }
                            break;
                        }

                        default:
                            break;
                    }
                }

                virtual void failed(std::shared_ptr<Future::Error> const& error) override {
                    switch (_state) {
                        case State::idle:
                            _failure = error;
                            _state = State::failed;
                            break;

                        case State::executing: {
                            auto const tmp(_subscribers);
                            _subscribers.clear();
                            _failure = error;
                            _state = State::failed;
                            for (auto& sub : tmp) {
                                sub->failed(error);
                            }
                            break;
                        }

                        default:
                            break;
                    }
                }

                virtual void received(std::shared_ptr<Stream::Event> const& event) override {
                    switch (_state) {
                        case State::executing: {
                            auto const tmp(_subscribers);
                            for (auto& sub : tmp) {
                                sub->received(event);
                            }
                            break;
                        }

                        default:
                            break;
                    }
                }

                void add(std::shared_ptr<Subscriber> const& subscriber) {
                    switch (_state) {
                        case State::idle: {
                            auto const impl = shared_from_this();
                            _subscribers.push_back(subscriber);
                            _state = State::executing;
                            _body(impl);
                            break;
                        }

                        case State::executing:
                            _subscribers.push_back(subscriber);
                            break;

                        case State::failed:
                            subscriber->failed(_failure);
                            break;

                        case State::completed:
                            subscriber->completed();
                            break;
                    }
                }

                void remove(std::shared_ptr<Subscriber> const& subscriber) {
                    _subscribers.erase(
                        std::remove(_subscribers.begin(), _subscribers.end(), subscriber),
                        _subscribers.end()
                    );
                }

            private:
                std::function<void(std::shared_ptr<Subscriber> const&)> const _body;
                std::vector<std::shared_ptr<Stream::Subscriber>> _subscribers;
                std::shared_ptr<Future::Error> _failure;
                State _state;

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
            Generic(std::function<void(std::shared_ptr<Stream::Subscriber> const&)> const& body) :
                _impl(std::make_shared<Impl>(body)) {}

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Stream::Subscriber> const& subscriber) override {
                _impl->add(subscriber);
                return std::make_shared<Subscription>(_impl, subscriber);
            }

        private:
            std::shared_ptr<Impl> const _impl;

        };
    }
}
