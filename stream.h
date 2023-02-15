#pragma once

namespace Promise {
    class Cancellable {
    public:
        virtual ~Cancellable() {}

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
            virtual ~Completion() {}

            virtual void completed() = 0;
            virtual void failed(std::shared_ptr<Error> const& error) = 0;
        };

    public:
        virtual ~Future() {}

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

    public:
        virtual ~Stream() {}

        virtual std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) = 0;
    };
}
