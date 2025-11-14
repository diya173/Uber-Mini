/**
 * map-renderer.js
 *
 * Renders the city map with nodes, edges, drivers, and routes
 * Shows weighted edges, driver names, and real-time selection highlighting
 */

class MapRenderer {
    constructor(canvasId) {
        this.canvas = document.getElementById(canvasId);
        this.ctx = this.canvas.getContext('2d');

        // Map data
        this.graph = null;
        this.drivers = [];
        this.selectedPickup = null;
        this.selectedDestination = null;
        this.driverToPickupPath = [];
        this.pickupToDestPath = [];
        this.assignedDriver = null;

        // Rendering state
        this.scale = 1.0;
        this.offsetX = 0;
        this.offsetY = 0;
        this.showLabels = true;
        this.showWeights = true; // Show edge weights

        // Animation
        this.animationFrame = 0;
        this.isAnimating = false;

        // Dragging
        this.isDragging = false;
        this.lastMouseX = 0;
        this.lastMouseY = 0;

        this.initCanvas();
        this.setupEventListeners();
    }

    /**
     * Initialize canvas size
     */
    initCanvas() {
        const container = this.canvas.parentElement;
        this.canvas.width = container.clientWidth;
        this.canvas.height = container.clientHeight;

        window.addEventListener('resize', () => {
            this.canvas.width = container.clientWidth;
            this.canvas.height = container.clientHeight;
            this.render();
        });
    }

    /**
     * Setup mouse event listeners for pan functionality
     */
    setupEventListeners() {
        this.canvas.addEventListener('mousedown', (e) => {
            this.isDragging = true;
            this.lastMouseX = e.clientX;
            this.lastMouseY = e.clientY;
        });

        this.canvas.addEventListener('mousemove', (e) => {
            if (this.isDragging) {
                const dx = e.clientX - this.lastMouseX;
                const dy = e.clientY - this.lastMouseY;

                this.offsetX += dx;
                this.offsetY += dy;

                this.lastMouseX = e.clientX;
                this.lastMouseY = e.clientY;

                this.render();
            }
        });

        this.canvas.addEventListener('mouseup', () => {
            this.isDragging = false;
        });

        this.canvas.addEventListener('mouseleave', () => {
            this.isDragging = false;
        });

        // Mouse wheel for zoom
        this.canvas.addEventListener('wheel', (e) => {
            e.preventDefault();
            const zoomSpeed = 0.1;
            const delta = e.deltaY > 0 ? -zoomSpeed : zoomSpeed;

            this.scale = Math.max(0.2, Math.min(3.0, this.scale + delta));
            this.render();
        });
    }

    /**
     * Load graph data
     */
    loadGraph(graphData) {
        this.graph = graphData;
        this.centerView();
        this.render();
    }

    /**
     * Load drivers
     */
    loadDrivers(drivers) {
        this.drivers = drivers;
        this.render();
    }

    /**
     * Set pickup selection (called when user selects from dropdown)
     */
    setPickupSelection(nodeId) {
        this.selectedPickup = nodeId;
        this.render();
    }

    /**
     * Set destination selection (called when user selects from dropdown)
     */
    setDestinationSelection(nodeId) {
        this.selectedDestination = nodeId;
        this.render();
    }

    /**
     * Clear selections
     */
    clearSelections() {
        this.selectedPickup = null;
        this.selectedDestination = null;
        this.render();
    }

    /**
     * Center view on graph
     */
    centerView() {
        if (!this.graph || !this.graph.nodes || this.graph.nodes.length === 0) {
            return;
        }

        // Calculate bounds
        let minLat = Infinity, maxLat = -Infinity;
        let minLon = Infinity, maxLon = -Infinity;

        for (const node of this.graph.nodes) {
            minLat = Math.min(minLat, node.latitude);
            maxLat = Math.max(maxLat, node.latitude);
            minLon = Math.min(minLon, node.longitude);
            maxLon = Math.max(maxLon, node.longitude);
        }

        // Calculate center and scale
        const centerLat = (minLat + maxLat) / 2;
        const centerLon = (minLon + maxLon) / 2;

        const latRange = maxLat - minLat || 0.1;
        const lonRange = maxLon - minLon || 0.1;

        const scaleX = this.canvas.width / (lonRange * 10000);
        const scaleY = this.canvas.height / (latRange * 10000);

        this.scale = Math.min(scaleX, scaleY) * 0.8;

        // Center offset
        this.offsetX = this.canvas.width / 2;
        this.offsetY = this.canvas.height / 2;

        // Store center for coordinate conversion
        this.centerLat = centerLat;
        this.centerLon = centerLon;
    }

