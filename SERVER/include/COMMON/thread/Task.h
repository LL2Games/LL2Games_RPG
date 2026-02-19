#pragma once

class Task {
public:
    virtual ~Task() = default;
    virtual void Execute() = 0;
};