#pragma once

#include <memory>
#include "../internal/bag.h"
#include "../publisher.h"

namespace Promise {
    namespace Concrete {
        class Subject : public Publisher {
        private:
            class Impl : public Internal::Bag {
            private:
                enum class State {
                    empty,
                    active,
                    done,
                };

            public:
                Impl() :
                _currentValue(nullptr),
                _state(State::active) {}

                void sink(std::shared_ptr<Event> const& event) override {
                    switch (_state) {
                        case State::empty:
                        case State::active:
                            _currentValue = event;
                            _state = _stateFromEvent(event);
                            Bag::sink(event);
                            break;

                        case State::done:
                            break;
                    }
                }

                void add(std::shared_ptr<Subscriber> const& subscriber) override {
                    switch (_state) {
                        case State::empty:
                            Bag::add(subscriber);
                            break;

                        case State::active:
                            Bag::add(subscriber);
                            subscriber->sink(_currentValue);
                            break;

                        case State::done:
                            break;
                    }
                }

            private:
                std::shared_ptr<Event> _currentValue;
                State _state;

                State _stateFromEvent(std::shared_ptr<Event> const& event) {
                    return std::dynamic_pointer_cast<Done>(event) ? State::done : State::active;
                }
            };

        public:
            Subject() :
            _impl(std::make_shared<Impl>()) {}

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                _impl->add(subscriber);
                return std::make_shared<Internal::Subscription>(_impl, subscriber);
            }

            void sink(std::shared_ptr<Event> const& event) {
                _impl->sink(event);
            }

        private:
            std::shared_ptr<Impl> const _impl;

        };
    }
}
