#### Tasks To Do  
1. **[Medium Priority] TASK: Main Loop Implementation**
   - Decide if loop lives in app or engine.
   - **Effort**: Low-Medium | **Impact**: Medium
2. **[Medium Priority] Module Retains Context Reference**
   - Add `ui_context` to `ui_module`.
   - **Effort**: Low | **Impact**: Medium
3. **[Medium Priority] Widget System (e.g., Buttons)**
   - Add clickable widgets.
   - **Effort**: Medium | **Impact**: High
4. **[Low Priority] Multi-Window Support**
   - Multiple windows per context.
   - **Effort**: Medium | **Impact**: Medium
5. **[Medium Priority] Event Targeting and Dispatch Timing**
   - Use `event_info->target`.
   - **Effort**: Medium | **Impact**: Medium-High | **Status**: Partially done (Sprint 5)
6. **[Low Priority] Flexible Module Rendering Output**
   - Generalize beyond windows.
   - **Effort**: Low-Medium | **Impact**: Medium
7. **[Medium Priority] Selective Event Dispatching**
    - Dispatch to interested modules.
    - **Effort**: Medium | **Impact**: High

#### Addendum (Completed Tasks)  
- **[Sprint 5] Extend `add_module` to Accept `event_handler`**
- **[Sprint 5] Introduce `event_info` Struct**
- **[Sprint 1-4] Core Event/Command Pipeline**

#### Process  
- **Test-Driven**: Define test cases before coding:
  1. Test the feature/fix (API signature, params, flow).
  2. Determine expected result (e.g., event counts, output).
- **Unit Tests**: Focus on API design/logic (e.g., `test_dispatcher.c`); mock dependencies (e.g., SDL via `sigui_test.h`).
- **Functional Tests**: Validate rendering via `main.c` (e.g., OpenGL window).
- **Tasks per Sprint**: 0-2, locked before adding more.
