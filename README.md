# Spaghetti

<p align="center">
  <img src="https://raw.githubusercontent.com/skulltombstone-hub/Spaghetti/refs/heads/main/app/src/main/res/drawable/butterscotch_logo.png" width="128" />
</p>

<p align="center">
  A universal game runtime launcher for Android.
</p>

## About

Spaghetti is an open-source Android application designed to provide a unified platform for running games created with different engines and frameworks.

The project started with support for GameMaker games through the Butterscotch runtime, focusing on compatibility with older GameMaker releases such as GameMaker Studio 1.x.

The long-term goal of Spaghetti is to become a multi-engine runtime platform capable of supporting several popular game creation tools, allowing users to preserve and play games from different ecosystems on modern devices.

## Current Supported Engines

### ✅ GameMaker

Currently, Spaghetti supports GameMaker-based games through the Butterscotch runtime.

Supported versions and compatibility depend on the capabilities of the runtime itself.

At the moment, this is the only officially supported engine.

## Future Support

Spaghetti is planned to expand support for additional engines and formats:

- RPG Maker XP
- RPG Maker VX
- RPG Maker VX Ace
- RPG Maker MV
- Shockwave Flash / Adobe Flash
- HTML-based games
- Ren'Py (very long-term goal)

Some of these platforms will require dedicated runtimes or compatibility layers.

## Ren'Py Support

Ren'Py is a special case in the roadmap.

Unlike some other engines, Ren'Py cannot simply be added through a small compatibility layer. Supporting it would require the creation of a new and independent runtime capable of interpreting Ren'Py projects on Android.

Because of this, Ren'Py support is considered a very long-term objective.

## Project Structure

Spaghetti works together with external runtimes.

The Android application is responsible for:

- Providing the user interface
- Managing game files
- Handling Android-specific features
- Loading compatible runtimes

The actual game execution is handled by engine-specific runtimes.

Currently:

```text
Spaghetti
└── Butterscotch Runtime
    └── GameMaker support
```

Future versions may include additional runtimes:

```text
Spaghetti
├── Butterscotch
│   └── GameMaker
│
├── RPGMaker Runtime
│   ├── XP
│   ├── VX
│   ├── VX Ace
│   └── MV
│
├── Flash Runtime
│   └── Shockwave Flash / Adobe Flash
│
├── HTML Runtime
│
└── Ren'Py Runtime (future)
```

## Goals

The main goals of Spaghetti are:

- Preserve old games and interactive experiences
- Provide modern Android compatibility for older engines
- Create a unified launcher for multiple game development platforms
- Avoid requiring the original development environment to run games
- Keep old projects accessible for future generations

## Development Status

Spaghetti is currently under active development.

The project is still in an early stage, and many planned features are not implemented yet.

Current focus:

- Improving GameMaker compatibility
- Stabilizing the Android application
- Improving runtime integration
- Preparing the architecture for future engines

## Contributing

Contributions are welcome!

You can help by:

- Improving compatibility  
  *(Please contact me via the email address on my profile page.)*

- Fixing bugs  
  *(Please contact me via the email address on my profile page.)*

- Testing games  
  *(If you discover a bug, open an issue and be careful with spam replies.)*

- Developing new runtime modules  
  *(Please contact me via the email address on my profile page.)*

- Improving documentation  
  *(Please contact me via the email address on my profile page.)*

Before contributing, please check the project structure and existing issues.

## Acknowledgements

Special thanks to the developers and contributors of the open-source projects that make Spaghetti possible.

Spaghetti would not exist without the work of the communities preserving old game engines and formats.
