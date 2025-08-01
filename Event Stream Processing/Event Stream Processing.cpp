//Event Stream Processing
//Simulate a log of timestamped events(sensor readings, actuator commands).You need to :
//  Parse / store SimulationEvent structs.
//  Sort them by time.
//  Filter by event type or component.
//  Accumulate values like total force, average RPM, etc.

//Problem Prompt:
//You are given a sequence of discrete simulation events representing actions or readings in a flight simulation environment.Each event is defined with EventType.
//Write the functions using C++20 and STL algorithms :

#include <iostream>
#include <algorithm>   // for std::ranges::sort
#include <ranges>      // for ranges in C++20
#include <vector>
#include <string>
#include <unordered_map> 
#include <numeric>   // for std::accumulate
#include <functional>

enum class EventType 
{
    SENSOR_READING,
    CONTROL_INPUT,
    ACTUATOR_COMMAND//2
};

struct SimulationEvent 
{
    double timestampSec{};        // Time in seconds since simulation start
    EventType type{};             // What kind of event this is
    std::string source{};         // e.g., "engine1", "rudder", etc.
    double value{};               // A context-dependent value (e.g., throttle %, altitude)

    void ConstructMockingSimulationEventVector(std::vector<SimulationEvent>& events)// Mocking a vector of SimulationEvent for testing purposes
    {
        events.push_back({ 5.0, EventType::ACTUATOR_COMMAND, "flaps", 15.0 });
        events.push_back({1.5, EventType::CONTROL_INPUT, "pilot", 75.0});
        events.push_back({ 10.5, EventType::CONTROL_INPUT, "pilot", 85.0 });
        events.push_back({ 0.0, EventType::SENSOR_READING, "engine1", 100.0 });
        events.push_back({3.2, EventType::SENSOR_READING, "altimeter", 5000.0});
        events.push_back({4.5, EventType::CONTROL_INPUT, "pilot", 80.0});
        events.push_back({ 9.2, EventType::SENSOR_READING, "altimeter", 6000.0 });
        events.push_back({ 2.0, EventType::ACTUATOR_COMMAND, "rudder", 30.0 });
        events.push_back({ 6.0, EventType::SENSOR_READING, "engine2", 110.0 });
        events.push_back({ 7.5, EventType::CONTROL_INPUT, "pilot", 70.0 });
        events.push_back({ 8.0, EventType::ACTUATOR_COMMAND, "aileron", 20.0 });
	}
};

void PrintEvents(const std::vector<SimulationEvent>& events) 
{
    for (const auto& event : events) 
    {
        std::cout << "Timestamp: " << event.timestampSec
            << ", Type: " << static_cast<int>(event.type)
            << ", Source: " << event.source
            << ", Value: " << event.value << '\n';
    }
    std::cout << std::endl << "----------------------------------------" << std::endl;
}

// ** TASKS **
//1. Sort Events by Timestamp
void static SortEventsByTime(std::vector<SimulationEvent>& events)// Key STL concept: sort with a projection.
{
    std::cout << "SortEventsByTime, in-place" << "\n";

    std::ranges::sort(events, {}, &SimulationEvent::timestampSec);//Sort events in ascending order using the value of timestampSec for comparison
	PrintEvents(events);//NOTE: ranges::sort() is in-place
}

void static SortEventsByTime(const std::vector<SimulationEvent>& events, bool isAscending = true)//is this const a legal overload?
{
    std::cout << "SortEventsByTime, value" << "\n";

    std::vector<SimulationEvent> sortedByTimeEvents{ events };
    if (isAscending)
    {
        std::ranges::sort(sortedByTimeEvents, {}, &SimulationEvent::timestampSec);
    }
    else
    {
        std::ranges::sort(sortedByTimeEvents, std::greater<double>(), &SimulationEvent::timestampSec);
    }
    PrintEvents(sortedByTimeEvents);//NOTE: ranges::sort() is in-place
}

// Explanation:
// - std::ranges::sort is part of the new C++20 ranges library.
// - It allows a "projection" function (the 3rd argument here), which is like saying:
//     "please compare elements by their .timestamp field".
// - Equivalent to writing a lambda: [](const auto& a, const auto& b) { return a.timestamp < b.timestamp; }
// - But using `&SimulationEvent::timestamp` is more concise, idiomatic, and efficient.

