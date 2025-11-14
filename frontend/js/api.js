/**
 * api.js
 *
 * API client for communicating with the backend
 * Handles all HTTP requests with error handling
 */

const API_BASE_URL = 'http://localhost:3000/api';

class ApiClient {
    /**
     * Make GET request
     */
    async get(endpoint) {
        try {
            const response = await fetch(`${API_BASE_URL}${endpoint}`);
            const data = await response.json();

            if (!response.ok) {
                throw new Error(data.error || 'Request failed');
            }

            return data;
        } catch (error) {
            console.error(`GET ${endpoint} failed:`, error);
            throw error;
        }
    }

    /**
     * Make POST request
     */
    async post(endpoint, body) {
        try {
            const response = await fetch(`${API_BASE_URL}${endpoint}`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(body)
            });

            const data = await response.json();

            if (!response.ok) {
                throw new Error(data.error || 'Request failed');
            }

            return data;
        } catch (error) {
            console.error(`POST ${endpoint} failed:`, error);
            throw error;
        }
    }

    /**
     * Make PUT request
     */
    async put(endpoint, body) {
        try {
            const response = await fetch(`${API_BASE_URL}${endpoint}`, {
                method: 'PUT',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(body)
            });

            const data = await response.json();

            if (!response.ok) {
                throw new Error(data.error || 'Request failed');
            }

            return data;
        } catch (error) {
            console.error(`PUT ${endpoint} failed:`, error);
            throw error;
        }
    }

    /**
     * Get system health status
     */
    async getHealth() {
        return await this.get('/health');
    }

    /**
     * Get city graph data
     */
    async getGraph() {
        return await this.get('/graph');
    }

    /**
     * Get all drivers
     */
    async getDrivers() {
        return await this.get('/drivers');
    }

    /**
     * Get driver by ID
     */
    async getDriver(driverId) {
        return await this.get(`/drivers/${driverId}`);
    }

    /**
     * Request a ride
     */
    async requestRide(pickupLocation, destinationLocation, passengerId = 'GUEST') {
        return await this.post('/ride/request', {
            pickupLocation,
            destinationLocation,
            passengerId
        });
    }

    /**
     * Calculate shortest path
     */
    async calculatePath(source, destination) {
        return await this.post('/path/shortest', {
            source,
            destination
        });
    }

    /**
     * Update driver location
     */
    async updateDriverLocation(driverId, location) {
        return await this.put(`/drivers/${driverId}/location`, {
            location
        });
    }

    /**
     * Update driver availability
     */
    async updateDriverAvailability(driverId, available) {
        return await this.put(`/drivers/${driverId}/availability`, {
            available
        });
    }

    /**
     * Get demand analytics
     */
    async getDemandAnalytics() {
        return await this.get('/analytics/demand');
    }

    /**
     * Reset system
     */
    async resetSystem() {
        return await this.post('/system/reset', {});
    }
}

// Export singleton instance
const apiClient = new ApiClient();
