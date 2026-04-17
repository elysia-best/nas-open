# nas-open

An opensource nas system.

## Componentized workspace

Implemented scaffolds:

- `components/nas-common` (base contracts and toolchain bootstrap)
- `components/nasd-core` (plugin loader + service health skeleton)
- `components/nas-gateway` (REST/auth/ws/rate-limit skeleton)
- `components/nas-cli` (command routing + dry-run CLI skeleton)
- `components/nas-web-ui` (Vite scaffold)
- `components/nas-storage` (dry-run storage plugin scaffold)
- `components/nas-monitor` (monitoring plugin scaffold)
- `components/nas-installer` (ISO/firstboot placeholder entry)
- `proto/nas.proto` (shared gRPC contract)
- `gateway/openapi.yaml` (REST mapping contract)

## Build & Test (C++)

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

## Web UI (scaffold)

```bash
cd components/nas-web-ui
npm install
npm run dev
```

## Interactive CLI

The `nas-cli` offers both one-shot commands and an interactive shell using GNU readline.
The shell supports command history and basic parsing equivalent to shell tokenization.

```bash
# Non-interactive mode
nas-cli storage pool list --json

# Interactive mode
nas-cli
nas-cli > help
nas-cli > storage snapshot create pool/dataset@snap1
nas-cli > quit
```

## Integration placeholders

```bash
make test
make iso
```
