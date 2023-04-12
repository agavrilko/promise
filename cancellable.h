#pragma once

namespace Promise {
    class Cancellable {
    public:
        virtual void cancel() = 0;
    };
}
