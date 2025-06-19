# Guide Lines

## 1. Use Clear and Descriptive Names

- Use meaningful names for variables, functions, and classes.
- Avoid abbreviations unless they are well-known and widely accepted.
- Use **camelCase** for variable names.
- Use **snake_case** for function names.
- Use **PascalCase** for class/structure names.
- For member variables:
  - Use an underscore (`_`) prefix.
  - Prefix with lowercase `g` for global variables (e.g., `g_globalVar`).
  - Prefix with lowercase `s` for static variables (e.g., `s_staticVar`).
- For constants, use uppercase letters with underscores (e.g., `MAX_SIZE`).
- For constant or reference parameters, use `const&` after the type (e.g., `std::vector<int> const& param1`).
- For lists, use plural nouns (e.g., `items`, `users`).

## 2. Maintain Consistent Code Style

- Align your code according to indentation and brace style conventions.
- Put `{` on a new line for blocks, functions, and class definitions.
- Group related variables together.

## 3. Separate Interface from Implementation

- Use header files (`.h`) for declarations.
- Use source files (`.cpp`) for implementations.
- Use `.inl` files if separating template implementations from headers.

## 4. Use RAII and Smart Pointers

- Prefer `std::unique_ptr` or `std::shared_ptr` over raw pointers.
- Manage resources (memory, files, etc.) using RAII principles.

## 5. Use `const` and `constexpr` Where Possible

- Mark variables and member functions `const` when they don’t modify state.
- Use `constexpr` for compile-time constants.

