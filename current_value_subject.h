#pragma once

#include <memory>
#include "subject.h"

namespace Promise {
    namespace Concrete {
        class CurrentValueSubject : public Subject {
        public:
            CurrentValueSubject(std::shared_ptr<Event> const& event) :
                _shouldSendEvent(event != nullptr),
                _lastEvent(event) {}

            virtual std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                auto const subscription = Subject::listen(subscriber);
                if (_state == State::active && _shouldSendEvent) {
                    subscriber->received(_lastEvent);
                }
                return subscription;
            }

            virtual void receive(std::shared_ptr<Event> const& event) override {
                if (_state == State::active) {
                    _shouldSendEvent = true;
                    _lastEvent = event;
                }
                Subject::receive(event);
            }

            std::shared_ptr<Event> const currentValue() const {
                return _lastEvent;
            }

        private:
            bool _shouldSendEvent;
            std::shared_ptr<Event> _lastEvent;

        };
    }
}
