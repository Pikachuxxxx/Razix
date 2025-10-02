# CI/CD Workflow Changes

## Summary
Modified the CI Build workflow to run only before PRs are merged, not on every commit to the master branch.

## Changes Made

### 1. Removed `push` Trigger
- **Before**: CI ran on every push to the `master` branch
- **After**: CI no longer runs on pushes to `master` (after merge)
- **Reason**: Prevents redundant builds after code is already merged

### 2. Enhanced `pull_request` Trigger
- **Added specific PR types**: `[opened, synchronize, reopened, ready_for_review]`
- **Behavior**:
  - `opened`: Runs when a new PR is created (if not draft)
  - `synchronize`: Runs when new commits are pushed to the PR branch
  - `reopened`: Runs when a closed PR is reopened
  - `ready_for_review`: Runs when a draft PR is marked as ready for review

### 3. Draft PR Handling
- **Added condition**: `if: github.event.pull_request.draft == false`
- **Behavior**: CI builds are skipped for draft PRs
- **Workflow**: 
  1. Create PR as draft → No CI runs
  2. Push multiple commits → No CI runs (still draft)
  3. Mark as "Ready for review" → CI runs
  4. Additional commits → CI runs on each push (to catch regressions)

### 4. Manual Triggering
- **Added**: `workflow_dispatch` event
- **Purpose**: Allows manual triggering of CI builds from GitHub Actions UI when needed

## Benefits

1. **Resource Efficiency**: Reduces unnecessary CI runs on the master branch after merges
2. **Faster Development**: Developers can push multiple commits to draft PRs without triggering CI
3. **Better Control**: CI runs only when code is ready for review or about to be merged
4. **Validation**: Still ensures all code is tested before merge
5. **Flexibility**: Manual triggering available for special cases

## Workflow Examples

### Example 1: Draft PR Flow
```
1. Create draft PR from feature branch → ❌ No CI
2. Push commit 1 → ❌ No CI
3. Push commit 2 → ❌ No CI  
4. Push commit 3 → ❌ No CI
5. Mark as "Ready for review" → ✅ CI runs
6. Push commit 4 (fix review comments) → ✅ CI runs
7. Merge to master → ❌ No CI (already validated)
```

### Example 2: Direct PR Flow
```
1. Create PR (not draft) → ✅ CI runs
2. Push commit (fix) → ✅ CI runs
3. Merge to master → ❌ No CI (already validated)
```

### Example 3: Manual Trigger
```
1. Navigate to Actions tab
2. Select "CI Build" workflow
3. Click "Run workflow"
4. Select branch and run → ✅ CI runs
```

## Migration Notes

- Existing PRs will continue to work as before
- New PRs should follow the draft → ready flow for optimal CI usage
- If immediate CI validation is needed for draft PRs, mark them as ready temporarily
- The scheduled stability builds (schedule.yml) continue to run independently

## Related Workflows

- **release.yml**: Unchanged - still runs on version tags
- **schedule.yml**: Unchanged - still runs on weekly schedule
