# Contributing

Thanks for contributing! Please follow this simple workflow to keep the project consistent and reviewable.

1. Fork or clone the repository and create a feature branch:

```bash
git clone https://github.com/EmbedDevWhiz/STM32_Audio_MFCC.git
cd STM32_Audio_MFCC
git checkout -b feature/<short-description>
```

2. Make small, focused commits. Use the project's commit message convention:

```
<type>(<scope>): <short summary>
```

Common types: `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `chore`.

3. Test on the target hardware (B-U585I-IOT02A) or with the provided simulation steps where applicable. Document test steps in your PR.

4. Push your branch and open a Pull Request against `main`.

```bash
git push -u origin feature/<short-description>
```

5. Keep your branch up to date with `main` while your PR is open:

```bash
git fetch origin
git checkout feature/<short-description>
git rebase origin/main
git push --force-with-lease
```

6. In your PR include: a one-line summary, implementation details, testing steps, and the board used.

Code style:
- Keep changes minimal and focused.
- Prefer clear, descriptive identifiers.

If you need help, ask the maintainer of this repo.
