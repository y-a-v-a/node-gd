#!/bin/bash

# Docker test script for node-gd
# This script builds the Docker image and runs all tests within the container

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to log messages with timestamp
log() {
    echo -e "${BLUE}[$(date '+%Y-%m-%d %H:%M:%S')]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[$(date '+%Y-%m-%d %H:%M:%S')] ✓${NC} $1"
}

log_error() {
    echo -e "${RED}[$(date '+%Y-%m-%d %H:%M:%S')] ✗${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[$(date '+%Y-%m-%d %H:%M:%S')] ⚠${NC} $1"
}

# Docker image name
IMAGE_NAME="yava:node-gd"

echo "================================================"
echo "           Node-GD Docker Test Suite"
echo "================================================"
echo ""

# Step 1: Build Docker image
log "Building Docker image: $IMAGE_NAME"
if docker build --progress=plain -t "$IMAGE_NAME" . > build.log 2>&1; then
    log_success "Docker image built successfully"
else
    log_error "Docker image build failed"
    echo "Build log:"
    cat build.log
    exit 1
fi

echo ""

# Step 2: Run tests in container
log "Running tests in Docker container..."
echo "================================================"

# Create a temporary script to run inside the container
cat > /tmp/test_runner.sh << 'EOF'
#!/bin/bash
set -e

echo "Node.js version: $(node --version)"
echo "NPM version: $(npm --version)"
echo "Working directory: $(pwd)"
echo "Directory contents:"
ls -la

echo ""
echo "Installing dependencies..."
npm ci

echo ""
echo "Running tests..."
npm test

echo ""
echo "All tests completed successfully!"
EOF

chmod +x /tmp/test_runner.sh

# Run the container with volume mapping and execute tests
if docker run --rm \
    -v "$(pwd):/usr/src" \
    -v "/tmp/test_runner.sh:/tmp/test_runner.sh" \
    "$IMAGE_NAME" \
    /tmp/test_runner.sh > test_output.log 2>&1; then
    
    log_success "All tests passed successfully!"
    echo ""
    echo "Test output:"
    echo "================================================"
    cat test_output.log
    echo "================================================"
    
else
    log_error "Tests failed!"
    echo ""
    echo "Test output:"
    echo "================================================"
    cat test_output.log
    echo "================================================"
    exit 1
fi

echo ""
log_success "Docker test suite completed successfully!"

# Cleanup
rm -f build.log test_output.log /tmp/test_runner.sh

echo ""
echo "================================================"
echo "           Test Suite Summary"
echo "================================================"
echo "✓ Docker image built with Node.js 22 LTS"
echo "✓ All tests executed in containerized environment"
echo "✓ Test results logged and displayed"
echo "================================================"