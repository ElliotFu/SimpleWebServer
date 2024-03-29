#pragma once

#include "channel.h"
#include "timer.h"
#include "base/noncopyable.h"
#include "base/timestamp.h"
#include <memory>
#include <set>
#include <vector>

class Event_Loop;
class Timer_ID;

class Timer_Queue : Noncopyable
{
public:
    Timer_Queue(Event_Loop* loop);
    ~Timer_Queue();

    Timer_ID add_timer(const Timer::Timer_Callback& cb, Timestamp when, double interval);
    void cancel(Timer_ID timer_id);

    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> Timer_List;

private:
    typedef std::pair<Timer*, int64_t> Active_Timer;
    typedef std::set<Active_Timer> Active_Timer_Set;

    void add_timer_in_loop(Timer* timer);
    void cancel_in_loop(Timer_ID timer_id);
    // called when timerfd alarms
    void handle_read();
    // move out all expired timers
    std::vector<Entry> get_expired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    bool insert(Timer* timer);

    Event_Loop* loop_;
    const int timerfd_;
    Channel timerfd_channel_;
    Timer_List timers_;

    //for cancel
    std::atomic_bool calling_expired_timers_;
    Active_Timer_Set active_timers_;
    Active_Timer_Set canceling_timers_;
};