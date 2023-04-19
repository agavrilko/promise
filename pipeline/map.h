#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "../concrete/map.h"
#include "../publisher.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct Map : public Link {
        private:
            class Impl : public Promise::Concrete::Map::Converter {
            public:
                typedef std::shared_ptr<Promise::Publisher::Event> EventPtr;
                typedef std::function<EventPtr const(EventPtr const&)> EventMapper;

            public:
                Impl() :
                _mappers() {}

                std::shared_ptr<Promise::Publisher::Event> const map(std::shared_ptr<Promise::Publisher::Event> const& event) override {
                    for (auto& mapper : _mappers) {
                        auto const result = mapper(event);
                        if (result) {
                            return result;
                        }
                    }
                    return event;
                }

                void add(EventMapper const& mapper) {
                    _mappers.push_back(mapper);
                }

            private:
                std::vector<EventMapper> _mappers;
            };

        public:
            Map(std::shared_ptr<Publisher> const& publisher) :
            _publisher(publisher),
            _impl() {}

            std::shared_ptr<Publisher> const commit() override {
                return std::make_shared<Promise::Concrete::Map>(_publisher, _impl);
            }

            template <typename T>
            void on(std::function<std::shared_ptr<Promise::Publisher::Event> const(std::shared_ptr<T> const&)> const& mapper) {
                _impl->add([mapper](auto event) {
                    auto mapped = std::dynamic_pointer_cast<T>(event);
                    return mapped ? mapper(mapped) : nullptr;
                });
            }

        private:
            std::shared_ptr<Publisher> const _publisher;
            std::shared_ptr<Impl> const _impl;

        };
    }
}
