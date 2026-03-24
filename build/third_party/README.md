# getting win32 sdk to work
use the auto-toolchain detection first
use "bazel query --output=build" on that auto-detected toolchain (you see it's label if you start Bazel with toolchain resolution debugging)
write this toolchain definition into my own file, modify it to my liking, and registering my own version from MODULE.bazel
turn off Bazels auto-toolchain creation