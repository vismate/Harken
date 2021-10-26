//
// Created by vismat on 26/10/2021.
//

#ifndef ARDUINOCLOCK_HARKEN_HPP
#define ARDUINOCLOCK_HARKEN_HPP

#define ULONG_MAX 4294967295

#include <Arduino.h>

namespace Harken
{

    class Listener
    {
    public:
        Listener() = default;

        virtual bool operator()();
    };

    using AlwaysFire = Listener;

    class Timer : public Listener
    {
    private:
        unsigned long start;
        unsigned long current;
        unsigned long time;
        bool has_fired;
    public:
        explicit Timer(unsigned long time);

        bool operator()() override;

        bool HasFired();

        void Reset();

    };

    class Clock : public Listener
    {
    private:
        Timer timer;
    public:
        explicit Clock(unsigned long time);

        bool operator()() override;

        void Reset();

    };

    class DigitalPinChange : Listener
    {
    private:
        unsigned char pin;
        unsigned long debounce;
        unsigned long last_noticed;
        bool from;
        bool prev;
    public:
        explicit DigitalPinChange(unsigned char pin, bool from = LOW, unsigned long debounce = 40);

        bool operator()() override;
    };

    class DigitalPinRepeat : Listener
    {
    private:
        unsigned int count;
        unsigned int prev_count;
        Clock clock;
        DigitalPinChange dp_change;
    public:
        explicit DigitalPinRepeat(unsigned char pin, unsigned long delay = 250, bool from = LOW,
                                  unsigned long debounce = 40);

        bool operator()() override;

        unsigned int GetCount() const;
    };

    class Counter : Listener
    {
    private:
        unsigned int count;
        unsigned int thus_far;
    public:
        explicit Counter(unsigned int count);

        bool operator()() override;

        void Reset();
    };

    class Cycle : Listener
    {
    private:
        Counter counter;
    public:
        explicit Cycle(unsigned int count);

        bool operator()() override;

        void Reset();
    };

    class AnalogPinChange : Listener
    {
    private:
        unsigned char pin;
        int change;
        int prev;
        bool trigger_on_extremes;
    public:

        explicit AnalogPinChange(unsigned char pin, int change = 10, bool trigger_on_extremes = true);

        bool operator()() override;

        int GetValue() const;
    };

    class DigitalPinHold : Listener
    {
    private:
        unsigned char pin;
        bool active;
        bool prev;
        bool has_fired;
        unsigned long start_time;
        unsigned long desired;

    public:
        explicit DigitalPinHold(unsigned char pin, unsigned long desired = 1000, bool active = HIGH);

        bool operator()() override;
    };
}

#endif //ARDUINOCLOCK_HARKEN_HPP
