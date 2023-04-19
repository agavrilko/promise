#pragma once

#include <memory>
#include "../publisher.h"

namespace Promise {
    namespace Concrete {
        class SubscriptionObserver : public Publisher {
        public:
            class Callback {
            public:
                virtual void onEnter() = 0;
                virtual void onExit() = 0;
            };

        private:
            class Subscription : public Cancellable {
            public:
                Subscription(std::shared_ptr<Callback> const& callback, std::shared_ptr<Cancellable> const& subscription) :
                _callback(callback),
                _subscription(subscription) {
                    _callback->onEnter();
                }

                ~Subscription() {
                    if (_subscription) {
                        _callback->onExit();
                        _subscription->cancel();
                    }
                }

                void cancel() override {
                    if (_subscription) {
                        _callback->onExit();
                        _subscription->cancel();
                        _subscription = nullptr;
                    }
                }

            private:
                std::shared_ptr<Callback> const _callback;
                std::shared_ptr<Cancellable> _subscription;
            };

        public:
            SubscriptionObserver(std::shared_ptr<Publisher> const& publisher, std::shared_ptr<Callback> const& callback) :
            _publisher(publisher),
            _callback(callback) {}

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                return std::make_shared<Subscription>(_callback, _publisher->listen(subscriber));
            }

        private:
            std::shared_ptr<Publisher> const _publisher;
            std::shared_ptr<Callback> const _callback;

        };
    }
}
