# nas-open

An opensource nas system.

## Componentized workspace (Stage 1 scaffold)

This repository now contains the initial componentized skeleton:

- `components/nas-common` (base contracts and toolchain bootstrap)
- `components/nasd-core`
- `components/nas-storage`
- `components/nas-monitor`
- `components/nas-gateway`
- `components/nas-cli`
- `components/nas-web-ui`
- `components/nas-installer`
- `proto/nas.proto` (shared gRPC contract)
- `gateway/openapi.yaml` (REST mapping contract)

## Build

With vcpkg toolchain:

```bash
cmake --preset=debug
cmake --build --preset=debug
```

Without vcpkg:

```bash
cmake --preset=debug-no-vcpkg
cmake --build --preset=debug-no-vcpkg
ctest --preset=debug-no-vcpkg
```
