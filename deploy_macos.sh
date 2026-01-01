#!/bin/bash
# macOS deployment script for WouterAsio

set -e  # Exit on error

echo "Building WouterAsio for macOS distribution..."

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Clean and build
echo -e "${BLUE}Step 1: Building project...${NC}"
cd build
cmake --build . --config Release
cd ..

# Check if app bundle was created
if [ ! -d "build/wouterasio.app" ]; then
    echo "Error: App bundle not found at build/wouterasio.app"
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"

# Deploy Qt frameworks
echo -e "${BLUE}Step 2: Bundling Qt frameworks with macdeployqt...${NC}"
/opt/homebrew/opt/qt@5/bin/macdeployqt build/wouterasio.app

echo -e "${GREEN}Qt frameworks bundled successfully!${NC}"

# Optional: Create DMG for distribution
echo -e "${BLUE}Step 3 (optional): Create DMG for distribution?${NC}"
read -p "Create DMG? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    DMG_NAME="WouterAsio-1.0.dmg"
    echo "Creating $DMG_NAME..."

    # Remove old DMG if exists
    rm -f "$DMG_NAME"

    # Create DMG
    hdiutil create -volname "WouterAsio" -srcfolder build/wouterasio.app -ov -format UDZO "$DMG_NAME"

    echo -e "${GREEN}DMG created: $DMG_NAME${NC}"
fi

echo -e "${GREEN}Deployment complete!${NC}"
echo "Your app bundle is at: build/wouterasio.app"
echo "You can now:"
echo "  1. Double-click build/wouterasio.app to run it"
echo "  2. Copy wouterasio.app to /Applications"
echo "  3. Distribute the DMG file (if created)"
