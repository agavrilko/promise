#pragma once

#include <memory>

namespace Promise {
    class Cancellable {
    public:
        virtual void cancel() = 0;
    };

    class Future {
    public:
        class Error {
        public:
            virtual ~Error() {}
        };

        class Completion {
        public:
            virtual void completed() = 0;
            virtual void failed(std::shared_ptr<Error> const& error) = 0;
        };

    public:
        virtual std::shared_ptr<Cancellable> const await(std::shared_ptr<Completion> const& completion) = 0;
    };

    class Stream : public Future {
    public:
        class Event {
        public:
            virtual ~Event() {}
        };

        class Subscriber : public Completion {
        public:
            virtual void received(std::shared_ptr<Event> const& event) = 0;
        };

        class SubscriberFromCompletion : public Stream::Subscriber {
        public:
            SubscriberFromCompletion(std::shared_ptr<Future::Completion> const& completion) :
                _completion(completion) {}

            virtual void completed() override {
                _completion->completed();
            }

            virtual void failed(std::shared_ptr<Future::Error> const& error) override {
                _completion->failed(error);
            }

            virtual void received(std::shared_ptr<Stream::Event> const& event) override {
            }

        private:
            std::shared_ptr<Future::Completion> const _completion;
        };

    public:
        virtual std::shared_ptr<Cancellable> const await(std::shared_ptr<Completion> const& completion) override {
            return listen(std::make_shared<SubscriberFromCompletion>(completion));
        }
        
        virtual std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) = 0;
    };
}