    /**
     * Convert lat/lon to canvas coordinates
     */
    latLonToCanvas(lat, lon) {
        const x = (lon - this.centerLon) * 10000 * this.scale + this.offsetX;
        const y = -(lat - this.centerLat) * 10000 * this.scale + this.offsetY;
        return { x, y };
    }

    /**
     * Zoom in
     */
    zoomIn() {
        this.scale = Math.min(3.0, this.scale + 0.2);
        this.render();
    }

    /**
     * Zoom out
     */
    zoomOut() {
        this.scale = Math.max(0.2, this.scale - 0.2);
        this.render();
    }

    /**
     * Reset view
     */
    resetView() {
        this.centerView();
        this.render();
    }

    /**
     * Toggle labels
     */
    toggleLabels() {
        this.showLabels = !this.showLabels;
        this.render();
    }

    /**
     * Set ride details for visualization
     */
    setRideDetails(pickup, destination, driverToPickupPath, pickupToDestPath, driver) {
        this.selectedPickup = pickup;
        this.selectedDestination = destination;
        this.driverToPickupPath = driverToPickupPath || [];
        this.pickupToDestPath = pickupToDestPath || [];
        this.assignedDriver = driver;
        this.startAnimation();
    }

    /**
     * Clear ride details
     */
    clearRideDetails() {
        this.selectedPickup = null;
        this.selectedDestination = null;
        this.driverToPickupPath = [];
        this.pickupToDestPath = [];
        this.assignedDriver = null;
        this.isAnimating = false;
        this.render();
    }

    /**
     * Start route animation
     */
    startAnimation() {
        this.isAnimating = true;
        this.animationFrame = 0;
        this.animate();
    }

    /**
     * Animation loop
     */
    animate() {
        if (!this.isAnimating) return;

        this.animationFrame += 0.02;
        if (this.animationFrame > 1) {
            this.animationFrame = 1;
        }

        this.render();

        if (this.animationFrame < 1) {
            requestAnimationFrame(() => this.animate());
        }
    }

    /**
     * Main render function
     */
    render() {
        // Clear canvas
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);

        if (!this.graph) return;

        // Draw edges (with weights)
        this.drawEdges();

        // Draw route paths (if any)
        this.drawRoutePaths();

        // Draw nodes
        this.drawNodes();

        // Draw drivers (with names)
        this.drawDrivers();

