.PHONY: test iso

test:
	ctest --preset=debug-no-vcpkg

iso:
	$(MAKE) -C components/nas-installer iso
