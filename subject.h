#pragma once

#include <map>
#include "stream.h"
#include "subscription.h"

namespace Promise {
    namespace Concrete {
        class Subject : public Stream {
        protected:
            enum State {
                active,
                completed,
                failed,
            };

        public:
            Subject() :
                _state(State::active),
                _bag(std::make_shared<Internal::Subscription::Bag>()),
                _failure(nullptr) {}

            virtual std::shared_ptr<Cancellable> const await(std::shared_ptr<Completion> const& completion) override {
                return listen(std::make_shared<Stream::SubscriberFromCompletion>(completion));
            }

            virtual std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                switch (_state) {
                    case State::active:
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

            virtual void complete() {
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

            virtual void fail(std::shared_ptr<Error> const& error) {
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

            virtual void receive(std::shared_ptr<Event> const& event) {
                switch (_state) {
                    case State::active:
                        _bag->received(event);
                        break;

                    default:
                        break;
                }
            }

        protected:
            State _state;

        private:
            std::shared_ptr<Internal::Subscription::Bag> const _bag;
            std::shared_ptr<Error> _failure;

        };
    }
}
