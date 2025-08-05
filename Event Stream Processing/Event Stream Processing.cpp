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
std::vector<SimulationEvent> SortByTime(const std::vector<SimulationEvent>& events)
{
    auto sorted{ events };

    //std::ranges::sort(sorted, {}, sorted.timestampSec);//why wouldn't this work? => sorted is a vector of SimulationEvent, we need to look into each SimulationEvent to access the memeber timestampSec
    std::ranges::sort(sorted, {}, &SimulationEvent::timestampSec);//and why are we using memeber pointer here? what are the underlying reason? what  are the rule of thumb for using memeber pointer as arguments? 
    // => projection argument( how to extract a key from each element in the range): underneath is a pointer to member, so for each element in the range(each event), the projection access the members like this: | event.*(&SimulationEvent::timestampSec) |
    // => is to say: "sort by this field"

    return sorted;
}

std::vector<SimulationEvent> SortByTimeAndReturnACopy(const std::vector<SimulationEvent>& events)
{
    std::vector<SimulationEvent> sortedByTime{ events };
    std::ranges::sort(sortedByTime, {}, &SimulationEvent::timestampSec);
    return sortedByTime;
}

void SortEventsByTime(std::vector<SimulationEvent>& events)// Key STL concept: sort with a projection.
{
    std::cout << "SortEventsByTime, in-place" << "\n";

    std::ranges::sort(events, {}, &SimulationEvent::timestampSec);//Sort events in ascending order using the value of timestampSec for comparison
	PrintEvents(events);//NOTE: ranges::sort() is in-place
}

void SortEventsByTime(const std::vector<SimulationEvent>& events, bool isAscending = true)//is this const a legal overload?
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


//2. Filter Events by Source & Type
std::vector<SimulationEvent> FliterBySource(const std::vector<SimulationEvent>& events, const std::string& source)
{
    std::vector<SimulationEvent> filtered{ events };

    std::ranges::copy_if(events, std::back_inserter(filtered),
        [&source](const SimulationEvent& event)
        {
            return source == event.source;
        });
    return filtered;
}

std::vector<SimulationEvent> FilterByType(const std::vector<SimulationEvent>& events, const EventType& type)
{
    std::vector<SimulationEvent> filteredEvents{ events };
    std::ranges::copy_if(events, std::back_inserter(filteredEvents),
        [type](const SimulationEvent& event)//always const xxx& x, if we don't want to change the input
        {
            return type == event.type;
        }
    );
}

