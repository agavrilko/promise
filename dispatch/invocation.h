#pragma once

#include <functional>
#include <memory>
#include "run_loop.h"
#include "task.h"

namespace Promise {
    namespace Dispatch {
        struct Invocation {
        private:
            class Impl : public Task {
            public:
                Impl(std::function<void(void)> const& body) :
                _body(body) {}

                void execute() override {
                    _body();
                }

            private:
                std::function<void(void)> const _body;
            };

        public:
            Invocation(std::shared_ptr<RunLoop> const& runLoop) :
            _runLoop(runLoop) {}

            Invocation execute(std::function<void(void)> const& body) {
                _runLoop->add(std::make_shared<Impl>(body));
                return Invocation(_runLoop);
            }

        private:
            std::shared_ptr<RunLoop> const _runLoop;
        };
    }
}
