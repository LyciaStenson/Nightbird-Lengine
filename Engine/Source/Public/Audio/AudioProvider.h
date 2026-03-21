#pragma once

namespace Nightbird::Audio
{
    class Provider
    {
    public:
        virtual ~Provider() = default;
        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;
        virtual void Update() = 0;
    };
}