std::vector<SimulationEvent> FilterByType(const std::vector<SimulationEvent>& events, EventType typeToFilter)
{
    std::cout << "FilterByType" << "\n";

    std::vector<SimulationEvent> filtered{};

    std::ranges::copy_if
    (
        events, 
        std::back_inserter(filtered), //(source, output, predicate), predicate is a lambda, a condition
        [typeToFilter](const SimulationEvent& ev)//have to capture the value of typeToFilter because lambda are saperate scope like a differernt function, not a normal codeblock; Copy elements from events into filtered
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
// => (source, output iteritor, lambda predicate)
//2. why is type's value needs to be captured? doesn't it live inside copy_if()? which is inside the bigger function scope which should have access to the argument of the bigger function which have type already? how can I conceptulize lambda's scope and access?
// => lambdas are separate function objects, not regular code blocks.

//3. Group Events by Type/Source
//Return a std::unordered_map<std::string, std::vector<SimulationEvent>> where each key is the source name.
std::unordered_map<EventType, std::vector<SimulationEvent>> GroupByType(const std::vector<SimulationEvent>& events, const EventType& type)
{
    std::unordered_map<EventType, std::vector<SimulationEvent>> grouped{};

    for (const auto& event : events)
    {
        grouped[event.type].push_back(event);
    }
    //learned from mistake: what is the value's structure? need to put more thoughts into that, I initally made the value as one event which of course is not right, it's a vector of events, make sense if I just take a minute to think about it, each group SHOULD have MULTIPLE events!
    return grouped;
}

std::unordered_map<std::string, std::vector<SimulationEvent>> GroupBySource(const std::vector<SimulationEvent>& events)
{
    std::unordered_map<std::string, std::vector<SimulationEvent>> grouped{};

    for (const SimulationEvent& event : events)
    {
        grouped[event.source].push_back(event);//would this make a new key pair if have not seen it before?
    }
    return grouped;
    //{
    //{"sources", [event0, event2]}, 
    //{"source1", [event1, event4]},
    //...
    //}
}

std::unordered_map<std::string, std::vector<SimulationEvent>> GroupBySource(const std::vector<SimulationEvent>& events)
{
    //pre. translate into a hashmap
    //1. loop through, 
    //2. add into Grouped by it's source
    std::unordered_map<std::string, std::vector<SimulationEvent>> GroupedBySource{};//{ key: "source", value: {event0, event1, event2 }
    for (const SimulationEvent& event : events)
    {
            GroupedBySource[event.source].push_back(event);//[] operator is overloaded, it'll add new key and new values if not exist yet => side effect: if look up with [] it will add the key, use .find() instead
    }
    return GroupedBySource;
}
// We want to group all events that came from the same source string.
// A map from string (source) to vector of events is the natural structure.

//4. Compute Total Value for a Given Source, Return the sum of.value for a specific source.
double AcumulatebySource(const std::vector<SimulationEvent>& events, const EventType& type)
{
    return std::accumulate(events.begin(), events.end(), 0.0,
        [&type](double sum, const SimulationEvent& event)//what is the rule comes to the const XXX& parameter in a lambda function? => same with normal function: 1. non-trival to copy. 2. don't wish to mutate original
        {
            return sum + (type == event.type ? event.value : 0.0);//why do we have to return for the second time? => this lambda return's a value to the std::acumulate() at each step(with a new sum each time for the algo)
        });
}

double AcumulateTotalBySource(const std::vector<SimulationEvent>& events, const std::string& source)
{
    return std::accumulate(
        events.begin(), events.end(), 0.0,
        [&source](double sum, const SimulationEvent& event)
        {
            return sum + (event.source == source ? event.value : 0.0);
        }
    );
}

double AccumalateValueBySource(const std::vector<SimulationEvent>& events, const std::string& source)
{
    return std::accumulate(events.begin(), events.end(), 0.0,//start with the return?? is this the only way to write?
        [&source](double sum, const SimulationEvent& event)//You MUST follow the signature that accumulate(): (accumulated_value, current_element)
            { 
                return sum + (event.source == source ? event.value : 0.0);//does the lamdbda HAVE to be in this format??
                //=> YES, in the case of this lambda's parameters 
                //the body of lamdbda tells the acumulate() HOW to combine them
                //MUST return the updated accumulator.
            }
        );//how can I conceptualize it? so hard to memerize the syntax, for the lambda, what is the thought process behind the syntax?
}

double ComputeTotalValueBySource(const std::vector<SimulationEvent>& events, const std::string& source)
{
    return std::accumulate(events.begin(), events.end(), 0.0,
        [&source](double sum, const SimulationEvent& event)
        {
            return sum + (event.source == source ? event.value : 0.0);
        }
    );//accuulte()'s 4th argument need to be a bool operation
}

//5. Find the First Event After a Time Threshold
//Return a pointer to the first event after a given timestamp(or nullptr if none found).
const SimulationEvent* FirstEventAfter(const std::vector<SimulationEvent>& events, const double& thresholdTime)//const SimulationEvent* return a pointer to the event, but I never seen this syntax before, is this regular?
//returning a pointer to a 'const SimulationEvent', the caller can read, but can not mutate, AKA returning a read only pointer; plus, can return a nullptr if no valid object exits
{
    auto it = std::ranges::find_if(events,//find more info on find_if() basics, argument types, lambda requirments, qurik etc
        //=> (range, unary predicate), returns a iterator, returns .end() if not found
                [&thresholdTime](const SimulationEvent& event)//HAVE TO BE: take one argument(element matches container's type), and return a bool
                {
                    return event.timestampSec > thresholdTime;//true for the matching case
                });

    return (it != events.end()) ? &(*it) : nullptr;//why compare to the last iterator? 
    // => MUST check valid before access it, .end() indicate invaild found
    //what is this??&(*it)
    // => *it: dereference it to get the actual event
    // &(): get the address of the event; AKA, a pointer
    // NOTE: I have the wrong Conception that a literator is a pointer
    // => A pointer is a kind of iterator, but not all iterators are pointers. => a iterator is a pointer with some logics built in; some cannot just be dereferenced
}

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

	//SortEventsByTime(events, false);
	//SortEventsByTime(events);
	//FilterByType(events, EventType::SENSOR_READING);    
    //SortByTime(events);
    std::cout << "Hello Simulated World!\n";
}

//Tips for Success:
//Use std::ranges::algorithms where applicable.
//Use structured bindings, lambda captures, and auto effectively.
//Avoid manual loops; prefer std::ranges::views::filter, transform, group_by, etc.
//Brush up on std::unordered_map, std::optional, and iterator utilities.

//** EXTRA NOTES FOR FUTURE LEARNING **
//### 1. * *Vector Algebra * *
//
//***Math Concept * *: Vectors(as in math / physics) are quantities with direction and magnitude.
//* **C++ Application * *: Used in physics simulations, 3D graphics, UAV kinematics, etc.You might define `struct Vector3` and overload operators for addition, dot / cross product, normalization, etc.
//
//-- -
//
//### 2. * *Matrix Operations * *
//
//***Math Concept * *: Matrices are used to transform, rotate, and scale coordinate spaces.
//*** C++ Application * *: Common in 3D graphics, robotics, and Kalman filtering.Often implemented using `std: : array` or specialized libraries like Eigen or GLM.
//
//-- -
//
//### 3. * *Linear Interpolation(LERP) * *
//
//***Math Concept * *: Computes an intermediate value between two known values.
//* **C++ Application * *: Useful for animations, trajectory estimation, or blending sensor data.
//* ```cpp
//double LERP(double a, double b, double t) { return a + (b - a) * t; }
//```
//
//-- -
//
//### 4. * *Probability Distribution * *
//
//***Math Concept * *: Describes how values are distributed—uniform, Gaussian, etc.
//*** C++ Application * *: Used in Monte Carlo simulations, noise models, and sensor uncertainty.
//* Standard support : `<random > `
//
//-- -
//
//### 5. * *Predicate Logic * *
//
//***Math Concept * *: Boolean - valued functions(i.e., conditions).
//* **C++ Application * *: Used in STL algorithms(`std::ranges::any_of`, `std::find_if`, etc.).
//* You already know this one, but it shows up* everywhere* .
//
//-- -
//
//### 6. * *Projection(again, but deeper) * *
//
//***Math Concept * *: A transformation that reduces dimensions or changes perspective.
//* **C++ Application * *: In `std: : ranges::sort`, projections help extract a sort key.
//* Rule of thumb : *"What field/property should this operation look at?" *
//
//-- -
//
//### 7. * *Distance Metrics * *
//
//***Math Concept * *: Euclidean, Manhattan, etc.
//*** C++ Application * *: Used for clustering, nearest neighbor algorithms, pathfinding(A\*).
//* Often implemented as utility functions.
//
//-- -
//
//### 8. * *State - Space Representation * *
//
//***Math Concept * *: System modeled as a set of input / output / state equations.
//* **C++ Application * *: Control systems and simulation engines often encode this using structs / classes and matrix math.
//
//-- -
//
//### 9. * *Finite Differences / Derivatives * *
//
//***Math Concept * *: Approximates derivatives using discrete steps.
//*** C++ Application * *: For simulating physics, velocities, and acceleration from position data.
//
//-- -
//
//### 10. * *Covariance / Correlation * *
//
//***Math Concept * *: Measures how variables change together.
//* **C++ Application * *: Kalman filters, sensor fusion, error analysis.


//** MORE TASKS **

//## * *2. Flight Data Aggregation * *
//
//```cpp
//struct FlightDataPoint {
//    double timestamp;     // seconds
//    double altitude;      // meters
//    double velocity;      // m/s
//    double engineThrust;  // Newtons
//};
//
//// TASKS:
//// 1. Compute max, min, and mean altitude
//// 2. Segment data into 10-second intervals and compute average velocity per segment
//// 3. Detect all timestamps where velocity drops below 30 m/s (stall warning)
//```
//
//-- -
//
//## * *3. Signal Synchronization * *
//
//```cpp
//struct SensorSample {
//    double timestamp;  // seconds
//    double value;      // sensor reading
//};
//
//// Two input streams, sorted by timestamp
//std::vector<SensorSample> gpsSamples;
//std::vector<SensorSample> imuSamples;
//
//// TASKS:
//// 1. Merge the two streams chronologically
//// 2. Interpolate missing timestamps in gpsSamples to match imuSamples
//// 3. Identify timestamps where the delta between two sensor values exceeds 1.0
//```
//
//-- -
//
//## * *4. Simulation Model Output Comparison * *
//
//```cpp
//struct SimulationState {
//    double timestamp;
//    double positionX;
//    double velocityY;
//    double temperature;
//};
//
//// Two simulations producing output over time
//std::vector<SimulationState> simA;
//std::vector<SimulationState> simB;
//
//// TASKS:
//// 1. Find all timestamps where |A - B| > threshold for any field
//// 2. Compute RMSE for each field across all matching timestamps
//// 3. Return a report of timestamps and fields with large deviations
//```
//
//-- -
//
//## * *5. Discrete Time - Step Simulator * *
//
//```cpp
//struct PhysicsState {
//    double timestamp;
//    double velocity;
//    double acceleration;
//};
//
//// A constant time step and initial state vector
//double timeStep = 0.1; // seconds
//std::vector<PhysicsState> states;
//
//// TASKS:
//// 1. Apply: newVelocity = velocity + acceleration * timeStep
//// 2. Append updated states to newStates vector
//// 3. Track total kinetic energy and return it