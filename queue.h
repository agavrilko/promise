#pragma once

#include <queue>
#include <memory>
#include "pipeline/just.h"
#include "publisher.h"

namespace Promise {
    class Queue : public std::enable_shared_from_this<Queue> {
    public:
        Queue() :
            _publishers(),
            _bag() {}

        void add(std::shared_ptr<Publisher> const& publisher) {
            _publishers.push_back(publisher);
            if (_publishers.size() == 1) {
                _execute();
            }
        }

        void remove(std::shared_ptr<Publisher> const& publisher) {
            if (_publishers.front() == publisher) {
                _bag = nullptr;
                _publishers.pop_front();
                _execute();
            } else {
                _publishers.erase(
                    std::find(
                        _publishers.begin(),
                        _publishers.end(),
                        publisher
                    ),
                    _publishers.end()
                );
            }
        }

        void clear() {
            while (_publishers.size()) {
                _publishers.pop_front();
            }
            if (_bag) {
                _bag->cancel();
                _bag = nullptr;
            }
        }

    private:
        std::deque<std::shared_ptr<Publisher>> _publishers;
        std::shared_ptr<Cancellable> _bag;

        void _execute() {
            if (_publishers.size()) {

                auto const publisher = _publishers.front();
                auto const bag = Pipeline::Just(publisher)
                    .listen()
                    .on<Publisher::Done>(std::bind(&Queue::_pop, shared_from_this(), publisher))
                    .commit();

                if (_publishers.front() == publisher) {
                    _bag = bag;
                }
            }
        }

        void _pop(std::shared_ptr<Publisher> const& publisher) {
            if (_publishers.size() && _publishers.front() == publisher) {
                _bag = nullptr;
                _publishers.pop_front();
                _execute();
            }
        }
    };
}
