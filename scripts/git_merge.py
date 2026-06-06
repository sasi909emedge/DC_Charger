import subprocess
import sys
import shlex

def run_git_command(command):

    result = subprocess.run(
        ["git"] + shlex.split(command),
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        print(result.stderr)
        sys.exit(1)

    print(result.stdout)
    return result.stdout


def check_clean():

    result = subprocess.run(
        ["git", "status", "--porcelain"],
        capture_output=True,
        text=True
    )

    if result.stdout:
        print("Uncommitted changes found")
        print(result.stdout)
        print("Commit first")
        sys.exit(1)

    print("Working tree clean")

def auto_commit_feature(
        feature_branch,
        message="auto firmware update"):

    print("Checking for new changes...")

    result = subprocess.run(
        ["git", "status", "--porcelain"],
        capture_output=True,
        text=True
    )


    if not result.stdout:
        print("No changes to commit")
        return


    print("Adding changes...")

    run_git_command(
        "add ."
    )


    print("Creating commit...")

    run_git_command(
        f'commit -m "{message}"'
    )


    print(
        f"Pushing {feature_branch}"
    )

    run_git_command(
        f"push origin {feature_branch}"
    )    


def git_checkout(branch):

    print(f"Switching to {branch}")

    run_git_command(
        f"checkout {branch}"
    )


def git_pull(branch):

    print(
        f"Pulling {branch}"
    )

    run_git_command(
        f"pull origin {branch}"
    )


def git_merge(branch):

    print(
        f"Merging {branch}"
    )

    result = subprocess.run(
        [
            "git",
            "merge",
            branch
        ],
        capture_output=True,
        text=True
    )

    print(result.stdout)

    if result.returncode != 0:

        print(result.stderr)

        if "CONFLICT" in result.stdout + result.stderr:
            print(
                "Merge conflict detected"
            )

        sys.exit(1)


def git_push(branch):

    print(
        f"Pushing {branch}"
    )

    run_git_command(
        f"push origin {branch}"
    )


def merge_feature_to_master(
        feature_branch,
        master_branch):


    auto_commit_feature(
        feature_branch
    )


    check_clean()


    git_checkout(
        master_branch
    )


    git_pull(
        master_branch
    )


    git_merge(
        feature_branch
    )


    git_push(
        master_branch
    )


    git_checkout(
        feature_branch
    )


    git_merge(
        master_branch
    )


    print(
        "Git merge completed successfully"
    )


if __name__ == "__main__":

    merge_feature_to_master(
        "feature-branch",
        "master"
    )