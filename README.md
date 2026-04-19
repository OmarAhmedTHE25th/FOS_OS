# FOS Operating System - Project 2026
**Ain Shams University | Faculty of Engineering | Computer Engineering**

---

##  Team Setup Guide: Connecting to GitHub
Follow these steps exactly to get your environment synced without breaking the project paths.

### 1. Prerequisites
* Ensure you have the `fos_cygwin` folder extracted to your **D: drive** (highly recommended for path consistency).
* Make sure you have your GitHub account added to your local Git config.

### 2. Cloning the Repo
Open your **Cygwin terminal** and run:
```bash
cd /cygdrive/d/fos_cygwin/FOS_CODES/
git clone https://github.com/OmarAhmedTHE25th/FOS_OS.git
```
*Note: This will create the project folder inside `FOS_CODES`.*

### 3. Importing to Eclipse
1. Open Eclipse using `FOS_Eclipse.bat`.
2. Go to **File > Import...**
3. Select **General > Existing Projects into Workspace** and click Next.
4. Browse to the folder you just cloned and click **Finish**.

---

## Workflow Rules (Read Carefully!)

###  Rule #1: Do NOT Push Environment Files
Everyone’s drive letters or local configurations might differ. **Never** stage or commit these files:
* `.project` / `.cproject`
* `.bochsrc` (unless we agree on a global change)
* `.settings/` or `.metadata/`

*If you accidentally modify them, reset them using: `git checkout -- .project`*

### Rule #2: Sync Daily
Before starting any work, get the latest changes from the team:
```bash
git checkout main
git pull origin main
```

###  Rule #3: Work on Branches
Never push directly to `main`. Create a branch for your specific task:
```bash
git checkout -b task-name-yourname
# Example: git checkout -b kmalloc-omar
```

###  Rule #4: Test Before You Push
1. Click the **Hammer Icon** in Eclipse to Build.
2. Run the OS and ensure it reaches the `FOS>` prompt.
3. Run the relevant test command (e.g., `test_kmalloc`).
4. Only once it passes, add/commit/push your branch.

---

##  Project Structure
* `kern/kheap.c`: Kernel Heap (kmalloc/kfree)
* `kern/memory_manager.c`: Paging and Address Translation
* `kern/trap.c`: Page Fault Handling
* `inc/memlayout.h`: Memory Map and Constants
  
