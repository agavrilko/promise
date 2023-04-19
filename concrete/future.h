#pragma once

#include <functional>
#include <memory>
#include "../internal/bag.h"
#include "../internal/switch.h"
#include "../publisher.h"

namespace Promise {
    namespace Concrete {
        class Future : public Publisher {
        public:
            class Cancelled : public Failure {
            };

        private:
            class Impl : public std::enable_shared_from_this<Impl>, public Internal::Bag {
            private:
                enum class State {
                    idle,
                    running,
                    done,
                };

            public:
                Impl(std::function<void(std::shared_ptr<Subscriber> const&)> const& body) :
                Bag(),
                _body(body),
                _result(),
                _state(State::idle) {}

                void sink(std::shared_ptr<Event> const& event) override {
                    Internal::Switch(event)
                        .on<Done>(std::bind(&Impl::_onDone, this, std::placeholders::_1))
                        .on<Event>(std::bind(&Impl::_onEvent, this, std::placeholders::_1));
                }

                void add(std::shared_ptr<Subscriber> const& subscriber) override {
                    switch (_state) {
                        case State::idle: {
                            Bag::add(subscriber);
                            _state = State::running;
                            _body(shared_from_this());
                            break;
                        }

                        case State::running:
                            Bag::add(subscriber);
                            break;

                        case State::done:
                            subscriber->sink(_result);
                            break;
                    }
                }

                void remove(std::shared_ptr<Publisher::Subscriber> const& subscriber) override {
                    Bag::remove(subscriber);
                    if (_subscribers.size() == 0 && _state == State::running) {
                        _result = std::make_shared<Cancelled>();
                        _state = State::done;
                    }
                }

            private:
                std::function<void(std::shared_ptr<Subscriber> const&)> const _body;
                std::shared_ptr<Event> _result;
                State _state;

                void _onEvent(std::shared_ptr<Event> const& event) {
                    switch (_state) {
                        case State::running:
                            Bag::sink(event);
                            break;

                        default:
                            break;
                    }
                }

                void _onDone(std::shared_ptr<Done> const& done) {
                    switch (_state) {
                        case State::done:
                            break;

                        default: {
                            auto const tmp(_subscribers);
                            _result = done;
                            _state = State::done;
                            _subscribers.clear();
                            for (auto& sub : tmp) {
                                sub->sink(done);
                            }
                            break;
                        }
                    }
                }
            };

        public:
            Future(std::function<void(std::shared_ptr<Subscriber> const&)> const& body) :
            _impl(std::make_shared<Impl>(body)) {}

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                _impl->add(subscriber);
                return std::make_shared<Internal::Subscription>(_impl, subscriber);
            }

        private:
            std::shared_ptr<Impl> const _impl;
        };
    }
}
