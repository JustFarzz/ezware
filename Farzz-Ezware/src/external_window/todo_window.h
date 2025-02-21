#pragma once

#include "../../include/imgui/imgui.h"

#include "../../include/sqlite/sqlite3.h"

#include <string>
#include <vector>

using namespace std;

struct Subtask {
    int id;
    int parent_id;
    string text;
    bool completed;
};

struct Task {
    int id;
    string text;
    bool completed;
    vector<Subtask> subtasks;
    bool show_subtasks;
};

class TodoWindow {
public:
    TodoWindow();
    ~TodoWindow();
    void Show(bool* p_open);

private:
    sqlite3* db;
    vector<Task> tasks;
    char input_text[256];
    char subtask_input[256];
    int selected_task;

    bool InitializeDatabase();
    void LoadTasks();
    void AddTask(const string& text);
    void AddSubtask(int parent_id, const std::string& text);
    void DeleteTask(int id);
    void DeleteSubtask(int id);
    void ToggleTask(int id, bool completed);
    void ToggleSubtask(int id, bool completed);
    void RefreshTasks();
};