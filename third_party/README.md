# Third-Party Dependencies

This directory contains third-party, externally maintained software
that is required for documentation or visualization, but is not
authored or built as part of this repository.

All contents of this directory are treated as **vendored dependencies**:
they are committed in full, remain unmodified, and are version-pinned
to ensure long-term reproducibility.

------------------------------------------------------------

## MathJax

MathJax is used exclusively for documentation rendering and does not participate in any numerical computation or data generation.

- **Version:** 2.7.9
- **Source:** https://github.com/mathjax/MathJax
- **License:** Apache-2.0

MathJax is included locally to support mathematical rendering in generated HTML documentation (e.g., Doxygen output) under `/build/doxygen/html` without relying on external CDNs. This ensures:

- Offline documentation viewing
- Long-term reproducibility
- No external network dependencies during documentation builds

The MathJax files are stored under the local path:

    MathJax-2.7.9/

are referenced directly by the documentation toolchain.

------------------------------------------------------------

## Update Policy

Third-party dependencies in this directory should only be updated when:

- A specific feature or bug fix is required
- A security issue necessitates an upgrade
- Documentation tooling requires a newer version

Updates should preserve the full upstream directory structure and
clearly record version changes in commit history.
