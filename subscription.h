#pragma once

#include <map>

namespace Promise {
    namespace Internal {
        class Subscription : public Cancellable {
        public:
            class Bag : public Stream::Subscriber {
            public:
                Bag() :
                    _seed(INT_MIN),
                    _map() {}

                virtual void completed() override {
                    for (auto const& i : _map) {
                        i.second->completed();
                    }
                }

                virtual void failed(std::shared_ptr<Future::Error> const& error) override {
                    for (auto const& i : _map) {
                        i.second->failed(error);
                    }
                }

                virtual void received(std::shared_ptr<Stream::Event> const& event) override {
                    for (auto const& i : _map) {
                        i.second->received(event);
                    }
                }

                virtual int const add(std::shared_ptr<Stream::Subscriber> const& subscriber) {
                    auto const key = _nextKey();
                    _map[key] = subscriber;
                    return key;
                }

                virtual void remove(int const key) {
                    auto const& i = _map.find(key);
                    if (i != _map.end()) {
                        _map.erase(i);
                    }
                }

                virtual void clear() {
                    _map.clear();
                }

            private:
                int _seed;
                std::map<int, std::shared_ptr<Stream::Subscriber>> _map;

                int const _nextKey() {
                    switch (_seed) {
                        case INT_MAX:
                            _seed = INT_MIN;
                            return INT_MAX;

                        default:
                            return _seed++;
                    }
                }
            };

            class Empty : public Cancellable {
            public:
                virtual void cancel() override {}
            };

        public:
            Subscription(int const key, std::shared_ptr<Bag> const& bag) :
                _key(key),
                _bag(bag),
                _isActive(true) {}

            virtual ~Subscription() {
                if (_isActive) {
                    _bag->remove(_key);
                }
            }

            virtual void cancel() override {
                if (_isActive) {
                    _bag->remove(_key);
                    _isActive = false;
                }
            }

        private:
            int const _key;
            std::shared_ptr<Bag> const _bag;
            bool _isActive;

        };
    }
}
