# File: Makefile.d/tree.mk
# Purpose: Show a filtered repository tree for quick inspection.
# Targets:
#  - tree: runs the external 'tree' tool with ignores.
# Variables:
#  - TREE_ROOT, TREE_IGNORES
# Tree view of repository (requires 'tree')
TREE_ROOT ?= .
TREE_IGNORES ?= .git|cmake-build-*|build|build-*|out|.idea|CMakeFiles|Testing|__pycache__
tree:
	@command -v tree >/dev/null 2>&1 || { echo "tree not found. Install with: brew install tree"; exit 1; }
	@echo "Repository tree (excluding: $(TREE_IGNORES))"
	@tree -a -I "$(TREE_IGNORES)" "$(TREE_ROOT)"
