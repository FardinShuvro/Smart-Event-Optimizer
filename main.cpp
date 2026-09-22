#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

struct Service {
    int id;
    string name;
    string category;
    int price;
    double rating;
    int utility;
};

enum class SortCriterion {
    PRICE,
    RATING,
    UTILITY
};

enum class SortOrder {
    ASCENDING,
    DESCENDING
};

// -------------------- Display Helpers --------------------

void printLine(int width = 88) {
    cout << string(width, '-') << '\n';
}

void printTitle(const string& title, int width = 88) {
    cout << "\n";
    printLine(width);
    int padding = max(0, (width - static_cast<int>(title.size())) / 2);
    cout << string(padding, ' ') << title << '\n';
    printLine(width);
}

void printMoney(int amount) {
    cout << "Tk " << amount;
}

void printServiceHeader() {
    printLine();
    cout << left
         << setw(5) << "ID"
         << setw(28) << "Service"
         << setw(18) << "Category"
         << right
         << setw(10) << "Price"
         << setw(10) << "Rating"
         << setw(10) << "Utility"
         << '\n';
    printLine();
}

void printService(const Service& s) {
    cout << left
         << setw(5) << s.id
         << setw(28) << s.name
         << setw(18) << s.category
         << right
         << setw(10) << s.price
         << setw(10) << fixed << setprecision(1) << s.rating
         << setw(10) << s.utility
         << '\n';
}

void displayServices(const vector<Service>& services) {
    printTitle("AVAILABLE EVENT SERVICES");
    printServiceHeader();
    for (const auto& s : services) {
        printService(s);
    }
    printLine();
}

// -------------------- Comparison Logic --------------------

bool comesBefore(const Service& a, const Service& b,
                 SortCriterion criterion, SortOrder order) {
    double x, y;

    switch (criterion) {
        case SortCriterion::PRICE:
            x = a.price;
            y = b.price;
            break;
        case SortCriterion::RATING:
            x = a.rating;
            y = b.rating;
            break;
        case SortCriterion::UTILITY:
            x = a.utility;
            y = b.utility;
            break;
    }

    if (order == SortOrder::ASCENDING) {
        return x < y;
    }
    return x > y;
}

// -------------------- Bubble Sort --------------------

void bubbleSort(vector<Service>& a,
                SortCriterion criterion,
                SortOrder order) {
    int n = static_cast<int>(a.size());

    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;

        for (int j = 0; j < n - i - 1; ++j) {
            if (!comesBefore(a[j], a[j + 1], criterion, order)) {
                swap(a[j], a[j + 1]);
                swapped = true;
            }
        }

        if (!swapped) break;
    }
}

// -------------------- Selection Sort --------------------

void selectionSort(vector<Service>& a,
                   SortCriterion criterion,
                   SortOrder order) {
    int n = static_cast<int>(a.size());

    for (int i = 0; i < n - 1; ++i) {
        int selected = i;

        for (int j = i + 1; j < n; ++j) {
            if (comesBefore(a[j], a[selected], criterion, order)) {
                selected = j;
            }
        }

        if (selected != i) {
            swap(a[i], a[selected]);
        }
    }
}

// -------------------- Merge Sort --------------------

void mergeParts(vector<Service>& a, int left, int mid, int right,
                SortCriterion criterion, SortOrder order) {
    vector<Service> temp;
    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right) {
        if (comesBefore(a[i], a[j], criterion, order)) {
            temp.push_back(a[i++]);
        } else {
            temp.push_back(a[j++]);
        }
    }

    while (i <= mid) temp.push_back(a[i++]);
    while (j <= right) temp.push_back(a[j++]);

    for (int k = 0; k < static_cast<int>(temp.size()); ++k) {
        a[left + k] = temp[k];
    }
}

void mergeSortRecursive(vector<Service>& a, int left, int right,
                        SortCriterion criterion, SortOrder order) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    mergeSortRecursive(a, left, mid, criterion, order);
    mergeSortRecursive(a, mid + 1, right, criterion, order);
    mergeParts(a, left, mid, right, criterion, order);
}

