#pragma once

#include <map>
#include "stream.h"

namespace Promise {
    namespace Concrete {
        class Subject : public Stream {
        protected:
            enum State {
                active,
                completed,
                failed,
            };

        private:
            class Subscribers {
            public:
                Subscribers() :
                    _seed(INT_MIN),
                    _map() {}

                int const add(std::shared_ptr<Subscriber> const& subscriber) {
                    auto const key = _nextKey();
                    _map[key] = subscriber;
                    return key;
                }

                void remove(int const key) {
                    if (_map.contains(key)) {
                        _map.erase(key);
                    }
                }

                void complete() {
                    if (_map.size() > 0) {
                        for (auto const& [key, sub] : _map) {
                            sub->completed();
                        }
                        _map.clear();
                    }
                }

                void fail(std::shared_ptr<Error> const& error) {
                    if (_map.size() > 0) {
                        for (auto const& [key, sub] : _map) {
                            sub->failed(error);
                        }
                        _map.clear();
                    }
                }

                void receive(std::shared_ptr<Event> const& event) {
                    for (auto const& [key, sub] : _map) {
                        sub->received(event);
                    }
                }


            private:
                int _seed;
                std::map<int, std::shared_ptr<Subscriber>> _map;

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

            class Subscription : public Cancellable {
            public:
                Subscription(int const key, std::shared_ptr<Subscribers> const& subscribers) :
                    _key(key),
                    _subscribers(subscribers),
                    _isActive(true) {}

                virtual ~Subscription() {
                    if (_isActive) {
                        _subscribers->remove(_key);
                    }
                }

                virtual void cancel() override {
                    if (_isActive) {
                        _subscribers->remove(_key);
                        _isActive = false;
                    }
                }

            private:
                int const _key;
                std::shared_ptr<Subscribers> const _subscribers;
                bool _isActive;

            };

            class EmptyCancellable : public Cancellable {
            public:
                virtual void cancel() override {
                }
            };

            class CompletionWrapper : public Subscriber {
            public:
                CompletionWrapper(std::shared_ptr<Future::Completion> const& completion) :
                    _completion(completion) {}

                virtual void completed() override {
                    _completion->completed();
                }

                virtual void failed(std::shared_ptr<Error> const& error) override {
                    _completion->failed(error);
                }

                virtual void received(std::shared_ptr<Event> const& event) override {
                }

            private:
                std::shared_ptr<Completion> const _completion;

            };

        public:
            Subject() :
                _state(State::active),
                _subscribers(std::make_shared<Subscribers>()),
                _failure(nullptr) {}

            virtual std::shared_ptr<Cancellable> const await(std::shared_ptr<Completion> const& completion) override {
                return listen(std::make_shared<CompletionWrapper>(completion));
            }

            virtual std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                switch (_state) {
                    case State::active:
                        return std::make_shared<Subscription>(
                            _subscribers->add(subscriber),
                            _subscribers
                        );

                    case State::completed:
                        subscriber->completed();
                        break;

                    case State::failed:
                        subscriber->failed(_failure);
                        break;
                }
                return std::make_shared<EmptyCancellable>();
            }

            virtual void complete() {
                switch (_state) {
                    case State::active: {
                        _state = State::completed;
                        _subscribers->complete();
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
                        _subscribers->fail(error);
                        break;

                    default:
                        break;
                }
            }

            virtual void receive(std::shared_ptr<Event> const& event) {
                switch (_state) {
                    case State::active:
                        _subscribers->receive(event);
                        break;

                    default:
                        break;
                }
            }

        protected:
            State _state;

        private:
            std::shared_ptr<Subscribers> const _subscribers;
            std::shared_ptr<Error> _failure;

        };
    }
}