// ** QUESTIONS **
//1. what does discrete means in "discrete simulation events"?
//  => evolves in steps, not continuously, only log when something actually happens
//2. std::ranges::sort(events, {}, &SimulationEvent::timestampSec); => what is the {} for?,  and what does 3rd argument called "projection" mean? shouldn't it mean comparison?
//  => {} means empty which means default comparator, which is "<" aka "less than" which means sort in ascending order
//3. is ranges::sort() in place? as in does it change the origianal data? what if the original data should not be changed?
//  => yes, in-place, Make a copy first if don't want change


//2. Filter Events by Type
std::vector<SimulationEvent> FilterByType(const std::vector<SimulationEvent>& events, EventType typeToFilter)
{
    std::cout << "FilterByType" << "\n";

    std::vector<SimulationEvent> filtered{};

    std::ranges::copy_if
    (
        events, 
        std::back_inserter(filtered), //(source, output, predicate), predicate is a lambda, a condition
        [typeToFilter](const SimulationEvent& ev)//Copy elements from events into filtered
        {
            return ev.type == typeToFilter; // but only if their type == typeToFilter.
        }
    );//last arg: lambda predicate; captures typeToFilter by value

    PrintEvents(filtered);
    return filtered;
}

// Key idea: Use ranges::views::filter to create a lazy-filtered range
// Then convert it into a new vector using ranges::to (in <ranges> in C++23,
// or just use std::copy for C++20 fallback)

// ** QUESTIONS **
//1. std::ranges::copy_if()'s argument are crazy, can you break everything down? so much going on

//3. Group Events by Source
//Return a std::unordered_map<std::string, std::vector<SimulationEvent>> where each key is the source name.
std::unordered_map<std::string, std::vector<SimulationEvent>> GroupBySource(
    const std::vector<SimulationEvent>& events)
{
    // We want to group all events that came from the same source string.
    // A map from string (source) to vector of events is the natural structure.

    std::unordered_map<std::string, std::vector<SimulationEvent>> grouped;

    for (const auto& ev : events) {
        grouped[ev.source].push_back(ev);
    }

    // No fancy STL here: sometimes a plain loop is the cleanest and fastest.
    return grouped;
}

//4. Compute Total Value for a Given Source
//Return the sum of.value for a specific source.
double SumValuesForSource(
    const std::vector<SimulationEvent>& events,
    const std::string& sourceToMatch)
{
    // Use std::accumulate to sum .value fields for events with matching source
    return std::accumulate(events.begin(), events.end(), 0.0,
        [&sourceToMatch](double sum, const SimulationEvent& ev) {
            return sum + (ev.source == sourceToMatch ? ev.value : 0.0);
        });
}

//5. Find the First Event After a Time Threshold
//Return a pointer to the first event after a given timestamp(or nullptr if none found).
const SimulationEvent* FirstEventAfter(
    const std::vector<SimulationEvent>& events,
    double thresholdTime)
{
    // Use std::ranges::find_if to find the first event where timestamp > threshold
    auto it = std::ranges::find_if(events,
        [thresholdTime](const SimulationEvent& ev) {
            return ev.timestampSec > thresholdTime;
        });

    // Return a pointer if found, or nullptr if not
    return (it != events.end()) ? &(*it) : nullptr;
}

int main()
{
	SimulationEvent event;
    std::vector<SimulationEvent> events{};
    event.ConstructMockingSimulationEventVector(events);
	PrintEvents(events);

	SortEventsByTime(events, false);
	SortEventsByTime(events);
	FilterByType(events, EventType::SENSOR_READING);    
    std::cout << "Hello Simulated World!\n";
}

//Tips for Success:
//Use std::ranges::algorithms where applicable.
//Use structured bindings, lambda captures, and auto effectively.
//Avoid manual loops; prefer std::ranges::views::filter, transform, group_by, etc.
//Brush up on std::unordered_map, std::optional, and iterator utilities.