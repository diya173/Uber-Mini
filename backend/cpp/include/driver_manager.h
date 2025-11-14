/**
 * driver_manager.h
 *
 * Driver Management System using HashMap
 * Efficiently stores and retrieves driver information
 *
 * Time Complexity:
 *   - Add Driver: O(1) average
 *   - Get Driver: O(1) average
 *   - Find Nearest: O(D) where D is number of drivers
 * Space Complexity: O(D)
 */

#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H

#include <unordered_map>
#include <string>
#include <vector>

namespace RideSharing {

// Driver structure
struct Driver {
    std::string id;
    std::string name;
    int currentLocation;     // Node ID where driver is currently located
    bool isAvailable;
    std::string vehicleType;
    double rating;
    int completedRides;

    Driver(const std::string& driverId = "", const std::string& driverName = "",
           int location = 0, const std::string& vehicle = "Sedan", double rate = 5.0)
        : id(driverId), name(driverName), currentLocation(location),
          isAvailable(true), vehicleType(vehicle), rating(rate), completedRides(0) {}

    std::string toJSON() const;
};

// Structure for nearest driver result
struct NearestDriverResult {
    Driver driver;
    double distance;
    std::vector<int> pathToPassenger;
    bool found;

    NearestDriverResult() : distance(0.0), found(false) {}
};

class DriverManager {
private:
    std::unordered_map<std::string, Driver> drivers; // HashMap: driver_id -> Driver
    std::vector<std::string> operationLogs;

    void logOperation(const std::string& operation);

public:
    DriverManager();

    // Add a new driver to the system
    bool addDriver(const Driver& driver);

    // Remove a driver from the system
    bool removeDriver(const std::string& driverId);

    // Get driver by ID
    Driver* getDriver(const std::string& driverId);

    // Update driver location
    bool updateDriverLocation(const std::string& driverId, int newLocation);

    // Update driver availability
    bool updateDriverAvailability(const std::string& driverId, bool available);

    // Get all available drivers
    std::vector<Driver> getAvailableDrivers() const;

    // Get all drivers
    std::vector<Driver> getAllDrivers() const;

    // Get number of drivers
    int getDriverCount() const { return drivers.size(); }

    // Get number of available drivers
    int getAvailableDriverCount() const;

    // Get operation logs
    std::vector<std::string> getLogs() const { return operationLogs; }

    // Clear logs
    void clearLogs() { operationLogs.clear(); }

    // Export to JSON
    std::string toJSON() const;
};

} // namespace RideSharing

#endif // DRIVER_MANAGER_H
