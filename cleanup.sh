# cleanup.sh - Clean build artifacts
#!/bin/bash

echo "Cleaning build artifacts..."

# Remove build directories
rm -rf build/
rm -rf build_cross/
rm -rf bin/
rm -rf obj/

# Remove temporary files
find . -name "*.obj" -delete
find . -name "*.pdb" -delete
find . -name "*.ilk" -delete
find . -name "*.exp" -delete
find . -name "*.lib" -delete
find . -name "*.sdf" -delete
find . -name "*.suo" -delete
find . -name "*.user" -delete
find . -name "*.ncb" -delete
find . -name "*.aps" -delete

# Remove installer output
rm -rf Output/

echo "Cleanup complete!"