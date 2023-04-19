#pragma once

#include <memory>
#include "../publisher.h"

namespace Promise {
    namespace Concrete {
        class Map : public Publisher {
        public:
            class Converter {
            public:
                virtual std::shared_ptr<Event> const map(std::shared_ptr<Event> const&) = 0;
            };

        private:
            class Mapper : public Publisher::Subscriber {
            public:
                Mapper(std::shared_ptr<Converter> const& converter, std::shared_ptr<Subscriber> const& subscriber) :
                _converter(converter),
                _subscriber(subscriber) {}

                void sink(std::shared_ptr<Event> const& event) override {
                    _subscriber->sink(_converter->map(event));
                }

            private:
                std::shared_ptr<Converter> const _converter;
                std::shared_ptr<Subscriber> const _subscriber;
            };

        public:
            Map(std::shared_ptr<Publisher> const& publisher, std::shared_ptr<Converter> const& converter) :
            _publisher(publisher),
            _converter(converter) {}

            std::shared_ptr<Cancellable> const listen(std::shared_ptr<Subscriber> const& subscriber) override {
                return _publisher->listen(std::make_shared<Mapper>(_converter, subscriber));
            }

        private:
            std::shared_ptr<Publisher> const _publisher;
            std::shared_ptr<Converter> const _converter;

        };
    }
}
