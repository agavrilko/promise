#pragma once

#include "../concrete/map.h"
#include "../stream.h"
#include "link.h"

namespace Promise {
    namespace Pipeline {
        struct Map : public Link {
            Map(std::shared_ptr<Concrete::Map> const& map) :
                _map(map) {}

            Map(std::shared_ptr<Stream> const& stream) :
                Map(std::make_shared<Concrete::Map>(stream)) {}

            std::shared_ptr<Stream> const commit() override {
                return _map;
            }

            template <typename E>
            Map onReceive(std::function<void(std::shared_ptr<E> const&, std::function<void(std::shared_ptr<Stream::Event> const&)>)> const& func) {
                static_assert(std::is_base_of<Stream::Event, E>::value, "The E must be a Stream::Event derrived entity");
                _map->map<E>(func);
                return Map(_map);
            }

        private:
            std::shared_ptr<Concrete::Map> const _map;
        };
    }
}
