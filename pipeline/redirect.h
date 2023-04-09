#pragma once

#include "../concrete/subject.h"
#include "../internal/closure.h"
#include "../stream.h"

namespace Promise {
    namespace Pipeline {
        struct Redirect {
        private:
            class Redirection : public Stream::Subscriber {
            public:
                Redirection(std::shared_ptr<Concrete::Subject> const& subject) :
                    _subject(subject) {}

                void completed() override {
                    _subject->complete();
                }

                void failed(std::shared_ptr<Future::Error> const& error) override {
                    _subject->fail(error);
                }

                void received(std::shared_ptr<Stream::Event> const& event) override {
                    _subject->receive(event);
                }

            private:
                std::shared_ptr<Concrete::Subject> const _subject;

            };

        public:
            static Redirect from(std::shared_ptr<Stream> const& stream) {
                return Redirect(stream);
            }

            std::shared_ptr<Cancellable> const to(std::shared_ptr<Concrete::Subject> const& subject) {
                return _stream->listen(std::make_shared<Redirection>(subject));
            }

        private:
            std::shared_ptr<Stream> const _stream;

            Redirect(std::shared_ptr<Stream> const& stream) :
                _stream(stream) {}
        };
    }
}
