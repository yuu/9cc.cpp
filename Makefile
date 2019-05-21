.PHONY: help build release test
.DEFAULT_GOAL := help

BUILD_DIR := build

help:
	@grep -E '^[0-9a-zA-Z_-]+:.*?## .*$$' ${MAKEFILE_LIST} | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

debug_git_tag:
	$(eval ver = $(shell git describe --dirty --all | cut -d / -f 2))

release_git_tag:
	$(eval ver = $(shell git describe --dirty --tags))

build: ## debug build
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR)

release: ## release build
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

test: ## run unit test
	@./test.sh
