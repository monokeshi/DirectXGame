#pragma once

class Utility
{
private:

public:
    Utility();
    ~Utility();

    template <typename T>
    static inline void Clamp(T &value, T min, T max)
    {
        if ( value < min )
        {
            value = min;
            return;
        }

        if ( value > max )
        {
            value = max;
            return;
        }
    }
};