void mergeSort(vector<Service>& a,
               SortCriterion criterion,
               SortOrder order) {
    if (!a.empty()) {
        mergeSortRecursive(a, 0, static_cast<int>(a.size()) - 1,
                           criterion, order);
    }
}

// -------------------- Search --------------------

void searchService(const vector<Service>& services) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string query;
    cout << "\nEnter service name or category to search: ";
    getline(cin, query);

    bool found = false;
    for (const auto& s : services) {
        if (s.name.find(query) != string::npos ||
            s.category.find(query) != string::npos) {
            if (!found) {
                printTitle("SEARCH RESULTS");
                printServiceHeader();
            }
            printService(s);
            found = true;
        }
    }

    if (found) {
        printLine();
    } else {
        cout << "No matching service found.\n";
    }
}

// -------------------- Knapsack --------------------
//
// The event has two required core categories:
//   1) Venue
//   2) Catering
//
// The user chooses one option from each category.
// Then 0/1 Knapsack selects optional resources from the remaining budget.
//
// price   -> cost/weight
// utility -> value
//

struct KnapsackResult {
    vector<int> selectedIndices;
    int totalCost = 0;
    int totalUtility = 0;
};

KnapsackResult knapsack(const vector<Service>& services, int budget) {
    int n = static_cast<int>(services.size());

    // dp[i][b] = maximum utility using first i optional services
    // with budget b.
    vector<vector<int>> dp(n + 1, vector<int>(budget + 1, 0));

    for (int i = 1; i <= n; ++i) {
        int cost = services[i - 1].price;
        int value = services[i - 1].utility;

        for (int b = 0; b <= budget; ++b) {
            dp[i][b] = dp[i - 1][b];

            if (cost <= b) {
                dp[i][b] = max(dp[i][b],
                               dp[i - 1][b - cost] + value);
            }
        }
    }

    KnapsackResult result;
    int b = budget;

    for (int i = n; i >= 1; --i) {
        if (dp[i][b] != dp[i - 1][b]) {
            int index = i - 1;
            result.selectedIndices.push_back(index);
            result.totalCost += services[index].price;
            result.totalUtility += services[index].utility;
            b -= services[index].price;
        }
    }

    reverse(result.selectedIndices.begin(), result.selectedIndices.end());
    return result;
}

int chooseCoreService(const vector<Service>& services,
                      const string& category,
                      const string& label) {
    vector<int> indices;

    cout << "\n" << label << "\n";
    printServiceHeader();

    for (int i = 0; i < static_cast<int>(services.size()); ++i) {
        if (services[i].category == category) {
            printService(services[i]);
            indices.push_back(i);
        }
    }
    printLine();

    int id;
    cout << "Enter the ID of your chosen " << category << ": ";
    cin >> id;

    for (int index : indices) {
        if (services[index].id == id) {
            return index;
        }
    }

    cout << "Invalid ID. No core service selected.\n";
    return -1;
}

