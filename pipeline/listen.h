#pragma once

#include <functional>
#include <memory>
#include "../internal/switch.h"
#include "../publisher.h"

namespace Promise {
    namespace Pipeline {
        struct Listen {
        private:
            struct Impl : public Publisher::Subscriber {
                Impl() :
                _funcs() {}

                void sink(std::shared_ptr<Publisher::Event> const& event) override {
                    for (auto& func : _funcs) {
                        func(event);
                    }
                }

                void add(std::function<void(std::shared_ptr<Publisher::Event> const&)> const& func) {
                    _funcs.push_back(func);
                }

            private:
                std::vector<std::function<void(std::shared_ptr<Publisher::Event> const&)>> _funcs;
            };

        public:
            Listen(std::shared_ptr<Publisher> const& publisher) :
            Listen(publisher, std::make_shared<Impl>()) {}

            template <typename T>
            Listen on(std::function<void(std::shared_ptr<T> const&)> const& func) {
                _impl->add([func](auto event) {
                    Internal::Switch(event)
                        .on<T>(func);
                });
                return Listen(_publisher, _impl);
            }

            std::shared_ptr<Cancellable> const commit() {
                return _publisher->listen(_impl);
            }

            void commit(std::shared_ptr<Concrete::Bag> const& bag) {
                bag->add(commit());
            }

        private:
            std::shared_ptr<Publisher> const _publisher;
            std::shared_ptr<Impl> const _impl;

            Listen(std::shared_ptr<Publisher> const& publisher, std::shared_ptr<Impl> const& impl) :
            _publisher(publisher),
            _impl(impl) {}

        };
    }
}
