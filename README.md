# Smart Event Budget & Resource Optimization System

Project Language — C++

## Project Overview

This console-based project helps users plan an event within a fixed budget.

The user first chooses:
- One venue
- One catering package

The remaining budget is then used to select optional event resources such as photography, decoration, sound, transport, entertainment, lighting, security, invitations, and event support services.

The project demonstrates:

- Bubble Sort
- Selection Sort
- Merge Sort
- 0/1 Knapsack
- Searching
- Sorting performance comparison

## Main Features

1. Display all event services
2. Sort services by price, rating, or utility
3. Search by service name/category
4. Set an event budget
5. Select one venue and one catering package
6. Optimize optional resources using 0/1 Knapsack
7. Compare Bubble, Selection, and Merge Sort execution times

## Algorithm Mapping

| Algorithm | Project Role |
|---|---|
| Bubble Sort | Sort services using adjacent comparisons |
| Selection Sort | Repeatedly select the next minimum/maximum item |
| Merge Sort | Efficiently sort a larger service dataset |
| 0/1 Knapsack | Select the maximum-utility combination of optional resources within the remaining budget |

## Why 0/1 Knapsack?

Venue and catering are treated as required core choices. After selecting them, the remaining budget becomes the capacity for optional resources. Each optional resource category has one available service entry, so the Knapsack cannot accidentally select mutually exclusive alternatives such as Basic and Professional Photography for the same event. Every optional resource can be selected at most once.

- Cost = Knapsack weight
- Utility = Knapsack value
- Remaining event budget = Knapsack capacity

## Compile

With g++:

```bash
g++ -std=c++17 -O2 main.cpp -o smart_event_optimizer
```

Run:

```bash
./smart_event_optimizer
```

On Windows:

```bash
smart_event_optimizer.exe
```

## Suggested Demonstration

1. Display all services.
2. Configure one venue and one catering package.
3. Review the displayed remaining budget; this amount becomes the 0/1 Knapsack capacity.
4. Sort by price using Bubble Sort.
5. Sort by rating using Selection Sort.
6. Sort by utility using Merge Sort.
7. Set a budget such as 80000.
8. Run the 0/1 Knapsack optimizer.
9. Run the sorting performance comparison.
