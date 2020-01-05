#pragma once

#include <chrono>

namespace Djn
{
    typedef std::chrono::time_point<std::chrono::steady_clock> Clock;
    class Time
    {
    public:
        Time();
        void tick();
        float deltaTime() const
        {
            return  std::chrono::duration_cast<std::chrono::nanoseconds>(currTime - lastTime).count() * NanoToSec;
        }
        float totalTime()
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(currTime - startTime).count() * NanoToSec;
        }
        const float SecToMilli = 1000.0f;
        const float SecToMicro = SecToMilli * 1000.0f;
        const float SecToNano = SecToMicro * 1000.0f;
        const float NanoToSec = 1.0f / SecToNano;
    private:
        Clock startTime;
        Clock lastTime;
        Clock currTime;
    };
}
