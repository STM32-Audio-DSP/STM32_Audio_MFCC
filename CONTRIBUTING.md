# Contributing

Thanks for contributing! Please follow this workflow to keep the project consistent and reviewable.

### 1. Setup your workspace
Clone the repository and create your feature branch:
```bash
git clone https://github.com/EmbedDevWhiz/STM32_Audio_MFCC.git
cd STM32_Audio_MFCC
git checkout -b feature/<short-description>
```

### 2. Make your changes
Make small, focused commits. Use the project's commit message convention:
```
<type>(<scope>): <short summary>
```
*Common types: `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `chore`.*

### 3. Push your branch
After you have committed your changes locally, push them to the server:
```bash
git push -u origin feature/<short-description>
```

### 4. Create your Pull Request (The "Review" Step)
Since you are new to this, the easiest way to submit your code for review is through the web browser:
1. After pushing, go to the repository page on GitHub in your browser.
2. You will see a yellow banner near the top saying **"feature/<short-description> had recent pushes..."**.
3. Click the green **"Compare & pull request"** button.
4. **Fill out the PR form:**
    * **Title:** A clear, concise name for your change.
    * **Summary:** One line describing your work.
    * **Implementation Details:** Briefly explain how you solved the task.
    * **Testing Steps:** List how you verified it works on the **B-U585I-IOT02A** board.
5. Click **"Create pull request"**.

*Your PR is now submitted! The instructor will review your code and may leave comments. You can check back here to see if your code was approved or if you need to make adjustments.*

### 5. Keep your branch up to date
If you need to update your branch while your PR is still open:
```bash
git fetch origin
git checkout feature/<short-description>
git rebase origin/main
git push --force-with-lease
```

---------------------------------------------------------------------------
**Code Style & Help:**
* Keep changes minimal and focused.
* Use clear, descriptive variable names.
* If you need help, don't hesitate to ask the instructor/maintainer!