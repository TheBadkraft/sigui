# Current Status - sigui
Tracking sprints, tasks, and progress for the `sigui` project. Sprints follow an agile process with 0-2 tasks per iteration, locking features before adding new ones. Test cases are defined upfront to validate API design and behavior.

## Sprint 6 (In Progress)
**Start Date**: 3/16/2025  
**Tasks**:  

### 1. Task: Expand Input Struct (Front-End)  
- **Description**: Enhance `ui_input` to support multiple mouse buttons (bitmask) and keyboard keys (array/bitfield) for OpenGL input.
- **Effort**: Low-Medium | **Impact**: High | **Status**: Partially started (Sprint 5 added `mouse_button`).
- **A. Test Case(s):**
  - **Test 1: Multi-Button Detection**
    - Signature: `void test_multi_button_input(void)`
    - Result: `ui_input.button` mask (e.g., `MOUSE_BUTTON_LEFT | MOUSE_BUTTON_RIGHT`) triggers `EVENT_MOUSE_PRESS` for each button.
  - **Test 2: Multi-Key Input**
    - Signature: `void test_multi_key_input(void)`
    - Result: Multiple keys (e.g., space + ‘A’) trigger `EVENT_KEY_PRESS` events.
- **B. Implementations**: Pending

### 2. Task: Rendering Engine (Includes Task 5: Real Rendering Backend - OpenGL) (Back-End)  
- **Description**: Build an OpenGL rendering engine with SDL for window/context setup, replacing `printf` with OpenGL draw calls.
- **Note**: SDL + OpenGL chosen for cross-platform window/input simplicity; all rendering is pure OpenGL, ensuring no loss of graphics capability.
- **Effort**: Medium-High | **Impact**: High | **Status**: Planned.
- **A. Test Case(s):**
  - **Test 1: Engine Initialization (Unit)**
    - Signature: `void test_render_engine_init(void)`
    - API: `int render_init(int width, int height)`
    - Result: Mocks SDL calls (`SDL_CreateWindow`, `SDL_GL_CreateContext`), asserts success.
  - **Test 2: Basic Rendering (Functional)**
    - Via `main.c`: Opens 800x600 window, draws `MainWindow` as an OpenGL quad.
- **B. Implementations**: Pending
