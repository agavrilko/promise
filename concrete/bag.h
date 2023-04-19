#pragma once

#include <memory>
#include <vector>
#include "../cancellable.h"

namespace Promise {
    namespace Concrete {
        class Bag : public Cancellable {
        public:
            Bag() : _storage() {}

            virtual ~Bag() {
                for (auto& cancellable : _storage) {
                    cancellable->cancel();
                }
            }

            void cancel() override {
                if (_storage.size() > 0) {
                    decltype(_storage) tmp(_storage);
                    _storage.clear();
                    for (auto& cancellable : tmp) {
                        cancellable->cancel();
                    }
                }
            }

            void add(std::shared_ptr<Promise::Cancellable> const& cancellable) {
                _storage.push_back(cancellable);
            }

        private:
            std::vector<std::shared_ptr<Promise::Cancellable>> _storage;

        };
    }
}