void optimizeEvent(const vector<Service>& services,
                   int budget,
                   int venueIndex,
                   int cateringIndex) {
    if (budget <= 0) {
        cout << "\nPlease set a positive budget first.\n";
        return;
    }

    if (venueIndex == -1 || cateringIndex == -1) {
        cout << "\nPlease configure one venue and one catering option first.\n";
        return;
    }

    int coreCost = services[venueIndex].price +
                   services[cateringIndex].price;

    if (coreCost > budget) {
        cout << "\nThe selected venue and catering cost " << coreCost
             << ", which exceeds your budget of " << budget << ".\n";
        cout << "Choose cheaper core services or increase the budget.\n";
        return;
    }

    int remainingBudget = budget - coreCost;

    // Only non-core services are candidates for 0/1 Knapsack.
    // Each optional category has one service entry, so every candidate is
    // an independent event resource and can be selected at most once.
    vector<Service> optionalServices;
    vector<int> originalIndex;

    for (int i = 0; i < static_cast<int>(services.size()); ++i) {
        if (i != venueIndex &&
            i != cateringIndex &&
            services[i].category != "Venue" &&
            services[i].category != "Catering") {
            optionalServices.push_back(services[i]);
            originalIndex.push_back(i);
        }
    }

    KnapsackResult result = knapsack(optionalServices, remainingBudget);

    int totalCost = coreCost + result.totalCost;
    int totalUtility =
        services[venueIndex].utility +
        services[cateringIndex].utility +
        result.totalUtility;

    printTitle("OPTIMIZED EVENT PLAN");

    cout << "\nCORE SERVICES\n";
    printServiceHeader();
    printService(services[venueIndex]);
    printService(services[cateringIndex]);
    printLine();

    cout << "\nOPTIONAL SERVICES SELECTED BY 0/1 KNAPSACK\n";
    if (result.selectedIndices.empty()) {
        cout << "No optional service fits within the remaining budget.\n";
    } else {
        printServiceHeader();
        for (int index : result.selectedIndices) {
            printService(optionalServices[index]);
        }
        printLine();
    }

    cout << left << setw(25) << "Total Event Budget" << ": "; printMoney(budget); cout << '\n';
    cout << left << setw(25) << "Core Service Cost" << ": "; printMoney(coreCost); cout << '\n';
    cout << left << setw(25) << "Optional Service Cost" << ": "; printMoney(result.totalCost); cout << '\n';
    cout << left << setw(25) << "Total Event Cost" << ": "; printMoney(totalCost); cout << '\n';
    cout << left << setw(25) << "Remaining Budget" << ": "; printMoney(budget - totalCost); cout << '\n';
    cout << left << setw(25) << "Total Utility" << ": " << totalUtility << '\n';
    printLine();

    cout << "\nThe 0/1 Knapsack selected each optional resource at most once "
            "to maximize total utility within the remaining budget.\n";
}

// -------------------- Sorting Menu --------------------

SortCriterion chooseCriterion() {
    int choice;

    cout << "\nChoose sorting criterion:\n";
    cout << "1. Price\n";
    cout << "2. Rating\n";
    cout << "3. Utility\n";
    cout << "Enter choice: ";
    cin >> choice;

    switch (choice) {
        case 1: return SortCriterion::PRICE;
        case 2: return SortCriterion::RATING;
        case 3: return SortCriterion::UTILITY;
        default:
            cout << "Invalid choice. Using Price.\n";
            return SortCriterion::PRICE;
    }
}

SortOrder chooseOrder() {
    int choice;

    cout << "\nChoose order:\n";
    cout << "1. Ascending\n";
    cout << "2. Descending\n";
    cout << "Enter choice: ";
    cin >> choice;

    return (choice == 2) ? SortOrder::DESCENDING
                         : SortOrder::ASCENDING;
}

void sortMenu(const vector<Service>& services) {
    int algorithm;

    cout << "\nChoose sorting algorithm:\n";
    cout << "1. Bubble Sort\n";
    cout << "2. Selection Sort\n";
    cout << "3. Merge Sort\n";
    cout << "Enter choice: ";
    cin >> algorithm;

    SortCriterion criterion = chooseCriterion();
    SortOrder order = chooseOrder();

    vector<Service> sorted = services;

    auto start = chrono::high_resolution_clock::now();

    if (algorithm == 1) {
        bubbleSort(sorted, criterion, order);
    } else if (algorithm == 2) {
        selectionSort(sorted, criterion, order);
    } else if (algorithm == 3) {
        mergeSort(sorted, criterion, order);
    } else {
        cout << "Invalid algorithm choice.\n";
        return;
    }

    auto end = chrono::high_resolution_clock::now();
    auto elapsed =
        chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    printTitle("SORTED SERVICES");
    printServiceHeader();
    for (const auto& s : sorted) {
        printService(s);
    }
    printLine();

    cout << "Sorting time: " << elapsed << " ns\n";
}

// -------------------- Performance Comparison --------------------

