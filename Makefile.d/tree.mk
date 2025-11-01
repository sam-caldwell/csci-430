# File: Makefile.d/tree.mk
# Purpose: Tree view of repository (requires 'tree')

TREE_ROOT ?= .
TREE_IGNORES ?= .git|cmake-build-*|build|build-*|out|.idea|CMakeFiles|Testing|__pycache__

.PHONY: tree
tree:
	@command -v tree >/dev/null 2>&1 || { echo "tree not found. Install with: brew install tree || sudo apt-get install tree"; exit 1; }
	@echo "Repository tree (excluding: $(TREE_IGNORES))"
	@tree -a -I "$(TREE_IGNORES)" "$(TREE_ROOT)"
