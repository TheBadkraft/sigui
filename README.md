# sigui - Structured Immediate-mode GUI  
A lightweight, immediate-mode GUI library built in C, designed for simplicity and extensibility. `sigui` uses an event-driven architecture with modules, commands, and a flexible rendering pipeline, currently transitioning from console output to an OpenGL backend.

## Features
- Immediate-mode rendering: UI redraws each frame based on input state.
- Event/command system: Handles mouse/keyboard input and actions.
- Modular design: Add custom modules with render and event handlers.
- Current Backend: Console (Sprint 5); OpenGL with SDL in progress (Sprint 6).

## Getting Started  
### Prerequisites  
- C compiler (e.g., `gcc`)
- [`sigcore` library][1] (custom memory/list/string utilities)
- **SDL2** (for Sprint 6+ OpenGL backend): `sudo apt install libsdl2-dev` (Linux) or equivalent
- **OpenGL** (included with SDL2 or OS)

### Build  
``` bash
make               # Builds the library
make main          # Builds the library and main executable
make test_<name>   # Runs unit tests (test_context, test_dispatcher, etc.)
make clean         # Cleans build artifacts
```

### Run  
``` bash
./bin/main         # Runs main.c (console output until Sprint 6 complete)
```

### Project Structure  
- `src/`:     Core source files(`sigui.c`, `dispatcher.c`, `render.c` [**Sprint 6**])
- `include/`: Headers(`sigui.h`, *`sigui_test.h*`* [**Sprint 6**])
- `test/`:    Unit tests(`test_context.c`, `test_dispatcher.c`, `test_rendering.c` [**Sprint 6**])
- `main.c`:   Functional demo (evolving into **SDL** + **OpenGL** window [**Sprint 6**])

### Status  
See [Current Status][2] for sprint progress and task list.  
Also, check out the [To-Do List][3] for ways to get involved.  

### Contributing  
- Define test cases before coding (see *[Current Status]*[2] for process).
- Submit pull requests with tests and documentation updates prior to adding features, enhancements, etc.

### License  
[GNU GENERAL PUBLIC LICENSE][3]
- Version 3, 29 June 2007

[1]: https://github.com/TheBadkraft/sigcore  
[2]: https://github.com/TheBadkraft/sigui/blob/main/docs/current_status.md  
[3]: https://github.com/TheBadkraft/gigui/blob/main/docs/to_do_list.md  
[4]: https://github.com/TheBadkraft/sigui/blob/main/LICENSE