vector<Service> generateTestData(int n) {
    vector<Service> data;
    data.reserve(n);

    for (int i = 0; i < n; ++i) {
        Service s;
        s.id = i + 1;
        s.name = "Service_" + to_string(i + 1);
        s.category = "Test";
        s.price = 1000 + ((i * 7919) % 50000);
        s.rating = 1.0 + ((i * 37) % 41) / 10.0;
        s.utility = 1 + ((i * 17) % 20);
        data.push_back(s);
    }

    return data;
}

long long measureBubble(const vector<Service>& data) {
    vector<Service> copy = data;
    auto start = chrono::high_resolution_clock::now();
    bubbleSort(copy, SortCriterion::PRICE, SortOrder::ASCENDING);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

long long measureSelection(const vector<Service>& data) {
    vector<Service> copy = data;
    auto start = chrono::high_resolution_clock::now();
    selectionSort(copy, SortCriterion::PRICE, SortOrder::ASCENDING);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

long long measureMerge(const vector<Service>& data) {
    vector<Service> copy = data;
    auto start = chrono::high_resolution_clock::now();
    mergeSort(copy, SortCriterion::PRICE, SortOrder::ASCENDING);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

void compareAlgorithms() {
    printTitle("SORTING ALGORITHM COMPARISON", 70);

    vector<int> sizes = {10, 50, 100, 500, 1000, 2000};

    cout << left
         << setw(12) << "Dataset"
         << setw(18) << "Bubble (us)"
         << setw(18) << "Selection (us)"
         << setw(18) << "Merge (us)" << '\n';
    printLine(66);

    for (int n : sizes) {
        vector<Service> data = generateTestData(n);

        long long bubble = measureBubble(data);
        long long selection = measureSelection(data);
        long long merge = measureMerge(data);

        cout << left
             << setw(12) << n
             << setw(18) << bubble
             << setw(18) << selection
             << setw(18) << merge << '\n';
    }

    printLine(66);
    cout << "Times are measured on the current computer and may vary.\n";
}

// -------------------- Main --------------------

int main() {
    ios::sync_with_stdio(false);
    cin.tie(&cout);

    vector<Service> services = {
        // Core services: user selects exactly one Venue and one Catering package.
        {1,  "Royal Convention Hall", "Venue",         30000, 4.7, 10},
        {2,  "City Community Center", "Venue",         18000, 4.2, 7},
        {3,  "Premium Catering",      "Catering",      25000, 4.8, 10},
        {4,  "Standard Catering",     "Catering",      15000, 4.3, 7},

        // Optional services: one independent resource from each category.
        // Each category appears only once, so the 0/1 Knapsack model
        // cannot select mutually exclusive alternatives such as
        // Basic + Pro Photography at the same event.
        {5,  "Professional Photography", "Photography", 20000, 4.9, 9},
        {6,  "Premium Decoration",        "Decoration",  18000, 4.7, 8},
        {7,  "Professional Sound",        "Sound",       12000, 4.6, 8},
        {8,  "Luxury Transport",           "Transport",   10000, 4.5, 7},
        {9,  "Live Music",                 "Entertainment",18000, 4.8, 9},
        {10, "LED Screen",                 "Visual",       15000, 4.5, 7},
        {11, "Event Security",             "Security",      7000, 4.3, 6},
        {12, "Premium Invitation",         "Invitation",    5000, 4.2, 4},
        {13, "Photo Booth",                "Photo Booth",   9000, 4.7, 6},
        {14, "Stage Lighting",             "Lighting",      9000, 4.4, 6},
        {15, "Backup Generator",           "Power Backup",  8000, 4.5, 7},
        {16, "Event Staff",                "Staffing",      6000, 4.2, 5},
        {17, "Floral Arrangement",         "Floral",        7000, 4.4, 5},
        {18, "Guest Transportation",       "Shuttle",      11000, 4.3, 7},
        {19, "Social Media Coverage",      "Media",         6000, 4.6, 6},
        {20, "Event Insurance",             "Insurance",     5000, 4.5, 6}
    };

    int budget = 80000;
    int venueIndex = -1;
    int cateringIndex = -1;
    int choice;

    cout << "\n";
    printLine();
    cout << "        SMART EVENT BUDGET & RESOURCE OPTIMIZER\n";
    cout << "             Algorithm Lab Project\n";
    cout << "\n";
    cout << "   Bubble Sort | Selection Sort | Merge Sort | 0/1 Knapsack\n";
    printLine();

    do {
        cout << "\n";
        printLine();
        cout << "EVENT STATUS\n";
        printLine();
        cout << "Budget           : "; printMoney(budget); cout << '\n';
        cout << "Venue            : " << (venueIndex != -1 ? services[venueIndex].name : "Not selected") << '\n';
        cout << "Catering         : " << (cateringIndex != -1 ? services[cateringIndex].name : "Not selected") << '\n';
        if (venueIndex != -1 && cateringIndex != -1) {
            int coreCost = services[venueIndex].price + services[cateringIndex].price;
            int remainingBudget = budget - coreCost;
            cout << "Available for optional services: ";
            if (remainingBudget >= 0) {
                printMoney(remainingBudget);
            } else {
                cout << "Budget exceeded by ";
                printMoney(-remainingBudget);
            }
            cout << '\n';
        }
        printLine();
        cout << "MAIN MENU\n";
        printLine();
        cout << "[1] Display All Services\n";
        cout << "[2] Sort Services\n";
        cout << "[3] Search Service\n";
        cout << "[4] Set Event Budget\n";
        cout << "[5] Configure Venue & Catering\n";
        cout << "[6] Optimize Event Plan (0/1 Knapsack)\n";
        cout << "[7] Compare Sorting Algorithms\n";
        cout << "[8] Exit\n";
        printLine();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayServices(services);
                break;

            case 2:
                sortMenu(services);
                break;

            case 3:
                searchService(services);
                break;

            case 4:
                cout << "Enter your event budget: ";
                cin >> budget;

                if (budget <= 0) {
                    cout << "Budget must be greater than zero.\n";
                    budget = 80000;
                } else {
                    cout << "Budget updated successfully.\n";
                }
                break;

            case 5: {
                int selectedVenue =
                    chooseCoreService(services, "Venue", "AVAILABLE VENUES");

                if (selectedVenue != -1) {
                    venueIndex = selectedVenue;
                }

                int selectedCatering =
                    chooseCoreService(services, "Catering",
                                      "AVAILABLE CATERING PACKAGES");

                if (selectedCatering != -1) {
                    cateringIndex = selectedCatering;
                }

                cout << "\nCore event configuration updated.\n";

                if (venueIndex != -1 && cateringIndex != -1) {
                    int coreCost = services[venueIndex].price +
                                   services[cateringIndex].price;
                    int remainingBudget = budget - coreCost;

                    printTitle("BUDGET STATUS AFTER CORE SELECTION");
                    cout << left << setw(25) << "Total Event Budget" << ": ";
                    printMoney(budget);
                    cout << '\n';
                    cout << left << setw(25) << "Venue Cost" << ": ";
                    printMoney(services[venueIndex].price);
                    cout << '\n';
                    cout << left << setw(25) << "Catering Cost" << ": ";
                    printMoney(services[cateringIndex].price);
                    cout << '\n';
                    printLine();

                    if (remainingBudget >= 0) {
                        cout << left << setw(25) << "Remaining for Optional Services" << ": ";
                        printMoney(remainingBudget);
                        cout << '\n';
                        cout << "This remaining amount will be used as the 0/1 Knapsack capacity.\n";
                    } else {
                        cout << left << setw(25) << "Budget Shortfall" << ": ";
                        printMoney(-remainingBudget);
                        cout << '\n';
                        cout << "The venue and catering exceed your current budget.\n";
                        cout << "Increase the budget or choose cheaper core services.\n";
                    }
                    printLine();
                }
                break;
            }

            case 6:
                optimizeEvent(services, budget, venueIndex, cateringIndex);
                break;

            case 7:
                compareAlgorithms();
                break;

            case 8:
                cout << "\nThank you for using Smart Event Optimizer!\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 8);

    return 0;
}