#pragma once

#include <memory>

namespace Promise {
    class Publisher {
    public:
        class Event {
        public:
            virtual ~Event() {}
        };

        class Done : public Event {
        };

        class Success : public Done {
        };

        class Failure : public Done {
        };

        class Subscriber {
        public:
            virtual void sink(std::shared_ptr<Event> const& event) = 0;

            template <typename T>
            void sink() {
                sink(std::make_shared<T>());
            }

            template <typename T, typename U>
            void sink(U const& arg) {
                sink(std::make_shared<T>(arg));
            }

            template <typename T, typename U1, typename U2>
            void sink(U1 const& arg1, U2 const& arg2) {
                sink(std::make_shared<T>(arg1, arg2));
            }

            template <typename T, typename U1, typename U2, typename U3>
            void sink(U1 const& arg1, U2 const& arg2, U3 const& arg3) {
                sink(std::make_shared<T>(arg1, arg2, arg3));
            }
        };

    public:
        virtual std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) = 0;
    };
}
