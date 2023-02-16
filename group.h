#pragma once

#include <map>

namespace Promise {
    namespace Concrete {
        class Group : public Stream {
        private:
            class CompositeSubscriber : public Stream::Subscriber {
            private:
                enum State {
                    active,
                    completed_,
                    failed_,
                };

            public:
                CompositeSubscriber(int count) :
                    _state(State::active),
                    _counter(count),
                    _failure(nullptr),
                    _bag() {}

                virtual void completed() override {
                    switch (_state) {
                        case State::active:
                            if (_counter > 0) {
                                if (--_counter == 0) {
                                    _state = State::completed_;
                                    _bag->completed();
                                    _bag->clear();
                                }
                            }
                            break;

                        default:
                            break;

                    }
                }

                virtual void failed(std::shared_ptr<Error> const& error) override {
                    switch (_state) {
                        case State::active:
                            _state = State::failed_;
                            _counter = 0;
                            _failure = error;
                            _bag->failed(error);
                            _bag->clear();
                            break;

                        default:
                            break;

                    }
                }

                virtual void received(std::shared_ptr<Event> const& event) override {
                    switch (_state) {
                        case State::active:
                            _bag->received(event);
                            break;

                        default:
                            break;

                    }
                }

                std::shared_ptr<Cancellable> const add(std::shared_ptr<Stream::Subscriber> const& subscriber) {
                    switch (_state) {
                        case State::active:
                            return std::make_shared<Internal::Subscription>(
                                _bag->add(subscriber),
                                _bag
                            );

                        case State::completed_:
                            subscriber->completed();
                            break;

                        case State::failed_:
                            subscriber->failed(_failure);
                            break;
                    }
                    return std::shared_ptr<Internal::Subscription::Empty>();
                }

            private:
                State _state;
                int _counter;
                std::shared_ptr<Future::Error> _failure;
                std::shared_ptr<Internal::Subscription::Bag> _bag;

            };

            class CompositeCancellable : public Cancellable {
            public:
                CompositeCancellable()
                    : _cancellable() {}

                virtual void cancel() {
                    if (_cancellable.size() > 0) {
                        for (auto const& cancellable : _cancellable) {
                            cancellable->cancel();
                        }
                        _cancellable.clear();
                    }
                }

                void add(std::shared_ptr<Cancellable> const& cancellable) {
                    _cancellable.push_back(cancellable);
                }

            private:
                std::vector<std::shared_ptr<Cancellable>> _cancellable;
            };

        public:
            Group(std::vector<std::shared_ptr<Stream>> const& streams) :
                _needsToStart(true),
                _streams(streams),
                _subscriber(std::make_shared<CompositeSubscriber>(streams.size())) {}

            virtual std::shared_ptr<Cancellable> const await(std::shared_ptr<Completion> const& completion) override {
                return listen(std::make_shared<Stream::SubscriberFromCompletion>(completion));
            }

            virtual std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                if (_needsToStart) {
                    _needsToStart = false;
                    auto const cancellable = std::make_shared<CompositeCancellable>();
                    for (auto const& stream : _streams) {
                        cancellable->add(stream->listen(_subscriber));
                    }
                    cancellable->add(_subscriber->add(subscriber));
                    return cancellable;
                }
                return _subscriber->add(subscriber);
            }

        private:
            bool _needsToStart;
            std::vector<std::shared_ptr<Stream>> const _streams;
            std::shared_ptr<CompositeSubscriber> const _subscriber;

        };
    }
}
