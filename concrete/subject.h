#pragma once

#include <map>
#include <memory>
#include "../internal/subscription.h"
#include "../stream.h"

namespace Promise {
    namespace Concrete {
        class Subject : public Stream {
        protected:
            enum class State {
                active,
                completed,
                failed,
            };

        public:
            Subject() :
                _state(State::active),
                _shouldSendEvent(false),
                _bag(std::make_shared<Internal::Subscription::Bag>()),
                _lastEvent(nullptr),
                _failure(nullptr) {}

            virtual std::shared_ptr<Cancellable> const await(std::shared_ptr<Completion> const& completion) override {
                return listen(std::make_shared<Stream::SubscriberFromCompletion>(completion));
            }

            virtual std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                switch (_state) {
                    case State::active:
                        if (_shouldSendEvent) {
                            subscriber->received(_lastEvent);
                        }
                        return std::make_shared<Internal::Subscription>(
                            _bag->add(subscriber),
                            _bag
                        );

                    case State::completed:
                        subscriber->completed();
                        break;

                    case State::failed:
                        subscriber->failed(_failure);
                        break;
                }
                return std::make_shared<Internal::Subscription::Empty>();
            }

            std::shared_ptr<Event> const lastEvent() const {
                return _lastEvent;
            }

            void complete() {
                switch (_state) {
                    case State::active: {
                        _state = State::completed;
                        _bag->completed();
                        _bag->clear();
                        break;
                    }

                    default:
                        break;
                }
            }

            void fail(std::shared_ptr<Error> const& error) {
                switch (_state) {
                    case State::active:
                        _state = State::failed;
                        _failure = error;
                        _bag->failed(error);
                        _bag->clear();
                        break;

                    default:
                        break;
                }
            }

            void receive(std::shared_ptr<Event> const& event) {
                switch (_state) {
                    case State::active:
                        _shouldSendEvent = true;
                        _lastEvent = event;
                        _bag->received(event);
                        break;

                    default:
                        break;
                }
            }

        private:
            State _state;
            bool _shouldSendEvent;
            std::shared_ptr<Internal::Subscription::Bag> const _bag;
            std::shared_ptr<Event> _lastEvent;
            std::shared_ptr<Error> _failure;

        };
    }
}
