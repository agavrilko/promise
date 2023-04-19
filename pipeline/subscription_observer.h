#pragma once

#include <functional>
#include <memory>
#include "../concrete/subscription_observer.h"
#include "../publisher.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct SubscriptionObserver : public Link {
        private:
            class Impl : public Concrete::SubscriptionObserver::Callback {
            public:
                Impl(std::function<void()> const& onEnter, std::function<void()> const& onExit) :
                _onEnter(onEnter),
                _onExit(onExit) {}

                void onEnter() override {
                    _onEnter();
                }

                void onExit() override {
                    _onExit();
                }

            private:
                std::function<void()> const _onEnter;
                std::function<void()> const _onExit;

            };

        public:
            SubscriptionObserver(std::shared_ptr<Publisher> const& publisher) :
            SubscriptionObserver(publisher, std::function<void()>(), std::function<void()>()) {}

            std::shared_ptr<Publisher> const commit() override {
                return std::make_shared<Concrete::SubscriptionObserver>(
                    _publisher,
                    std::make_shared<Impl>(_onEnter, _onExit)
                );
            }

            SubscriptionObserver onEnter(std::function<void()> const& func) {
                return SubscriptionObserver(_publisher, func, _onExit);
            }

            SubscriptionObserver onExit(std::function<void()> const& func) {
                return SubscriptionObserver(_publisher, _onEnter, func);
            }

        private:
            std::shared_ptr<Publisher> const _publisher;
            std::function<void()> _onEnter;
            std::function<void()> _onExit;

            SubscriptionObserver(
                std::shared_ptr<Publisher> const& publisher,
                std::function<void()> const& onEnter,
                std::function<void()> const& onExit
            ) :
            _publisher(publisher),
            _onEnter(onEnter),
            _onExit(onExit) {}

        };
    }
}
