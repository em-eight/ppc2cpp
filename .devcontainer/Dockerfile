FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu22.04

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update
RUN apt install -y --no-install-recommends \
    build-essential cmake \
    protobuf-compiler \
    libprotobuf-dev \
    libz-dev
