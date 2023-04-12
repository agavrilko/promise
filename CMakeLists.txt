cmake_minimum_required(VERSION 3.6)

# Project name
project(promise)

# Define the project and its properties
project(
    promise
    VERSION 0.1
    DESCRIPTION "Light-weight data streaming library"
    LANGUAGES CXX
)

set(PROMISE_HEADER
    cancellable.h
    concrete/bag.h
    concrete/future.h
    concrete/map.h
    concrete/subject.h
    concrete/subscription_observer.h
    dispatch/invocation.h
    dispatch/run_loop.h
    dispatch/task.h
    internal/bag.h
    internal/switch.h
    pipeline/future.h
    pipeline/just.h
    pipeline/link.h
    pipeline/listen.h
    pipeline/map.h
    pipeline/subscription_observer.h
    promise.h
    publisher.h
    queue.h
)

# Define the library target
add_library(promise INTERFACE ${PROMISE_HEADER})

# Add the library header files to the target
target_include_directories(promise INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
