/**
 * app.js
 *
 * Main application controller
 * Coordinates map renderer, UI controller, and API client
 */

class App {
    constructor() {
        this.mapRenderer = new MapRenderer('city-map');
        this.uiController = new UIController();
        this.graphData = null;
        this.driversData = null;

        this.init();
    }

    /**
     * Initialize application
     */
    async init() {
        console.log('Initializing Uber Mini application...');

        try {
            // Show loading
            this.uiController.showLoading();

            // Load initial data
            await this.loadGraphData();
            await this.loadDrivers();

            // Setup event listeners
            this.setupEventListeners();

            // Hide loading
            this.uiController.hideLoading();

            this.uiController.showSuccess('System initialized successfully!');

            console.log('Application initialized successfully');
        } catch (error) {
            console.error('Failed to initialize application:', error);
            this.uiController.hideLoading();
            this.uiController.showError('Failed to initialize system: ' + error.message);
        }
    }

    /**
     * Load graph data
     */
    async loadGraphData() {
        try {
            const response = await apiClient.getGraph();

            if (response.success) {
                this.graphData = response.data;
                this.mapRenderer.loadGraph(this.graphData);
                this.uiController.populateLocations(this.graphData.nodes);

                // Update stats
                const availableDrivers = this.driversData
                    ? this.driversData.filter(d => d.isAvailable).length
                    : 0;

                this.uiController.updateStats(
                    availableDrivers,
                    this.graphData.numVertices,
                    this.graphData.edges.length
                );

                console.log('Graph data loaded:', this.graphData);
            } else {
                throw new Error('Failed to load graph data');
            }
        } catch (error) {
            console.error('Error loading graph:', error);
            throw error;
        }
    }

    /**
     * Load drivers
     */
    async loadDrivers() {
        try {
            const response = await apiClient.getDrivers();

            if (response.success) {
                this.driversData = response.data;
                this.mapRenderer.loadDrivers(this.driversData);
                this.uiController.displayDrivers(this.driversData);

                // Update available drivers stat
                const availableDrivers = this.driversData.filter(d => d.isAvailable).length;
                this.uiController.updateStats(
                    availableDrivers,
                    this.graphData ? this.graphData.numVertices : 0,
                    this.graphData ? this.graphData.edges.length : 0
                );

                console.log('Drivers loaded:', this.driversData);
            } else {
                throw new Error('Failed to load drivers');
            }
        } catch (error) {
            console.error('Error loading drivers:', error);
            throw error;
        }
    }

    /**
     * Setup event listeners
     */
    setupEventListeners() {
        // Ride request form
        this.uiController.rideForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            await this.handleRideRequest();
        });

        // Map control buttons
        document.getElementById('zoom-in-btn').addEventListener('click', () => {
            this.mapRenderer.zoomIn();
        });

        document.getElementById('zoom-out-btn').addEventListener('click', () => {
            this.mapRenderer.zoomOut();
        });

        document.getElementById('reset-view-btn').addEventListener('click', () => {
            this.mapRenderer.resetView();
        });

        document.getElementById('toggle-labels-btn').addEventListener('click', () => {
            this.mapRenderer.toggleLabels();
        });

        // Pickup selection - show BLUE marker immediately
        this.uiController.pickupSelect.addEventListener('change', (e) => {
            const pickupId = parseInt(e.target.value);
            if (!isNaN(pickupId)) {
                this.mapRenderer.setPickupSelection(pickupId);
            } else {
                this.mapRenderer.clearSelections();
            }
            // Clear previous route but keep selections
            if (this.mapRenderer.driverToPickupPath.length > 0) {
                this.mapRenderer.driverToPickupPath = [];
                this.mapRenderer.pickupToDestPath = [];
                this.mapRenderer.assignedDriver = null;
                this.uiController.clearRideResult();
            }
        });

        // Destination selection - show RED marker immediately
        this.uiController.destinationSelect.addEventListener('change', (e) => {
            const destId = parseInt(e.target.value);
            if (!isNaN(destId)) {
                this.mapRenderer.setDestinationSelection(destId);
            } else if (this.mapRenderer.selectedPickup === null) {
                this.mapRenderer.clearSelections();
            }
            // Clear previous route but keep selections
            if (this.mapRenderer.driverToPickupPath.length > 0) {
                this.mapRenderer.driverToPickupPath = [];
                this.mapRenderer.pickupToDestPath = [];
                this.mapRenderer.assignedDriver = null;
                this.uiController.clearRideResult();
            }
        });
    }

    /**
     * Handle ride request submission
     */
    async handleRideRequest() {
        try {
            // Get form values
            const pickupLocation = parseInt(this.uiController.pickupSelect.value);
            const destinationLocation = parseInt(this.uiController.destinationSelect.value);

            // Validate
            if (isNaN(pickupLocation) || isNaN(destinationLocation)) {
                this.uiController.showError('Please select both pickup and destination locations');
                return;
            }

            if (pickupLocation === destinationLocation) {
                this.uiController.showError('Pickup and destination cannot be the same');
                return;
            }

            // Show loading
            this.uiController.showLoading();
            this.uiController.disableForm();

            // Make API request
            const response = await apiClient.requestRide(pickupLocation, destinationLocation);

            // Hide loading
            this.uiController.hideLoading();
            this.uiController.enableForm();

            if (response.success) {
                // Display result
                this.uiController.displayRideResult(response.data);

                // Visualize on map
                this.mapRenderer.setRideDetails(
                    pickupLocation,
                    destinationLocation,
                    response.data.driverToPickup.path,
                    response.data.pickupToDestination.path,
                    response.data.assignedDriver
                );

                // Reload drivers to update availability
                await this.loadDrivers();

                this.uiController.showSuccess('Ride matched successfully!');

                console.log('Ride matched:', response.data);
            } else {
                this.uiController.showError(response.error || 'Failed to match ride');
            }

        } catch (error) {
            console.error('Error requesting ride:', error);
            this.uiController.hideLoading();
            this.uiController.enableForm();
            this.uiController.showError('Error: ' + error.message);
        }
    }

    /**
     * Clear ride visualization
     */
    clearRideVisualization() {
        this.mapRenderer.clearRideDetails();
        this.uiController.clearRideResult();
    }

    /**
     * Refresh application data
     */
    async refresh() {
        try {
            this.uiController.showLoading();
            await this.loadGraphData();
            await this.loadDrivers();
            this.clearRideVisualization();
            this.uiController.hideLoading();
            this.uiController.showSuccess('Data refreshed');
        } catch (error) {
            console.error('Error refreshing data:', error);
            this.uiController.hideLoading();
            this.uiController.showError('Failed to refresh data');
        }
    }
}

// Initialize app when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    window.app = new App();

    // Start continuous rendering for driver pulse animation
    setInterval(() => {
        if (window.app && window.app.mapRenderer) {
            window.app.mapRenderer.render();
        }
    }, 50); // 20 FPS for smooth animation
});

// Handle window resize
window.addEventListener('resize', () => {
    if (window.app && window.app.mapRenderer) {
        window.app.mapRenderer.render();
    }
});
