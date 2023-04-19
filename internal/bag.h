#pragma once

#include <memory>
#include <vector>
#include "../publisher.h"

namespace Promise {
    namespace Internal {
        class Bag : public Publisher::Subscriber {
        public:
            void sink(std::shared_ptr<Publisher::Event> const& event) {
                auto const tmp(_subscribers);
                for (auto& sub : tmp) {
                    sub->sink(event);
                }
            }

            virtual void add(std::shared_ptr<Subscriber> const& subscriber) {
                _subscribers.push_back(subscriber);
            }

            virtual void remove(std::shared_ptr<Subscriber> const& subscriber) {
                _subscribers.erase(
                    std::remove(
                        _subscribers.begin(),
                        _subscribers.end(),
                        subscriber
                    ),
                    _subscribers.end()
                );
            }

        protected:
            std::vector<std::shared_ptr<Subscriber>> _subscribers;
        };

        class Subscription : public Cancellable {
        public:
            Subscription(std::shared_ptr<Bag> const& bag, std::shared_ptr<Publisher::Subscriber> const& subscriber) :
            _bag(bag),
            _subscriber(subscriber) {}

            virtual ~Subscription() {
                if (_subscriber) {
                    _bag->remove(_subscriber);
                }
            }

            void cancel() override {
                if (_subscriber) {
                    _bag->remove(_subscriber);
                    _subscriber = nullptr;
                }
            }

        private:
            std::shared_ptr<Bag> const _bag;
            std::shared_ptr<Publisher::Subscriber> _subscriber;
        };
    }
}