        // Draw selected pickup and destination (REAL-TIME)
        this.drawSelectedLocations();
    }

    /**
     * Draw all edges with weights
     */
    drawEdges() {
        if (!this.graph.edges) return;

        this.ctx.strokeStyle = '#d0d0d0';
        this.ctx.lineWidth = 2;

        for (const edge of this.graph.edges) {
            const sourceNode = this.graph.nodes.find(n => n.id === edge.source);
            const destNode = this.graph.nodes.find(n => n.id === edge.destination);

            if (!sourceNode || !destNode) continue;

            const start = this.latLonToCanvas(sourceNode.latitude, sourceNode.longitude);
            const end = this.latLonToCanvas(destNode.latitude, destNode.longitude);

            // Draw edge line
            this.ctx.beginPath();
            this.ctx.moveTo(start.x, start.y);
            this.ctx.lineTo(end.x, end.y);
            this.ctx.stroke();

            // Draw weight label (if zoomed in enough)
            if (this.showWeights && this.scale > 0.6) {
                const midX = (start.x + end.x) / 2;
                const midY = (start.y + end.y) / 2;

                // Background for weight label
                this.ctx.fillStyle = 'rgba(255, 255, 255, 0.9)';
                const weightText = edge.weight.toFixed(1) + ' km';
                const textWidth = this.ctx.measureText(weightText).width;
                this.ctx.fillRect(midX - textWidth / 2 - 3, midY - 8, textWidth + 6, 16);

                // Weight text
                this.ctx.fillStyle = '#555';
                this.ctx.font = 'bold 11px Arial';
                this.ctx.textAlign = 'center';
                this.ctx.textBaseline = 'middle';
                this.ctx.fillText(weightText, midX, midY);
            }
        }
    }

    /**
     * Draw route paths with animation
     */
    drawRoutePaths() {
        // Draw driver to pickup path (green)
        if (this.driverToPickupPath.length > 1) {
            this.drawPath(this.driverToPickupPath, '#5cb85c', 4, this.animationFrame);
        }

        // Draw pickup to destination path (blue to red gradient)
        if (this.pickupToDestPath.length > 1) {
            this.drawPath(this.pickupToDestPath, '#0066ff', 5, this.animationFrame);
        }
    }

    /**
     * Draw a path with animation
     */
    drawPath(path, color, lineWidth, progress) {
        if (path.length < 2) return;

        this.ctx.strokeStyle = color;
        this.ctx.lineWidth = lineWidth;
        this.ctx.lineCap = 'round';
        this.ctx.lineJoin = 'round';

        // Draw path with glow effect
        this.ctx.shadowColor = color;
        this.ctx.shadowBlur = 10;

        this.ctx.beginPath();

        for (let i = 0; i < path.length - 1; i++) {
            const currentNode = this.graph.nodes.find(n => n.id === path[i]);
            const nextNode = this.graph.nodes.find(n => n.id === path[i + 1]);

            if (!currentNode || !nextNode) continue;

            const start = this.latLonToCanvas(currentNode.latitude, currentNode.longitude);
            const end = this.latLonToCanvas(nextNode.latitude, nextNode.longitude);

            if (i === 0) {
                this.ctx.moveTo(start.x, start.y);
            }

            // Animate path drawing
            const segmentProgress = Math.min(1, Math.max(0, progress * path.length - i));
            const x = start.x + (end.x - start.x) * segmentProgress;
            const y = start.y + (end.y - start.y) * segmentProgress;

            this.ctx.lineTo(x, y);

            if (segmentProgress < 1) break;
        }

        this.ctx.stroke();
        this.ctx.shadowBlur = 0;

        // Draw animated dot at the front
        if (progress < 1) {
            const idx = Math.min(Math.floor(progress * (path.length - 1)), path.length - 2);
            const currentNode = this.graph.nodes.find(n => n.id === path[idx]);
            const nextNode = this.graph.nodes.find(n => n.id === path[idx + 1]);

            if (currentNode && nextNode) {
                const start = this.latLonToCanvas(currentNode.latitude, currentNode.longitude);
                const end = this.latLonToCanvas(nextNode.latitude, nextNode.longitude);

                const segmentProgress = (progress * (path.length - 1)) - idx;
                const x = start.x + (end.x - start.x) * segmentProgress;
                const y = start.y + (end.y - start.y) * segmentProgress;

                this.ctx.fillStyle = color;
                this.ctx.shadowColor = color;
                this.ctx.shadowBlur = 15;
                this.ctx.beginPath();
                this.ctx.arc(x, y, 8, 0, Math.PI * 2);
                this.ctx.fill();
                this.ctx.shadowBlur = 0;
            }
        }
    }

    /**
     * Draw all nodes
     */
    drawNodes() {
        if (!this.graph.nodes) return;

        for (const node of this.graph.nodes) {
            const pos = this.latLonToCanvas(node.latitude, node.longitude);

            // Check if this node is pickup or destination
            const isPickup = this.selectedPickup !== null && node.id === this.selectedPickup;
            const isDestination = this.selectedDestination !== null && node.id === this.selectedDestination;

            // Skip if it's pickup or destination (drawn separately)
            if (isPickup || isDestination) continue;

            // Draw node circle
            this.ctx.fillStyle = '#6c757d';
            this.ctx.beginPath();
            this.ctx.arc(pos.x, pos.y, 6, 0, Math.PI * 2);
            this.ctx.fill();

            // Draw node border
            this.ctx.strokeStyle = '#495057';
            this.ctx.lineWidth = 2;
            this.ctx.stroke();

            // Draw label
            if (this.showLabels && this.scale > 0.5) {
                this.ctx.fillStyle = '#212529';
                this.ctx.font = '12px Arial';
                this.ctx.textAlign = 'center';
                this.ctx.fillText(node.name, pos.x, pos.y - 12);
            }
        }
    }

    /**
     * Draw drivers with names and colors
     */
    drawDrivers() {
        for (const driver of this.drivers) {
            const node = this.graph.nodes.find(n => n.id === driver.currentLocation);
            if (!node) continue;

            const pos = this.latLonToCanvas(node.latitude, node.longitude);

            // Check if this is the assigned driver
            const isAssigned = this.assignedDriver && driver.id === this.assignedDriver.id;

            // Driver marker with different colors
            const pulse = Math.sin(Date.now() * 0.003) * 0.2 + 1;

            if (isAssigned) {
                // Assigned driver - special gold color
                this.ctx.fillStyle = '#FFD700';
                this.ctx.shadowColor = '#FFD700';
            } else if (driver.isAvailable) {
                // Available driver - green
                this.ctx.fillStyle = '#5cb85c';
                this.ctx.shadowColor = '#5cb85c';
            } else {
                // Busy driver - gray
                this.ctx.fillStyle = '#999';
                this.ctx.shadowColor = '#999';
            }

            this.ctx.shadowBlur = 15 * pulse;
            this.ctx.beginPath();
            this.ctx.arc(pos.x, pos.y, isAssigned ? 14 : 10, 0, Math.PI * 2);
            this.ctx.fill();
            this.ctx.shadowBlur = 0;

            // Driver icon
            this.ctx.fillStyle = 'white';
            this.ctx.font = isAssigned ? 'bold 16px Arial' : 'bold 12px Arial';
            this.ctx.textAlign = 'center';
            this.ctx.textBaseline = 'middle';
            this.ctx.fillText('🚗', pos.x, pos.y);

            // Driver name label
            if (this.scale > 0.5) {
                // Background for name
                const nameText = driver.name;
                const textWidth = this.ctx.measureText(nameText).width;

                this.ctx.fillStyle = isAssigned ? 'rgba(255, 215, 0, 0.9)' :
                                     driver.isAvailable ? 'rgba(92, 184, 92, 0.9)' : 'rgba(153, 153, 153, 0.9)';
                this.ctx.fillRect(pos.x - textWidth / 2 - 4, pos.y + 18, textWidth + 8, 18);

                // Name text
                this.ctx.fillStyle = 'white';
                this.ctx.font = isAssigned ? 'bold 12px Arial' : '11px Arial';
                this.ctx.textAlign = 'center';
                this.ctx.textBaseline = 'middle';
                this.ctx.fillText(nameText, pos.x, pos.y + 27);

                // Vehicle type
                if (this.scale > 0.8) {
                    this.ctx.fillStyle = '#333';
                    this.ctx.font = '9px Arial';
                    this.ctx.fillText(driver.vehicleType, pos.x, pos.y + 40);
                }
            }
        }
    }

    /**
     * Draw selected pickup and destination (REAL-TIME highlighting)
     */
    drawSelectedLocations() {
        // Draw pickup location (BLUE) - shown immediately when selected
        if (this.selectedPickup !== null) {
            const node = this.graph.nodes.find(n => n.id === this.selectedPickup);
            if (node) {
                const pos = this.latLonToCanvas(node.latitude, node.longitude);

                // Pulsing effect
                const pulse = Math.sin(Date.now() * 0.005) * 3 + 12;

                this.ctx.fillStyle = '#0066ff';
                this.ctx.shadowColor = '#0066ff';
                this.ctx.shadowBlur = 20;
                this.ctx.beginPath();
                this.ctx.arc(pos.x, pos.y, pulse, 0, Math.PI * 2);
                this.ctx.fill();
                this.ctx.shadowBlur = 0;

                // Inner circle
                this.ctx.fillStyle = '#ffffff';
                this.ctx.beginPath();
                this.ctx.arc(pos.x, pos.y, 6, 0, Math.PI * 2);
                this.ctx.fill();

                // P label
                this.ctx.fillStyle = '#0066ff';
                this.ctx.font = 'bold 14px Arial';
                this.ctx.textAlign = 'center';
                this.ctx.textBaseline = 'middle';
                this.ctx.fillText('P', pos.x, pos.y);

                // Location name
                if (this.showLabels) {
                    this.ctx.fillStyle = '#0066ff';
                    this.ctx.font = 'bold 14px Arial';
                    this.ctx.fillText(node.name, pos.x, pos.y - 22);
                }
            }
        }

        // Draw destination location (RED) - shown immediately when selected
        if (this.selectedDestination !== null) {
            const node = this.graph.nodes.find(n => n.id === this.selectedDestination);
            if (node) {
                const pos = this.latLonToCanvas(node.latitude, node.longitude);

                // Pulsing effect
                const pulse = Math.sin(Date.now() * 0.005) * 3 + 12;

                this.ctx.fillStyle = '#ff3366';
                this.ctx.shadowColor = '#ff3366';
                this.ctx.shadowBlur = 20;
                this.ctx.beginPath();
                this.ctx.arc(pos.x, pos.y, pulse, 0, Math.PI * 2);
                this.ctx.fill();
                this.ctx.shadowBlur = 0;

                // Inner circle
                this.ctx.fillStyle = '#ffffff';
                this.ctx.beginPath();
                this.ctx.arc(pos.x, pos.y, 6, 0, Math.PI * 2);
                this.ctx.fill();

                // D label
                this.ctx.fillStyle = '#ff3366';
                this.ctx.font = 'bold 14px Arial';
                this.ctx.textAlign = 'center';
                this.ctx.textBaseline = 'middle';
                this.ctx.fillText('D', pos.x, pos.y);

                // Location name
                if (this.showLabels) {
                    this.ctx.fillStyle = '#ff3366';
                    this.ctx.font = 'bold 14px Arial';
                    this.ctx.fillText(node.name, pos.x, pos.y - 22);
                }
            }
        }
    }
}
