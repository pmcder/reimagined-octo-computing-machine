# Git Workflow

## Branching
- **Never commit directly to `main`.** Before making any change, check the current branch; if on `main`, create a new branch first, based off the latest `main`.
- Branch names require one of these prefixes:
  - `feature/<short-kebab-description>` — new functionality
  - `fix/<short-kebab-description>` — bug fixes
  - `chore/<short-kebab-description>` — tooling, CI, deps, repo maintenance
  - `docs/<short-kebab-description>` — documentation-only changes
  - `refactor/<short-kebab-description>` — internal restructuring with no behavior change

## Atomic commits
- Each commit represents one logical, self-contained change. Don't mix unrelated changes (e.g. a source fix + a CMake change + a doc update) in one commit — split into separate commits.
- Each commit should leave the project in a working state (builds; tests pass if present).
- Commit message: short imperative summary line (e.g. "Add atomic-commit rule to CLAUDE.md"), with an optional body explaining *why* for non-trivial changes.
- Prefer several small, reviewable commits over one large one.

## When to apply this
This governs Claude's own git workflow in this repo. Only branch or commit when the user asks — these rules describe *how*, not a standing instruction to act unprompted.
