#pragma once

namespace Promise {
    namespace Dispatch {
        class Task {
        public:
            virtual void execute() = 0;
        };
    }
}
