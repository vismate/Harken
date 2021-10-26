//
// Created by vismat on 26/10/2021.
//

#include <Harken.hpp>

bool Harken::Listener::operator()()
{
    return true;
}

bool Harken::Timer::HasFired()
{
    return has_fired;
}

void Harken::Timer::Reset()
{
    has_fired = false;
    start = millis();
}

Harken::Timer::Timer(unsigned long time) : current(0), time(time)
{
    Reset();
}

bool Harken::Timer::operator()()
{
    if (has_fired)
    { return false; }

    current = millis();
    if (current >= start)
    {
        if (current - start > time)
        {
            has_fired = true;
            return true;
        }
    }
    else if (((ULONG_MAX - start) + current) > time)
    {
        has_fired = true;
        return true;
    }

    return false;
}

Harken::Clock::Clock(unsigned long time) : timer(time)
{
}


bool Harken::Clock::operator()()
{
    if (timer())
    {
        Reset();
        return true;
    }

    return false;
}

void Harken::Clock::Reset()
{
    timer.Reset();
}

Harken::DigitalPinChange::DigitalPinChange(unsigned char pin, bool from, unsigned long debounce)
        : pin(pin), debounce(debounce), from(from), last_noticed(0)
{
    pinMode(pin, INPUT);
    prev = digitalRead(pin);
}

bool Harken::DigitalPinChange::operator()()
{
    bool current = digitalRead(pin);

    unsigned long current_time = millis();
    if (current_time > last_noticed + debounce)
    {
        last_noticed = current_time;
        bool return_val = (prev == from && current == !from);
        prev = current;
        return return_val;
    }

    return false;

}

Harken::DigitalPinRepeat::DigitalPinRepeat(unsigned char pin, unsigned long delay, bool from, unsigned long debounce)
        : count(0), prev_count(0), clock(delay), dp_change(pin, from, debounce)
{

}

bool Harken::DigitalPinRepeat::operator()()
{
    if (clock())
    {
        prev_count = count;
        count = 0;
        return prev_count != 0;
    }

    if (dp_change())
    {
        clock.Reset();
        count++;
    }

    return false;
}

unsigned int Harken::DigitalPinRepeat::GetCount() const
{
    return prev_count;
}

Harken::Counter::Counter(unsigned int count) : count(count), thus_far(0)
{
}

//TODO Should I check for overflow here? If the function gets called repetitively, thus_far can reach count again.
bool Harken::Counter::operator()()
{
    thus_far++;
    return thus_far == count;
}

void Harken::Counter::Reset()
{
    thus_far = 0;
}

Harken::Cycle::Cycle(unsigned int count) : counter(count)
{

}

bool Harken::Cycle::operator()()
{
    if (counter())
    {
        Reset();
        return true;
    }
    return false;
}

void Harken::Cycle::Reset()
{
    counter.Reset();
}

Harken::AnalogPinChange::AnalogPinChange(unsigned char pin, int change, bool trigger_on_extremes)
        : pin(pin), change(change), trigger_on_extremes(trigger_on_extremes)
{
    pinMode(pin, INPUT);
    prev = -1;
}

int Harken::AnalogPinChange::GetValue() const
{
    return prev;
}

bool Harken::AnalogPinChange::operator()()
{
    int current = analogRead(pin);

    if ((trigger_on_extremes && (current == 0 || current == 2023))
        || abs(current - prev) > change)
    {
        prev = current;
        return true;
    }

    return false;
}


Harken::DigitalPinHold::DigitalPinHold(unsigned char pin, unsigned long desired, bool active)
        : pin(pin), desired(desired), active(active), prev(active), start_time(0), has_fired(false)
{
    pinMode(pin, INPUT);
}

bool Harken::DigitalPinHold::operator()()
{
    bool val = digitalRead(pin);

    bool return_val = false;

    if (val == active && prev != active)
    {
        start_time = millis();
    }
    else if(val == active)
    {
        if (millis() - start_time > desired)
        {
            return_val = !has_fired;
            has_fired = true;
        }
    }
    else
    {
        has_fired = false;
    }

    prev = val;
    return return_val;
}
