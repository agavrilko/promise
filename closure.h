#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "stream.h"

namespace Promise {
    namespace Concrete {
        class Closure : public Stream::Subscriber {
        public:
            Closure() :
                _onComplete(),
                _onFail(),
                _onReceive() {}

            virtual void completed() {
                for (auto const& handler : _onComplete) {
                    handler();
                }
            }

            virtual void failed(std::shared_ptr<Future::Error> const& error) {
                for (auto const& handler : _onFail) {
                    handler(error);
                }
            }

            virtual void received(std::shared_ptr<Stream::Event> const& event) {
                for (auto const& handler : _onReceive) {
                    handler(event);
                }
            }

            void onComplete(std::function<void(void)> const& handler) {
                _onComplete.push_back(handler);
            }

            template <typename E>
            void onFail(std::function<void(std::shared_ptr<E> const&)> const& handler) {
                _onFail.push_back([handler](std::shared_ptr<Future::Error> const& e) {
                    auto const& error = std::static_pointer_cast<E>(e);
                    if (error != nullptr) {
                        handler(error);
                    }
                });
            }

            template <typename T>
            void onReceive(std::function<void(std::shared_ptr<T> const&)> const& handler) {
                _onReceive.push_back([handler](std::shared_ptr<Stream::Event> const& e) {
                    auto const& event = std::static_pointer_cast<T>(e);
                    if (event != nullptr) {
                        handler(event);
                    }
                });
            }

        private:
            std::vector<std::function<void(void)>> _onComplete;
            std::vector<std::function<void(std::shared_ptr<Future::Error> const&)>> _onFail;
            std::vector<std::function<void(std::shared_ptr<Stream::Event> const&)>> _onReceive;
        };
    }
}