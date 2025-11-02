#!/bin/bash
# Helper script to get started with fil-C memory safety checking

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

clear

cat << "EOF"
   _____ _           
  / ____| |          
 | |    | | ___  ___ 
 | |    | |/ _ \/ __|
 | |____| |  __/ (__ 
  \_____|_|\___\___|
                     
 __ _  _        _____ 
|  _| || |      / ____|
|  |_|  | |_   | |     
|  _  || |_|  | |     
| | | || |_   | |____ 
|_| |_||_|_|   \_____|
                      
EOF

echo -e "${BLUE}=== Flecs Memory Safety Testing with fil-C ===${NC}"
echo ""
echo "This setup helps you check flecs for memory safety issues using fil-C,"
echo "a memory-safe implementation of C that catches all memory safety violations."
echo ""

# Check system
echo -e "${YELLOW}Checking your system...${NC}"
PLATFORM=$(uname -s)
ARCH=$(uname -m)
echo "Platform: $PLATFORM $ARCH"
echo ""

if [[ "$PLATFORM" == "Darwin" ]]; then
    echo "✓ macOS detected - Docker-based solution available"
    echo ""
    
    # Check Docker
    if command -v docker &> /dev/null; then
        echo -e "${GREEN}✓ Docker is installed${NC}"
        CAN_RUN=true
    elif command -v podman &> /dev/null; then
        echo -e "${GREEN}✓ Podman is installed${NC}"
        CAN_RUN=true
    else
        echo "⚠️  Docker/Podman not found"
        echo ""
        echo "To use fil-C on macOS, you need Docker or Podman."
        echo "Install Docker Desktop from: https://www.docker.com/products/docker-desktop/"
        echo ""
        CAN_RUN=false
    fi
    
    if [[ "$CAN_RUN" == "true" ]]; then
        echo ""
        echo -e "${GREEN}=== You're ready to go! ===${NC}"
        echo ""
        echo "Quick Start Commands:"
        echo ""
        echo -e "${BLUE}1. Build and test flecs with fil-C:${NC}"
        echo "   ./docker_filc_build.sh"
        echo ""
        echo -e "${BLUE}2. Interactive development (recommended for first time):${NC}"
        echo "   ./docker_filc_build.sh shell"
        echo ""
        echo -e "${BLUE}3. Read the quick guide:${NC}"
        echo "   cat QUICKSTART_FILC.md"
        echo ""
        
        read -p "Run the build now? [y/N] " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            echo ""
            echo "Starting build..."
            echo ""
            ./docker_filc_build.sh
        else
            echo ""
            echo "Run './docker_filc_build.sh' when you're ready!"
        fi
    fi
    
elif [[ "$PLATFORM" == "Linux" ]] && [[ "$ARCH" == "x86_64" ]]; then
    echo -e "${GREEN}✓ Linux x86_64 detected - Native fil-C support available${NC}"
    echo ""
    echo "Quick Start Commands:"
    echo ""
    echo -e "${BLUE}1. Install fil-C and build flecs:${NC}"
    echo "   ./compile_with_filc.sh"
    echo ""
    echo -e "${BLUE}2. Read the quick guide:${NC}"
    echo "   cat QUICKSTART_FILC.md"
    echo ""
    
    read -p "Run the build now? [y/N] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo ""
        echo "Starting build..."
        echo ""
        ./compile_with_filc.sh
    else
        echo ""
        echo "Run './compile_with_filc.sh' when you're ready!"
    fi
    
else
    echo "⚠️  Platform not directly supported by fil-C"
    echo ""
    echo "fil-C currently only supports Linux x86_64."
    echo ""
    echo "Options:"
    echo "  1. Use Docker/Podman with the docker_filc_build.sh script"
    echo "  2. Use a Linux x86_64 VM"
    echo "  3. Use a remote Linux x86_64 machine"
    echo ""
    echo "See README_FILC.md for detailed instructions."
fi

echo ""
echo -e "${BLUE}=== Documentation ===${NC}"
echo ""
echo "📖 QUICKSTART_FILC.md  - Quick reference guide"
echo "📚 README_FILC.md      - Detailed documentation"
echo "📝 SUMMARY_FILC.md     - Complete overview"
echo ""
echo "🌐 fil-C website: https://fil-c.org/"
echo "💬 fil-C Discord: https://discord.gg/dPyNUaeajg"
echo "💬 Flecs Discord: https://discord.gg/BEzP5Rgrrp"
echo ""
