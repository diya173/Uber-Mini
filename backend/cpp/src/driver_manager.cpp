/**
 * driver_manager.cpp
 *
 * Implementation of Driver Management System
 */

#include "include/driver_manager.h"
#include <sstream>
#include <iomanip>

namespace RideSharing {

std::string Driver::toJSON() const {
    std::ostringstream oss;
    oss << "{\"id\":\"" << id << "\""
        << ",\"name\":\"" << name << "\""
        << ",\"currentLocation\":" << currentLocation
        << ",\"isAvailable\":" << (isAvailable ? "true" : "false")
        << ",\"vehicleType\":\"" << vehicleType << "\""
        << ",\"rating\":" << std::fixed << std::setprecision(1) << rating
        << ",\"completedRides\":" << completedRides << "}";
    return oss.str();
}

DriverManager::DriverManager() {}

void DriverManager::logOperation(const std::string& operation) {
    operationLogs.push_back(operation);
}

bool DriverManager::addDriver(const Driver& driver) {
    // Check if driver already exists
    if (drivers.find(driver.id) != drivers.end()) {
        std::ostringstream log;
        log << "Failed to add driver " << driver.id << ": already exists";
        logOperation(log.str());
        return false;
    }

    drivers[driver.id] = driver;

    std::ostringstream log;
    log << "Added driver " << driver.id << " (" << driver.name
        << ") at location " << driver.currentLocation;
    logOperation(log.str());

    return true;
}

bool DriverManager::removeDriver(const std::string& driverId) {
    auto it = drivers.find(driverId);
    if (it == drivers.end()) {
        std::ostringstream log;
        log << "Failed to remove driver " << driverId << ": not found";
        logOperation(log.str());
        return false;
    }

    drivers.erase(it);

    std::ostringstream log;
    log << "Removed driver " << driverId;
    logOperation(log.str());

    return true;
}

Driver* DriverManager::getDriver(const std::string& driverId) {
    auto it = drivers.find(driverId);
    if (it == drivers.end()) {
        return nullptr;
    }
    return &(it->second);
}

bool DriverManager::updateDriverLocation(const std::string& driverId, int newLocation) {
    auto it = drivers.find(driverId);
    if (it == drivers.end()) {
        std::ostringstream log;
        log << "Failed to update location for driver " << driverId << ": not found";
        logOperation(log.str());
        return false;
    }

    int oldLocation = it->second.currentLocation;
    it->second.currentLocation = newLocation;

    std::ostringstream log;
    log << "Updated driver " << driverId << " location from "
        << oldLocation << " to " << newLocation;
    logOperation(log.str());

    return true;
}

bool DriverManager::updateDriverAvailability(const std::string& driverId, bool available) {
    auto it = drivers.find(driverId);
    if (it == drivers.end()) {
        std::ostringstream log;
        log << "Failed to update availability for driver " << driverId << ": not found";
        logOperation(log.str());
        return false;
    }

    it->second.isAvailable = available;

    std::ostringstream log;
    log << "Updated driver " << driverId << " availability to "
        << (available ? "available" : "busy");
    logOperation(log.str());

    return true;
}

std::vector<Driver> DriverManager::getAvailableDrivers() const {
    std::vector<Driver> available;
    for (const auto& pair : drivers) {
        if (pair.second.isAvailable) {
            available.push_back(pair.second);
        }
    }
    return available;
}

std::vector<Driver> DriverManager::getAllDrivers() const {
    std::vector<Driver> allDrivers;
    for (const auto& pair : drivers) {
        allDrivers.push_back(pair.second);
    }
    return allDrivers;
}

int DriverManager::getAvailableDriverCount() const {
    int count = 0;
    for (const auto& pair : drivers) {
        if (pair.second.isAvailable) {
            count++;
        }
    }
    return count;
}

std::string DriverManager::toJSON() const {
    std::ostringstream oss;
    oss << "{\"totalDrivers\":" << drivers.size()
        << ",\"availableDrivers\":" << getAvailableDriverCount()
        << ",\"drivers\":[";

    bool first = true;
    for (const auto& pair : drivers) {
        if (!first) oss << ",";
        first = false;
        oss << pair.second.toJSON();
    }

    oss << "]}";
    return oss.str();
}

} // namespace RideSharing
